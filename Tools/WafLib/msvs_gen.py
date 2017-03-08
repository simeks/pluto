from waflib.extras import msvs
from waflib.Configure import conf
from waflib import Utils

import os

@conf
def platform_vs_to_waf(conf, p):
	if p == 'Win32':
		return 'win32'
	if p == 'x64':
		return 'win64'

@conf
def configuration_vs_to_waf(conf, p):
	if p == 'Debug':
		return 'debug'
	if p == 'Release':
		return 'release'

@conf
def configuration_waf_to_vs(conf, p):
	if p == 'debug':
		return 'Debug'
	if p == 'release':
		return 'Release'

class vsnode_target(msvs.vsnode_target):
	def get_build_command(self, props):
		p = self.get_build_params(props)
		return "%s build_%s_%s %s" % (p[0], self.ctx.platform_vs_to_waf(props.platform), self.ctx.configuration_vs_to_waf(props.configuration), p[1])
	def get_clean_command(self, props):
		p = self.get_build_params(props)
		v = self.ctx.platform_vs_to_waf(props.platform) + '_' + self.ctx.configuration_vs_to_waf(props.configuration)
		return "%s clean_%s %s" % (p[0], v, p[1])
	def get_rebuild_command(self, props):
		p = self.get_build_params(props)
		v = self.ctx.platform_vs_to_waf(props.platform) + '_' + self.ctx.configuration_vs_to_waf(props.configuration)
		return "%s clean_%s build_%s %s" % (p[0], v, v, p[1])
	
	def collect_source(self):
		tg = self.tg
		source_files = tg.to_nodes(getattr(tg, 'source', []))

		include_files = []
		lst = [y for y in tg.path.ant_glob('**/(*.h|*.hpp|*.H|*.inl)', flat=False)]
		include_files.extend(lst)

		# remove duplicates
		self.source.extend(list(set(source_files + include_files)))
		self.source.sort(key=lambda x: x.win32path())

	def collect_properties(self):
		"""
		Visual studio projects are associated with platforms and configurations (for building especially)
		"""
		super(vsnode_target, self).collect_properties()
		for x in self.build_properties:
			variant = '%s_%s' % (self.ctx.platform_vs_to_waf(x.platform), self.ctx.configuration_vs_to_waf(x.configuration))
			v = self.ctx.all_envs[variant]

			outdir = self.ctx.get_output_dir(self.ctx.platform_vs_to_waf(x.platform), self.ctx.configuration_vs_to_waf(x.configuration))

			x.outdir = outdir.abspath()
			x.preprocessor_definitions = ''
			x.includes_search_path = ''

			try:
				tsk = self.tg.link_task
			except AttributeError:
				pass
			else:
				x.output_file = os.path.join(x.outdir, tsk.outputs[0].win32path().split(os.sep)[-1])
				x.preprocessor_definitions = ';'.join(v.DEFINES)
				x.includes_search_path = ';'.join(self.tg.env.INCPATHS)	

class vsnode_build_all(msvs.vsnode_build_all):
	"""
	Fake target used to emulate the behaviour of "make all" (starting one process by target is slow)
	This is the only alias enabled by default
	"""
	def __init__(self, ctx, node, name='build_all_projects'):
		msvs.vsnode_build_all.__init__(self, ctx, node, name)
		self.is_active = True

	def get_build_command(self, props):
		p = self.get_build_params(props)
		return "%s build_%s_%s %s" % (p[0], self.ctx.platform_vs_to_waf(props.platform), self.ctx.configuration_vs_to_waf(props.configuration), p[1])
	def get_clean_command(self, props):
		p = self.get_build_params(props)
		v = self.ctx.platform_vs_to_waf(props.platform) + '_' + self.ctx.configuration_vs_to_waf(props.configuration)
		return "%s clean_%s %s" % (p[0], v, p[1])
	def get_rebuild_command(self, props):
		p = self.get_build_params(props)
		v = self.ctx.platform_vs_to_waf(props.platform) + '_' + self.ctx.configuration_vs_to_waf(props.configuration)
		return "%s clean_%s build_%s %s" % (p[0], v, v, p[1])

class vsnode_install_all(msvs.vsnode_install_all):
	"""
	Fake target used to emulate the behaviour of "make install"
	"""
	def __init__(self, ctx, node, name='install_all_projects'):
		msvs.vsnode_install_all.__init__(self, ctx, node, name)

	def get_build_command(self, props):
		p = self.get_build_params(props)
		return "%s build_%s_%s %s" % (p[0], self.ctx.platform_vs_to_waf(props.platform), self.ctx.configuration_vs_to_waf(props.configuration), p[1])
	def get_clean_command(self, props):
		p = self.get_build_params(props)
		v = self.ctx.platform_vs_to_waf(props.platform) + '_' + self.ctx.configuration_vs_to_waf(props.configuration)
		return "%s clean_%s %s" % (p[0], v, p[1])
	def get_rebuild_command(self, props):
		p = self.get_build_params(props)
		v = self.ctx.platform_vs_to_waf(props.platform) + '_' + self.ctx.configuration_vs_to_waf(props.configuration)
		return "%s clean_%s build_%s %s" % (p[0], v, v, p[1])

class msvs_2013(msvs.msvs_generator):
	cmd = 'msvs2013'
	numver = '13.00'
	vsver = '2013'
	platform_toolset_ver = 'v120'
	def init(self):
		msvs.msvs_generator.init(self)
		self.vsnode_target = vsnode_target
		self.vsnode_build_all = vsnode_build_all
		self.vsnode_install_all = vsnode_install_all

	def collect_dirs(self):
		"""
		Create the folder structure in the Visual studio project view
		"""

		root = None
		for p in self.all_projects[:]: # iterate over a copy of all projects
			if not getattr(p, 'tg', None):
				# but only projects that have a task generator
				continue

			# make a folder for each task generator
			x = p.tg.path

			if root == None:
				root = self.vsnode_vsdir(self, msvs.make_uuid('Source'), 'Source')
				self.all_projects.append(root)

			if p.parent == None:
				p.parent = root

class msvs_2015(msvs.msvs_generator):
	cmd = 'msvs2015'
	numver = '14.00'
	vsver = '2015'
	platform_toolset_ver = 'v140'
	def init(self):
		msvs.msvs_generator.init(self)
		self.vsnode_target = vsnode_target
		self.vsnode_build_all = vsnode_build_all
		self.vsnode_install_all = vsnode_install_all

	def collect_dirs(self):
		"""
		Create the folder structure in the Visual studio project view
		"""

		root = None
		for p in self.all_projects[:]: # iterate over a copy of all projects
			if not getattr(p, 'tg', None):
				# but only projects that have a task generator
				continue

			# make a folder for each task generator
			x = p.tg.path

			if root == None:
				root = self.vsnode_vsdir(self, msvs.make_uuid('Source'), 'Source')
				self.all_projects.append(root)

			if p.parent == None:
				p.parent = root

class msvs_2017(msvs.msvs_generator):
	cmd = 'msvs2017'
	numver = '14.10'
	vsver = '2017'
	platform_toolset_ver = 'v141'
	def init(self):
		msvs.msvs_generator.init(self)
		self.vsnode_target = vsnode_target
		self.vsnode_build_all = vsnode_build_all
		self.vsnode_install_all = vsnode_install_all

	def collect_dirs(self):
		"""
		Create the folder structure in the Visual studio project view
		"""

		root = None
		for p in self.all_projects[:]: # iterate over a copy of all projects
			if not getattr(p, 'tg', None):
				# but only projects that have a task generator
				continue

			# make a folder for each task generator
			x = p.tg.path

			if root == None:
				root = self.vsnode_vsdir(self, msvs.make_uuid('Source'), 'Source')
				self.all_projects.append(root)

			if p.parent == None:
				p.parent = root

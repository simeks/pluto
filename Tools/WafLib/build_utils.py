from waflib import Utils
from waflib.Task import Task, RUN_ME, SKIP_ME
from waflib.TaskGen import after_method, before_method, feature, extension, taskgen_method
from waflib.Configure import conf

import sys, os, time, shutil

bin_dir = {
	'win64': 'Bin/Win64',
	'linux_x64_gcc': 'Bin/Linux64_GCC',
	'linux_x64_clang': 'Bin/Linux64_Clang'
}

@conf
def get_output_dir(self, platform, configuration):
	out = bin_dir[platform]
	if configuration == 'debug':
		out = out + '_Debug'

	out_node = self.path.make_node(out)
	return out_node


@feature('c', 'cxx')
@before_method('apply_manifest')
@after_method('apply_link')
def copy_bins(self):
	if self.bld.platform == "msvs":
		return

	if not hasattr(self, 'link_task'):
		return

	if self.typ == 'stlib': # Do not copy static libs
		return

	for src in self.link_task.outputs:
		tgt = self.bld.get_output_dir(self.env['PLATFORM'], self.env['CONFIGURATION']).make_node(os.path.basename(src.abspath()))
		self.create_task('copy_outputs', src, tgt)


class copy_outputs(Task):
	color = 'YELLOW'

	def run(self):
		src = self.inputs[0].abspath()
		tgt = self.outputs[0].abspath()

		dir = os.path.dirname(tgt)
		try:
			os.makedirs(os.path.dirname(tgt))		
		except OSError:
			pass

		shutil.copy2(src, tgt)

		return 0

	def runnable_status(self):
		if super(copy_outputs, self).runnable_status() == -1:
			return -1

		src = self.inputs[0].abspath()
		tgt = self.outputs[0].abspath()

		try:
			stat_tgt = os.stat(tgt)
			stat_src = os.stat(src)
			if stat_src.st_mtime >= stat_tgt.st_mtime + 2 or stat_src.st_size != stat_tgt.st_size:
				return RUN_ME			
		except OSError:
			return RUN_ME

		# Everything fine, we can skip this task
		return SKIP_ME

@feature('c', 'cxx')
@after_method('process_source')
def apply_unique_pdb_per_target(self):
	"""For msvc, specify a unique compile pdb per target, to work around
	LNK4099 in case multiple independent targets build in the same
	directory, e.g. test programs. CFLAGS are updated with a unique
	/Fd flag based on the target name. This is separate from the link pdb.
	"""
	if self.env.CC_NAME != 'msvc':
		return

	debug = False
	fd = False
	for f in (self.env['CFLAGS'] + self.env['CXXFLAGS']):
		fl = f.lower()
		if fl[1:].lower() == 'zi': # /Zi, /ZI enable debug
			debug = True
		elif fl[1:3] == 'fd':	   # /Fd, file or directory already provided
			fd = True

	if debug and not fd:
		pdb = "%s.pdb" % self.target
		self.env.append_value('CFLAGS', '/Fd%s' % pdb)
		self.env.append_value('CXXFLAGS', '/Fd%s' % pdb)


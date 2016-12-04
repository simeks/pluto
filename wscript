APPNAME = 'Pluto'
VERSION = '0.0.1'

SUBFOLDERS = ['Source']

top = '.'
out = 'Build'

import os, shutil
from waflib import Utils, Task
from waflib.TaskGen import extension, before_method, after_method, feature
from waflib.Configure import conf
from waflib.Node import Node

CONFIGURATIONS = ['debug', 'release']
PLATFORMS = {
	'win32' : ['win64'],
	'linux' : ['linux_x64_gcc', 'linux_x64_clang']
}

def supported_platforms():
	p = Utils.unversioned_sys_platform()
	return PLATFORMS[p]

QT5_MODULES = ['Qt5Core', 'Qt5Gui', 'Qt5Widgets']

def options(opt):
	opt.load('compiler_cxx python qt5 msvs')
	opt.load('compile_settings', tooldir='Tools/WafLib')
	opt.load('msvs_gen', tooldir='Tools/WafLib')

def configure(conf):
	v = conf.env
	v.MSVC_VERSIONS = ['msvc 14.0']# 'msvc 12.0']

	conf.load('compiler_cxx python msvs')
	conf.load('compile_settings', tooldir='Tools/WafLib')
	conf.load('msvs_gen', tooldir='Tools/WafLib')

	# Qt5
	conf.load('qt5')
	
	v.LIB_QT5 = []
	v.LIB_QT5_DEBUG = []
	v.LIBPATH_QT5 = []
	v.LIBPATH_QT5_DEBUG = []
	v.INCLUDES_QT5 = []
	v.INCLUDES_QT5_DEBUG = []
	v.DEFINES_QT5 = []
	v.DEFINES_QT5_DEBUG = []

	for m in QT5_MODULES:
		v.LIB_QT5 += v['LIB_%s' % m.upper()]
		v.LIB_QT5_DEBUG += v['LIB_%s_DEBUG' % m.upper()]
		v.INCLUDES_QT5 += v['INCLUDES_%s' % m.upper()]
		v.INCLUDES_QT5_DEBUG += v.INCLUDES_QT5 # v['INCLUDES_%s' % m.upper()]
		v.DEFINES_QT5 += v['DEFINES_%s' % m.upper()]
		v.DEFINES_QT5_DEBUG += v['DEFINES_%s_DEBUG' % m.upper()]
		v.LIBPATH_QT5 += v['LIBPATH_%s' % m.upper()]
		v.LIBPATH_QT5_DEBUG += v['LIBPATH_%s_DEBUG' % m.upper()]

	v.RPATH += ['.']
	v.RPATH += v.LIBPATH_QT5

	# Python
	conf.check_python_version()
	conf.check_python_headers('pyembed')

	# Numpy
	numpy_inc_path = ''
	for p in [__import__('numpy').get_include(), os.path.join(v.PYTHONDIR, 'numpy', 'core', 'include')]:
		if os.path.isfile(os.path.join(p, 'numpy', 'arrayobject.h')):
			numpy_inc_path = p

	if numpy_inc_path == '':
		conf.fatal('Failed to determine include path for numpy.')

	v.INCLUDES_NUMPY = [numpy_inc_path]
	v.LIBPATH_NUMPY = v.LIBPATH_PYEMBED

	# Platform specific setup

	variant_configure = {
		'win64_debug': conf.configure_msvc_x64_debug,
		'win64_release': conf.configure_msvc_x64_release,
		'linux_x64_gcc_release': conf.configure_gcc_x64_release,
		'linux_x64_clang_release': conf.configure_clang_x64_release,
	}

	for p in supported_platforms():
		for c in CONFIGURATIONS:
			v = p + '_' + c
			conf.setenv(v, env=conf.env.derive().detach()) # Make sure to make a deep copy of base env
			if v in variant_configure:			
				variant_configure[v]()
			else:			
				print('No configuration set for variant %s' % v)
			conf.setenv('')

	conf.load('build_utils', tooldir='Tools/WafLib')
	
	conf.recurse(SUBFOLDERS, mandatory=False)

def build(bld):
	v = bld.env
	if bld.cmd in ['msvs2013', 'msvs2015']:
		print('Generating MSVS files')
		bld.solution_name = APPNAME + '.sln'
		bld.configurations = [bld.configuration_waf_to_vs(c) for c in CONFIGURATIONS]
		bld.platforms = ['x64']
		bld.projects_dir = bld.srcnode.make_node('.depproj')
		bld.projects_dir.mkdir()

		bld.platform = v.PLATFORM = 'msvs'
		bld.configuration = v.CONFIGURATION = ''
	else:
		if not bld.variant:
			# A variant needs to be specified, the variant is of the form "<platform>_<configuration>"
			bld.fatal('No variant specified, read the comments in the wscript file!')

		v.PLATFORM = bld.platform
		v.CONFIGURATION = bld.configuration

		print('Variant: %s' % bld.variant)
	bld.recurse(SUBFOLDERS, mandatory=False)



def init(ctx):
	from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext

	for p in supported_platforms():
		for c in CONFIGURATIONS:
			for x in (BuildContext, CleanContext, InstallContext, UninstallContext):
				name = x.__name__.replace('Context','').lower()
				class tmp(x):
					cmd = name + '_' + p + '_' + c
					variant = p + '_' + c
					platform = p
					configuration = c


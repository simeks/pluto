from waflib.Configure import conf

def_prefix = 'PLUTO_'

@conf
def configure_msvc_x64_common(conf):
	flags = [
		'/FS', '/WX', '/W4', '/MP',
		'/EHsc',
		'/wd4127', # C4127: conditional expression is constant.
		'/wd4251', # C4251: * needs to have dll-interface to be used by clients of class '*'.
		'/openmp',
		]

	v = conf.env
	v.CFLAGS += flags
	v.CXXFLAGS += flags

	v.DEFINES += [
		'_WIN32', 
		'_WIN64', 
		def_prefix+'PLATFORM_WINDOWS', 
		def_prefix+'PLATFORM_WIN64', 
		'_UNICODE', 
		'UNICODE',
		'_CRT_SECURE_NO_WARNINGS',
		'_SCL_SECURE_NO_DEPRECATE',
	]
	v.LINKFLAGS += [ '/MACHINE:X64' ]
	v.LIB += ["kernel32", "user32", "gdi32", "comdlg32", "advapi32", "Ws2_32", "psapi", "Rpcrt4", "Shell32", "Ole32"]
	
	v.CUDAFLAGS += ['--use-local-env', '--cl-version=2013', '--machine=64', '--compile', '-Xcudafe="--diag_suppress=field_without_dll_interface"']

@conf
def configure_msvc_x64_debug(conf):
	configure_msvc_x64_common(conf)
	flags = ['/MDd', '/Od', '/Zi']

	v = conf.env
	v.CFLAGS += flags
	v.CXXFLAGS += flags
	v.DEFINES += ['DEBUG', '_DEBUG', def_prefix+'BUILD_DEBUG']
	v.CUDAFLAGS += ['-G', '-g', '-Xcompiler="'+' '.join(v.CXXFLAGS)+'"']
	v.LINKFLAGS += ['/DEBUG']

@conf
def configure_msvc_x64_release(conf):
	configure_msvc_x64_common(conf)
	flags = ['/MD', '/O2']

	v = conf.env
	v.CFLAGS += flags
	v.CXXFLAGS += flags
	v.DEFINES += ['NDEBUG', def_prefix+'BUILD_RELEASE']
	v.CUDAFLAGS += ['-Xcompiler="'+' '.join(v.CXXFLAGS)+'"']


@conf
def configure_gcc_x64_common(conf):
	flags = [
		'-m64', '-Werror', '-Wall', '-std=c++11', '-fopenmp',
		'-Wno-unused-variable',
		'-Wno-switch',
		]

	v = conf.env
	v.CC = 'gcc'
	v.CXX = 'g++'
	v.CFLAGS += flags
	v.CXXFLAGS += flags
	v.LINKFLAGS += [ '-fopenmp' ]

	v.DEFINES += [
		def_prefix+'_PLATFORM_LINUX', 
		'_UNICODE', 
		'UNICODE',
	]

@conf
def configure_gcc_x64_release(conf):
	configure_gcc_x64_common(conf)
	flags = ['-O2']

	v = conf.env
	v.CFLAGS += flags
	v.CXXFLAGS += flags
	v.DEFINES += ['NDEBUG', def_prefix+'BUILD_RELEASE']

@conf
def configure_clang_x64_common(conf):
	flags = [
		'-m64', '-Werror', '-Wall', '-std=c++11', '-openmp',
		'-Wno-inconsistent-missing-override',
		'-Wno-switch',
		]

	v = conf.env
	v.CC = 'clang'
	v.CXX = 'clang++'
	v.CFLAGS += flags
	v.CXXFLAGS += flags
	v.LINKFLAGS += [ '-fopenmp' ]

	v.DEFINES += [
		def_prefix+'PLATFORM_LINUX', 
		'_UNICODE', 
		'UNICODE',
	]

@conf
def configure_clang_x64_release(conf):
	configure_clang_x64_common(conf)
	flags = ['-O2']

	v = conf.env
	v.CFLAGS += flags
	v.CXXFLAGS += flags
	v.DEFINES += ['NDEBUG', def_prefix+'BUILD_RELEASE']


from glob import glob

auto_libs = ''

# Detect the system's endianness
from sys import byteorder
if byteorder == 'big':
	endian = '_NXT_BIG_ENDIAN'
else:
	endian = '_NXT_LITTLE_ENDIAN'

BuildEnv = Environment(CCFLAGS=['-Wall',
				'-g', '-ggdb', '-D' + endian])
if auto_libs:
	BuildEnv.ParseConfig('pkg-config --cflags --libs ' + auto_libs)

Default(BuildEnv.Library('lestat',
			 [x for x in glob('*.cpp')
			  if not x.startswith('main_')],
			 LIBS=['usb','bluetooth']))


Default(BuildEnv.Program('test', 'main_test.cpp',
			 LIBS=['usb','bluetooth', 'lestat'], LIBPATH='.'))

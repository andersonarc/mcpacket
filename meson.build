# mcpacket library build file

# project definition
project('mcpacket', 'c', 
    version: '0.6',
    license: 'GPL3')

# find required programs
python = find_program('python')

# pip dependencies
dependencies = custom_target('dependencies',
    input: 'mcd2packet/requirements.txt', 
    output: 'requirements.lock', 
    command: [python, '-m', 'pip', 'install', '-r', '@INPUT@'])

# generated sources and headers
protocol = custom_target('protocol', 
    build_by_default: true,
    input: 'mcd2packet/mcd2packet.py', 
    output: ['protocol.c', 'protocol.h', 'particle.h'],
    depends: [dependencies],
    command: [python, '@INPUT@'])

# get dependencies
csafe = dependency('csafe', 
    fallback: ['csafe', 'csafe_dep'])

# prepare build files
miniz = files('src/lib/miniz.c')
src = files('src/nbt.c', 'src/handler.c', 'src/codec.c', 'src/buffer.c', 'src/varnum.c')
include = include_directories('include')

# compile library
libmcpacket = library('mcpacket', [src, protocol, miniz],
    include_directories: include,
    dependencies: [csafe],
    c_args: ['-Wno-switch'])

# create a dependency
libmcpacket_dep = declare_dependency(
    include_directories: include, 
    link_with: libmcpacket)
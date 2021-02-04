# mcpacket library build file

# project definition
project('mcpacket', 'c', 
    version: '0.4',
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
    output: ['protocol.c', 'protocol.h', 'particles.h'],
    depends: [dependencies],
    command: [python, '@INPUT@'])

# prepare build files
src = files('src/types.c', 'src/misc.c', 'src/nbt.c', 'src/handler.c')
include = include_directories('include')

# compile library
libmcpacket = library('mcpacket', [src, protocol],
    include_directories: include)

# create a dependency
libmcpacket_dep = declare_dependency(
    include_directories: include, 
    link_with: libmcpacket)
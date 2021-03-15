# mcpacket library build file

# project definition
project('mcpacket', 'c', 
    version: '0.9',
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

# C arguments
c_args = ['-Wno-switch']

# get dependencies
csafe = dependency('csafe', 
    fallback: ['csafe', 'libcsafe_dep'])

# zlib / libdeflate workaround
zlib = meson.get_compiler('c').find_library('deflate', required: false, has_headers: ['libdeflate.h'])
if not zlib.found()
    message('libdeflate not found, falling back to zlib')
    c_args += '-DMCP_USE_ZLIB'
    zlib = dependency('zlib')
endif


# prepare build files
src = files('src/handler.c', 'src/codec.c', 'src/io/stream.c', 'src/connection.c')
include = include_directories('include')

# compile library
libmcpacket = library('mcpacket', [src, protocol],
    include_directories: include,
    dependencies: [csafe, zlib],
    c_args: c_args)

# create a dependency
libmcpacket_dep = declare_dependency(
    include_directories: include, 
    link_with: libmcpacket)
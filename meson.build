# mcpacket library build file

## find required programs
python = find_program('python')

## generate build files

### pip dependencies
dependencies = custom_target('dependencies',
    input: 'mcd2packet/requirements.txt', 
    output: 'requirements.lock', 
    command: [python, '-m', 'pip', 'install', '-r', '@INPUT@'])

### generated sources and headers
protocol = custom_target('protocol', 
    build_by_default: true,
    input: 'mcd2packet/mcd2packet.py', 
    output: ['protocol.c', 'protocol.h', 'particles.h'],
    depends: [dependencies],
    command: [python, '@INPUT@'])

## prepare build files
mcpacket_src = files('types.c', 'misc.c')
mcpacket_include = include_directories('include')

## compile library
mcpacket = library('mcpacket', [mcpacket_src, protocol],
    include_directories: [mcpacket_include])

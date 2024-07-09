project('lateify', 'c')

src = ['src/main.c', 'src/parser.c']
include_dir = include_directories('include')

libnotify = dependency('libnotify')

executable('lateify', sources : src, dependencies : libnotify, include_directories: include_dir)

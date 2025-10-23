libporthole_deps = []
sources = files('''
	atom.c
	version.c
	use.c
	repo.c
'''.split())
subdir('util')
subdir('db')

# See `config.h.in' for the definitions these map to.
conf_h = configuration_data()
conf_h.set('version', '0.0.1')
conf_h.set('EXTRA_DEBUG', get_option('i_know_precisely_what_i_want'))
conf_h.set('cache_dir', get_option('cache_dir'))

configure_file(input: 'config.h.in',
               output: 'libporthole_config.h',
               configuration: conf_h)

libporthole_include = include_directories('include', '.')
libporthole = library(
	'porthole',
	sources,
	install: true,
	dependencies: libporthole_deps,
	include_directories: libporthole_include,
	gnu_symbol_visibility: 'hidden',
)

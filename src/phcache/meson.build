sources = files('''
	main.c
	cache_spawner.c
'''.split())

executable('phcache',
	sources,
	link_with: libporthole,
	include_directories: libporthole_include)

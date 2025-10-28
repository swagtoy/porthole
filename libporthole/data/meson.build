sqls = '''
	setup.sql
	add_pkg.sql
	ebuild_spawn_proc.sh
'''.split()

sqlgen = []

foreach f : sqls
	sqlgen += custom_target(
	    f'generated-@f@',
	    input:   f,
	    output:  '@PLAINNAME@.h',
	    command: [find_program('xxd'), '-i', '@INPUT@'],
	    capture: true)
endforeach

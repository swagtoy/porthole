sqls = [ 'setup.sql' ]

foreach sql : sqls
	run_command(['sh', '-c', 'xxd -i @0@ > @1@/@0@.h'.format(sql, meson.current_build_dir())])
endforeach

sqls = [ 'setup.sql' ]

sqlgen = custom_target(
	'generated',
	input:   sqls,
	output:  '@PLAINNAME@.h',
	command: [find_program('xxd'), '-i', '@INPUT@'],
	capture: true)

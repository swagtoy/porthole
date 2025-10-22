libs = [ libporthole ]

tests = [
	[ 'String test',      'string_test',      'string_test.c' ],
	[ 'Validation tests', 'validation_tests', 'validation_tests.c' ],
	[ 'Atom tests',       'atom_tests',       'atom_tests.c' ],
#	[ 'Common test',      'common_test',      'common_test.c' ],
	[ 'repos/Everything test', 'repo_everything_test', 'repo_everything_test.c' ],
]

foreach t : tests
	tt = executable(t[1],
		t[2],
		link_with: libporthole,
		include_directories: libporthole_include)
	test(t[0], tt)
endforeach

#include <assert.h>
#include <stdio.h>
#include <str.h>
#include <atom.h>
#include <signal.h>
#include "_debug_prints.h"

#ifndef TESTS_DIR
#	define TESTS_DIR ""
#	error "No tests dir set"
#endif

void
t_dependency_parsing()
{
	ph_atom_t atom;
	ph_atom_parse_opts_t opts = PH_ATOM_PARSE_DEPENDENCY;
	
	//assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[bu]", &atom, opts) == 0);
	//assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[-bu]", &atom, opts) == 0);
	//assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[buz,-bu]", &atom, opts) == 0);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[-be,buz,-bu]", &atom, opts) == 0);
	_atom_debug_print(&atom);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[-be,buz,-bu]", &atom, opts) == 0);
	_atom_debug_print(&atom);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[be?,buz,-bu]", &atom, opts) == 0);
	_atom_debug_print(&atom);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[be(+)?,buz,-bu]", &atom, opts) == 0);
	_atom_debug_print(&atom);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[-be,-buz(-)?,-bu]", &atom, opts) == 0);
	_atom_debug_print(&atom);
	assert(ph_atom_parse_string(">=_.+-0-/_-test-T-123_beta1_-4a-6+-_p--1.00.02b_alpha3_pre_p4-r5:slot/_-+6-9[test(+),test(-)]", &atom, opts) == 0);
	_atom_debug_print(&atom);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[--be,-buz(-)?,-bu]", &atom, opts) != 0);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[--be,-buz()?,-bu]", &atom, opts) != 0);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[--be,-buz(/)?,-bu]", &atom, opts) != 0);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[--be,-buz(/)?,]", &atom, opts) != 0);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[--be,-buz(++)?,]", &atom, opts) != 0);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[--be,-buz(+))?]", &atom, opts) != 0);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[--be,-buz(+))]", &atom, opts) != 0);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[--be,-buz(+,]", &atom, opts) != 0);
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[be,]", &atom, opts) != 0);
	assert(ph_atom_parse_string(">=media-video/knobgoblin-1.3.4.0a_alpha_beta3_p_p20244040_alpha-r3:0/b::wow[!knobbed?,-test(+)?,foogbaab]", &atom, opts) == 0);
	_atom_debug_print(&atom);
	
	assert(ph_atom_parse_string(">=cat/pkg-1.0.0v_alpha2_beta3-rc-1:slot/sub=[!usedep?]::gentoo", &atom, opts) != 0);
	assert(ph_atom_parse_string(">=cat/pkg-1.0.0v_alpha2_beta3-r3:slot/sub::gentoo[!usedep?]", &atom, opts) == 0);
	assert(ph_atom_parse_string(">=cat/pkg-1.0.0v_alpha2_beta3-r3:slot/sub=[!usedep?]::gentoo", &atom, opts) != 0);
	assert(ph_atom_parse_string("!! >=cat/pkg-1.0.0v_alpha2_beta3-r3:slot/sub=[!usedep?]::gentoo", &atom, opts) != 0);
	
	// Test alternative syntax
	opts |= PH_ATOM_PARSE_REPO_AFTER_USEDEP;
	
	puts("--- TESTING ALTERNATIVE SYNTAX ---");
	
	assert(ph_atom_parse_string("something/package-1.0:3.0::gentoo[-be,buz,-bu]", &atom, opts) != 0);
	assert(ph_atom_parse_string(">=media-video/knobgoblin-1.3.4.0a_alpha_beta3_p_p20244040_alpha-r3:0/b::wow[!knobbed?,-test(+)?,foogbaab]", &atom, opts) != 0);
	
	// Now lets see if those work
	assert(ph_atom_parse_string("something/package-1.0:3.0[-be,buz,-bu]::definitelygentoo", &atom, opts) == 0);
	_atom_debug_print(&atom);
	assert(ph_atom_parse_string("something/package-1.0:3.0[-be,buz,-bu]i:definitelygentoo", &atom, opts) != 0);
}

static bool
_is_blanker(char bl)
{
	return bl == ' ' || bl == '\t' || bl == '\0';
}

int
main()
{
	int ret = 0;
	char line[1024];
	FILE *file = fopen(TESTS_DIR "/atom_tests", "r");
	assert(file);
	int res;
	char *pt;
	ph_atom_t _atom;
	while (fgets(line, sizeof(line), file) != 0)
	{
		if (*line == '*' || *line == '\n')
			continue;
		char mode = ' ';
		bool print = false;
		bool trace = false;
		bool not = false;
		if (line[strlen(line)-1] == '\n')
			line[strlen(line)-1] = '\0';
		
		for (pt = line; *pt && *pt != ' '; ++pt)
			switch (*pt)
			{
			case '+': mode = '+'; break;
			case '-': mode = '-'; break;
			case '>': mode = '>'; break;
			case '<': mode = '<'; break;
			case '=': mode = '='; break;
			case '!': not = true; break;
			case 'p': print = true; break; // print atom (debugging)
			case 't': trace = true; break; // trace here
			
			// These are hints for parsers which aren't porthole. We
			//  ignore these, others probably wont.
			case 'r': break; // repo
			case 'v': break; // version
			default: assert(!"Invalid character in atom_tests!");
			}
		
		if (not)
			assert(not && mode == '=');	
		
		char *atomstr = pt + 1;
		if (mode == '>' || mode == '<' || mode == '=')
		{
			char *atomstr2 = pt+1;
			while (!_is_blanker(*atomstr2))
				++atomstr2;
			assert(*atomstr2 == ' ');
			*atomstr2 = '\0';
			++atomstr2;
		
			if (trace) raise(SIGTRAP);

			if (mode == '>')
			{
				//ph_atom_cmp_gt(atomstr, atomstr2, ...);
			}
			else if (mode == '<') {
				//ph_atom_cmp_lt(atomstr, atomstr2, ...);
			}
			else if (mode == '=') {
				if (not)
					;// ph_atom_cmp_eq(atomstr, atomstr2, ...);
					
				else
					;// !ph_atom_cmp_eq(atomstr, atomstr2, ...);
			}
			printf("%s %s%c %s CHECK\n", atomstr, not ? "!" : "", mode, atomstr2);
		}
		else if (mode == '+' || mode == '-') {
			if (trace) raise(SIGTRAP);
			if ((mode == '+' && (res = ph_atom_parse_string(atomstr, &_atom, 0)) != 0) ||
			    (mode == '-' && (res = ph_atom_parse_string(atomstr, &_atom, 0)) == 0))
			{
				fprintf(stderr, "Failed to parse atom \"%s\": returned %d!\n", atomstr, res);
				ret = 1;
			}
			else
				printf("Check \"%s\" passed!\n", atomstr);
		}
		
		if (print)
			_atom_debug_print(&_atom);
	}
	
	// clobber test
	char const *noclobber = ">=media-video/revised-knob-3.0-r15:12/5::wow";
	assert(ph_atom_parse_string(noclobber, &_atom, 0) == 0);
	assert(strcmp(">=media-video/revised-knob-3.0-r15:12/5::wow", noclobber) == 0);
	
	t_dependency_parsing();
	
	return ret;
}

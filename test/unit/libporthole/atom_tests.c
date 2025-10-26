#include <assert.h>
#include <stdio.h>
#include <str.h>
#include <atom.h>
#include "_debug_prints.h"

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

int
main()
{
	ph_atom_t atom;
	// Comparison
	assert(ph_atom_parse_string("<something/package", &atom, 0) == 0);
	assert(atom.cmp == PH_CMP_LT);
	assert(ph_atom_parse_string("=something/package", &atom, 0) == 0);
	assert(atom.cmp == PH_CMP_EQ);
	assert(ph_atom_parse_string("some#thing/package", &atom, 0) != 0);
	assert(atom.cmp == PH_CMP_BAD);
	
	
	assert(ph_atom_parse_string("package-1.0:0/2.0", &atom, 0) == 0);
	
	// some invalid stuff
	assert(ph_atom_parse_string("something/pac?kage", &atom, 0) != 0);
	assert(ph_atom_parse_string("something/+package-1.0:3.0::gentoo", &atom, 0) != 0);
	assert(ph_atom_parse_string("a b c ", &atom, 0) != 0);

	// Slots
	assert(ph_atom_parse_string("media-video/knobgoblin-1.0:3.0::gentoo", &atom, 0) == 0);
	assert(atom.cmp == PH_CMP_UNDEF);
	_atom_debug_print(&atom);
	
	assert(ph_atom_parse_string(">=media-video/knobgoblin-1.0:hey::gentoo", &atom, 0) == 0);
	assert(atom.cmp != PH_CMP_UNDEF);
	_atom_debug_print(&atom);
	
	// clobber test
	char const *noclobber = ">=media-video/revised-knob-3.0-r15:12/5::wow";
	assert(ph_atom_parse_string(noclobber, &atom, 0) == 0);
	assert(strcmp(">=media-video/revised-knob-3.0-r15:12/5::wow", noclobber) == 0);
	_atom_debug_print(&atom);
	
	/* https://github.com/pkgcore/pkgcore/pull/420
	 * I accidentally stumbled upon this old bug somehow in pkgcore
	 * and was curious if we would pass it. */
	assert(ph_atom_parse_string("foo/bar-11-r3", &atom, 0) == 0);
	assert(strcmp(atom.pkgname, "bar-11-r3") != 0);
	_atom_debug_print(&atom);
	assert(ph_atom_parse_string(">=media-video/knob-goblin-1.0_alpha-r4::wow", &atom, 0) == 0);
	assert(ph_atom_parse_string(">=media-video/knobgoblin:4", &atom, 0) == 0);
	assert(ph_atom_parse_string("a/a-a_1a", &atom, 0) == 0);
	_atom_debug_print(&atom);
	
	// Some broken stuff
	assert(ph_atom_parse_string("/hello", &atom, 0) != 0);
	assert(ph_atom_parse_string("group/-1.0", &atom, 0) != 0);
	assert(ph_atom_parse_string(">=/", &atom, 0) != 0);
	//assert(ph_atom_parse_string("@@/hi", &atom) != 0);
	assert(ph_atom_parse_string(">=media-video/revised-knob-3.0aaaaaaaaa-r15::wow", &atom, 0) == 0);
	assert(ph_atom_parse_string(">=/", &atom, 0) != 0);
	assert(ph_atom_parse_string(">=/", &atom, 0) != 0);
	
	t_dependency_parsing();
	
	return 0;
}

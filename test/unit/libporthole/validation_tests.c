#include <assert.h>
#include <stdio.h>
#include <str.h>
#include <atom.h>
#include <use.h>

int
main()
{
	assert(ph_atom_category_is_valid("app-text") == true);
	assert(ph_atom_category_is_valid("app-text+") == true);
	assert(ph_atom_category_is_valid("-app-text") == false);
	assert(ph_atom_category_is_valid(".-app-text") == false);
	assert(ph_atom_category_is_valid(" app-text") == false);
	assert(ph_atom_category_is_valid("+app-text") == false);
	assert(ph_atom_category_is_valid("   ") == false);
	assert(ph_atom_category_is_valid("") == false);
	
	assert(ph_atom_pkgname_is_valid("systemd1337-r355onwards-whatever") == true);
	assert(ph_atom_pkgname_is_valid("systemd1337-r34isthebest") == true);
	assert(ph_atom_pkgname_is_valid("wawawawawawawawawawawawawa") == true);
	assert(ph_atom_pkgname_is_valid("systemd1337") == true);
	assert(ph_atom_pkgname_is_valid("systemd1337-1.0-r3") == false);
	assert(ph_atom_pkgname_is_valid("systemd1337-r3") == false);
	assert(ph_atom_pkgname_is_valid(".systemd13370") == false);
	assert(ph_atom_pkgname_is_valid("+systemd13370") == false);
	assert(ph_atom_pkgname_is_valid("sy+stemd13370") == true);
	assert(ph_atom_pkgname_is_valid("systemd 1337-r3") == false);
	assert(ph_atom_pkgname_is_valid("systemd1337-r32101234") == false);
	assert(ph_atom_pkgname_is_valid("systemd1337-1") == false);
	assert(ph_atom_pkgname_is_valid("systemd1337-9999.0") == false);
	assert(ph_atom_pkgname_is_valid("   ") == false);
	assert(ph_atom_pkgname_is_valid("") == false);
	
	assert(ph_use_is_valid("test") == true);
	assert(ph_use_is_valid("test-wow") == true);
	assert(ph_use_is_valid("test0") == true);
	assert(ph_use_is_valid("0test") == true);
	assert(ph_use_is_valid("-test") == false);
	return 0;
}

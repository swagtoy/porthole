#ifndef _DEBUG_PRINTS_H
#define _DEBUG_PRINTS_H

#include <atom.h>
#include <stdlib.h>
#include <stdio.h>

#define _atom_debug_print_and_free(atom) _atom_debug_print(atom);ph_atom_free(atom);

void
_atom_debug_print(ph_atom_t *atom)
{
#define TT "     "
#define TU "  "
	char *atomstr = ph_atom_to_string(atom);
	printf("Atom \"%s\":\n", atomstr);
	printf(TT "  Category:" TU "%s\n", atom->category);
	printf(TT "      Name:" TU "%s\n", atom->pkgname);
	printf(TT "   Version:" TU "%s%c\n", atom->version.version, atom->version.version_suffix);
	printf(TT "    Suffix:" TU "%s\n", "suffix");
	printf(TT "  Revision:" TU "r%lu\n", atom->version.rev);
	printf(TT "      Slot:" TU "%s\n", atom->slot);
	printf(TT "Repository:" TU "%s\n", atom->repository);
	free(atomstr);
#undef TT
#undef TU
}

#endif

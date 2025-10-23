#include <stdio.h>
#include <string.h>
#include "atom.h"

int
main(int argc, char **argv)
{
	char buf[BUFSIZ];
	char *res;
	if (argc > 1)
		res = argv[1];
	else {
		fgets(buf, sizeof(buf), stdin);
		if (buf[strlen(buf)-1] != '\n')
			fputs("Bad stdin data!\n", stderr);
		buf[strlen(buf)-1] = '\0';
		res = buf;
	}
	
	ph_atom_t atom;
	int ret = ph_atom_parse_string(res, &atom, PH_ATOM_PARSE_DEPENDENCY);
	
	return ret == 0 ? 0 : 1;
}

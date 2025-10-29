#include <assert.h>
#include <stdio.h>
#include <repo.h>
#include "_debug_prints.h"
#include "atom.h"
#include <sys/types.h>
#include <dirent.h>

#define ROOT "/var/db/repos/gentoo/metadata/md5-cache"

int
main()
{
	char buf[512] = {0};
	char path[2048] = { 0 };
	DIR *root = opendir(ROOT);
	DIR *cat, *pkg;
	
	struct dirent *ent, *ent2;
	while ((ent = readdir(root)))
	{
		snprintf(path, 2048-1, ROOT "/%s", ent->d_name);
		cat = opendir(path);
		while ((ent2 = readdir(cat)))
		{
			// parse ebuild filename now!
			ph_atom_t atom;
			ph_atom_parse_string(ent2->d_name, &atom, PH_ATOM_PARSE_STRIP_EBUILD);
		}
		closedir(cat);
	}
	closedir(root);
	
	return 0;
}

#include <assert.h>
#include <stdio.h>
#include <repo.h>
#include "_debug_prints.h"
#include "atom.h"

int
main()
{
	char buf[512] = {0};
	struct ph_repo repo = {
		.directory = "/var/db/repos/gentoo"
	};
	struct ph_repo_ctx ctx = { &repo };
	
	ph_repo_context_category_open(&ctx);
	char *cat;
	while ((cat = ph_repo_context_next(&ctx)))
	{
		printf("CAT: %s\n", cat);
		
		struct ph_repo_ctx ctx2 = { &repo };
		char *pkg;
		ph_repo_context_catpkg_open(&ctx2, cat);
		while ((pkg = ph_repo_context_next(&ctx2)))
		{
			printf("     | %s\n", pkg);
			
			struct ph_repo_ctx ctx3 = { &repo, 0 };
			char *ebuild;
			ph_repo_context_pkg_open(&ctx3, cat, pkg);
			while ((ebuild = ph_repo_context_next(&ctx3)))
			{
				snprintf(buf, 511, "%s/%s", cat, ebuild);
				// parse ebuild filename now!
				ph_atom_t atom;
				char *atomstr;
#if 1
				assert(ph_atom_parse_string(buf, &atom, PH_ATOM_PARSE_STRIP_EBUILD) == 0);
				atomstr = ph_atom_to_string(&atom);
#else
				// I have some ebuilds named stuff like -TODO.ebuild so this would break there
				if (ph_atom_parse_string(buf, &atom, PH_ATOM_PARSE_STRIP_EBUILD) != 0)
				{
					fprintf(stderr, "Couldn't parse ebuild: %s\n", ebuild);
					atomstr = ebuild;
				}
				else
					atomstr = ph_atom_to_string(&atom);
#endif

				printf("     |-- %s\n", atomstr);
				if (atomstr != ebuild) free(atomstr);
			}
			ph_repo_context_close(&ctx3);
		}
		ph_repo_context_close(&ctx2);
	}
	ph_repo_context_close(&ctx);
	
	return 0;
}

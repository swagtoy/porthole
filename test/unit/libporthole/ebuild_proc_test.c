#include <assert.h>
#include "dbcommon.h"
#include "ebuild.h"

int
main()
{
	struct ph_ebuild_proc proc = { 0 };
	struct ph_common_ecache data;
	assert(ph_ebuild_proc_spawn(&proc, "/var/db/repos/gentoo") == true);
	
	ph_ebuild_proc_push_ebuild(&proc, "games-arcade", "supertux", "supertux-0.6.3-r2");
	ph_ebuild_proc_push_ebuild(&proc, "app-editors", "nano", "nano-8.6");
	
	
	ph_ebuild_proc_read_ecache_vars(&proc, &data);
	ph_common_ecache_free(&data, 1);
	ph_ebuild_proc_read_ecache_vars(&proc, &data);
	ph_common_ecache_free(&data, 1);
	ph_ebuild_proc_stop(&proc);
	ph_ebuild_proc_wait(&proc);
	return 0;
}

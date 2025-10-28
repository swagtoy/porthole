// Licensed under the BSD 3-Clause License
	
#ifndef PORTHOLE_EBUILD_H
#define PORTHOLE_EBUILD_H

#include <stdbool.h>
#include "common.h"
#include "dbcommon.h"

/* Porthole relies on a bash script to retrieve all the ebuild
 * variables from, however, it only spawns one process for this. It
 * communicates all data over STDIN and listens for STDOUT over a pipe.
 */
 
struct _ph_ebuild_proc_impl;

struct ph_ebuild_proc
{
	struct _ph_ebuild_proc_impl *_impl;
};

_LIBPH_PUBLIC bool ph_ebuild_proc_spawn(struct ph_ebuild_proc *proc, char const *repo);
_LIBPH_PUBLIC bool ph_ebuild_proc_push_ebuild(struct ph_ebuild_proc *proc, char *cat, char *pkg, char *ebuild);
_LIBPH_PUBLIC bool ph_ebuild_proc_read_ecache_vars(struct ph_ebuild_proc *proc, struct ph_common_ecache *data);
_LIBPH_PUBLIC void ph_ebuild_proc_wait(struct ph_ebuild_proc *proc);
_LIBPH_PUBLIC bool ph_ebuild_proc_stop(struct ph_ebuild_proc *proc);

#endif

#include <assert.h>
#include <spawn.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "ebuild.h"
#include "ebuild_spawn_proc.sh.h"
#include "str.h"
#ifdef EXTRA_DEBUG
#	include <errno.h>
#endif

struct _ph_ebuild_proc_impl
{
	// store
	char *store;
	char *work;
	// process
	pid_t proc;
	posix_spawn_file_actions_t actions;
	int pipein[2];
	int pipeout[2];
};

char **environ;

bool
ph_ebuild_proc_spawn(struct ph_ebuild_proc *proc, char const *repo)
{
	if (!proc->_impl)
		proc->_impl = calloc(1, sizeof(struct _ph_ebuild_proc_impl));
	struct _ph_ebuild_proc_impl *impl = proc->_impl;
	char *argv[] = { "bash", "-c", (char *)___libporthole_data_ebuild_spawn_proc_sh, (char *)repo, NULL };
	
	if (pipe(impl->pipein) || pipe(impl->pipeout))
		DEBUGF("Couldn't pipe process! %s", strerror(errno));
	
	posix_spawn_file_actions_init(&impl->actions);
	
	posix_spawn_file_actions_adddup2(&impl->actions, impl->pipein[0], STDIN_FILENO);
	posix_spawn_file_actions_adddup2(&impl->actions, impl->pipeout[1], STDOUT_FILENO);
	//posix_spawn_file_actions_addclose(&impl->actions, impl->pipein[0]);
	//posix_spawn_file_actions_addclose(&impl->actions, impl->pipeout[1]);
	
	if (posix_spawnp(&impl->proc, "bash", &impl->actions, NULL, argv, environ) != 0)
	{
		DEBUGF("posix_spawnp failed: %s\n", strerror(errno));
		return false;
	}
	
	return true;
}

bool
ph_ebuild_proc_push_ebuild(struct ph_ebuild_proc *proc, char *cat, char *pkg, char *ebuild)
{
	struct _ph_ebuild_proc_impl *impl = proc->_impl;
	size_t catlen = strlen(cat),
	       pkglen = strlen(pkg),
	       ebuildlen = strlen(ebuild);
	
	//DEBUGF("cat: %s\npkg: %s\nebuild: %s.ebuild\n", cat, pkg, ebuild); 
	write(impl->pipein[1], cat, catlen);
	write(impl->pipein[1], "/", 1);
	write(impl->pipein[1], pkg, pkglen);
	write(impl->pipein[1], "/", 1);
	write(impl->pipein[1], ebuild, ebuildlen);
	write(impl->pipein[1], ".ebuild", sizeof(".ebuild")-1);
	write(impl->pipein[1], "\n", 1);
	
	return true;
}

bool
ph_ebuild_proc_read_ecache_vars(struct ph_ebuild_proc *proc, struct ph_common_ecache *data)
{
	memset(data, 0, sizeof(struct ph_common_ecache));
	data->alloc_bit = 1;
	const int MAX_LINES = 13;
	struct _ph_ebuild_proc_impl *impl = proc->_impl;
	if (!impl->store)
		impl->store = str_new();
	char buf[BUFSIZ] =  { 0 };
	int sz;
	bool breakout = false;
	while ((sz = read(impl->pipeout[0], buf, BUFSIZ-1)) > 0)
	{
		//DEBUGF("buf[BUFSIZ,%d]: %s\n", sz, buf);
		for (int i = 0; i < sz; ++i)
		{
			if (buf[i] == '\0')
			{
				DEBUG("STOP. Hit a null terminator!\n");
				breakout = true;
			}
		}
		
		buf[sz] = '\0';
		str_cappend(&impl->store, buf);
		if (breakout)
			break;
	}
	impl->work = impl->store;

	// TODO: str.h needs a step amount, what we are doing is probably
	// slow if the malloc impl is bad enough
	
	for (int line = 0; line < MAX_LINES + 1; ++line)
	{
		// TODO: use one string and separate it?
		char *tmp = NULL;
		for (; *impl->work && *impl->work != '\n'; ++impl->work)
		{
			if (tmp == NULL)
				tmp = str_new();
			str_chappend(&tmp, *impl->work);
		}
		if (*impl->work == '\n')
			++impl->work;
		
#define STR_CASE(i, x) case i: x = tmp; if (x) DEBUGF(#x ": %s\n", x); break;
		switch (line)
		{
		case 0:
			data->EAPI = 0;
			break;
		STR_CASE(1, data->DESCRIPTION);
		STR_CASE(2, data->BDEPEND);
		STR_CASE(3, data->HOMEPAGE);
		STR_CASE(4, data->IDEPEND);
		STR_CASE(5, data->INHERIT);
		STR_CASE(6, data->IUSE);
		STR_CASE(7, data->KEYWORDS);
		STR_CASE(8, data->LICENSE);
		STR_CASE(9, data->RDEPEND);
		STR_CASE(10, data->REQUIRED_USE);
		STR_CASE(11, data->RESTRICT);
		STR_CASE(12, data->SLOT);
		STR_CASE(13, data->SRC_URI);
		default:
			assert(!"Line too much!");
		}
	}
	

	return false;
}

void
ph_ebuild_proc_wait(struct ph_ebuild_proc *proc)
{
	int status;
	do {
		waitpid(proc->_impl->proc, &status, WUNTRACED);
		
		if (WIFEXITED(status))
			DEBUGF("%d exited, status=%d\n", proc->_impl->proc, WEXITSTATUS(status));
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
}

bool
ph_ebuild_proc_stop(struct ph_ebuild_proc *proc)
{
	write(proc->_impl->pipein[1], "<<<byebye!>>>\n", 15);
	posix_spawn_file_actions_destroy(&proc->_impl->actions);
	close(proc->_impl->pipein[0]);
	close(proc->_impl->pipein[1]);
	close(proc->_impl->pipeout[0]);
	close(proc->_impl->pipeout[1]);
	free(proc->_impl);
	return true;
}

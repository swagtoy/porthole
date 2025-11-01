#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include "cache_spawner.h"

static struct option long_options[] = {
	{ "update", no_argument, 0, 0 },
	{ "repository", optional_argument, 0, 0 },
	{ "jobs", optional_argument, 0, 0 },
	{ 0, 0, 0, 0 },
};

int
main(int argc, char **argv)
{
	int option_idx;
	int opt;
	bool parsed_opt;
	char *endptr;
	// opts
	long jobs = 1;
	char const *repo = "gentoo";
	bool update_database = false;
	
	while ((opt = getopt_long(argc, argv, "ur:j:", long_options, &option_idx)) != -1)
	{
		parsed_opt = true;
		switch (opt)
		{
		case 'u':
			update_database = true;
			break;
		case 'r':
			repo = optarg;
			break;
		case 'j': {
				jobs = strtol(optarg, &endptr, 10);
				if (optarg == endptr || *endptr != '\0' || errno == ERANGE)
				{
					fputs("Value must be an integer.\n", stderr);
				}
			}
			break;
		default:
			return EXIT_FAILURE;
		}
	}
	
	if (!parsed_opt)
	{
		fputs("No argument passed!\n", stderr);
		return EXIT_FAILURE;
	}
	
	if (update_database)
	{
		if (!phcache_gen_cache(repo, jobs))
		{
			fprintf(stderr, "Failed to generate cache for repository \"%s\".\n", repo);
			return EXIT_FAILURE;
		}
	}
	
	return EXIT_SUCCESS;
}

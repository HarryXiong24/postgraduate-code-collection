/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * main.c
 */

#include "avl.h"
#include "term.h"
#include "shell.h"

static int
exists(struct avl *avl, const char *s)
{
	uint64_t count;

	if (!(count = avl_exists(avl, s))) {
		printf("'%s' does not exist\n", s);
	}
	else {
		printf("'%s' x %lu exists\n", s, (unsigned long)count);
	}
	return 0;
}

static int
insert(struct avl *avl, const char *s)
{
	if (avl_insert(avl, s)) {
		printf("error: failed to insert '%s'", s);
	}
	return 0;
}

static int
load(struct avl *avl, const char *s)
{
	char word[256];
	FILE *file;

	if (!(file = fopen(s, "r"))) {
		printf("error: unable to open '%s' for reading\n", s);
		return 0;
	}
	while (fgets(word, sizeof (word), file)) {
		shell_strtrim(word);
		if (safe_strlen(word)) {
			if (avl_insert(avl, word)) {
				printf("error: unable to load '%s'", word);
				break;
			}
		}
	}
	fclose(file);
	return 0;
}

static void
list_word(void *arg, const char *word, uint64_t count)
{
	UNUSED(arg);

	printf("'%s' x %lu\n", word, (unsigned long)count);
}

static int
list(struct avl *avl, const char *s)
{
	UNUSED(s);

	avl_traverse(avl, list_word, NULL);
	return 0;
}

static int
info(struct avl *avl, const char *s)
{
	UNUSED(s);

	printf("\n-- info -- \n"
	       "  words    : %lu (total)\n"
	       "  words    : %lu (unique)\n"
	       "  utilized : %lu bytes\n"
	       "  capacity : %lu bytes\n"
	       "\n",
	       (unsigned long)avl_items(avl),
	       (unsigned long)avl_unique(avl),
	       (unsigned long)avl_scm_utilized(avl),
	       (unsigned long)avl_scm_capacity(avl));
	return 0;
}

static int
help(struct avl *avl, const char *s)
{
	UNUSED(avl);
	UNUSED(s);

	printf("\n-- commands -- \n"
	       "  quit          : exit the program\n"
	       "  help          : prints this menu\n"
	       "  info          : report info\n"
	       "  list          : list words in sorted order\n"
	       "  load pathname : load words from file @ 'pathname'\n"
	       "  insert word   : insert 'word'\n"
	       "  exists word   : check if 'word' exists\n"
	       "\n");
	return 0;
}

static int
quit(struct avl *avl, const char *s)
{
	UNUSED(avl);
	UNUSED(s);

	return 1;
}

static int
shell_fnc(void *arg, const char *s)
{
	const struct {
		int argc;
		const char *face;
		int (*fnc)(struct avl *avl, const char *s);
	} CMDS[] = {
		{ 0, "quit", quit },
		{ 0, "help", help },
		{ 0, "info", info },
		{ 0, "list", list },
		{ 1, "load", load },
		{ 1, "insert", insert },
		{ 1, "exists", exists }
	};
	struct avl *avl;
	uint64_t i;

	avl = (struct avl *)arg;
	for (i=0; i<ARRAY_SIZE(CMDS); ++i) {
		if (!strncmp(CMDS[i].face, s, safe_strlen(CMDS[i].face))) {
			s += safe_strlen(CMDS[i].face);
			shell_strtrim((char *)s);
			if ((CMDS[i].argc && !safe_strlen(s)) ||
			    (!CMDS[i].argc && safe_strlen(s))) {
				break;
			}
			return CMDS[i].fnc(avl, s);
		}
	}
	printf("error: bad command/argument (%s)\n",
	       safe_strlen(s)
	       ? s
	       : "missing argument");
	return 0;
}

static void
greetings(void)
{
	term_bold();
	term_color(TERM_COLOR_MAGENTA);
	printf("\n"
	       "              ___  _____ ____      \n"
	       "    _________|__ \\|__  /( __ )____ \n"
	       "   / ___/ ___/_/ / /_ </ __  / __ \\\n"
	       "  / /__(__  ) __/___/ / /_/ / /_/ /\n"
	       "  \\___/____/____/____/\\____/ .___/ \n"
	       "                          /_/      \n");
	term_reset();
	term_color(TERM_COLOR_GREEN);
	printf("   Storage Class Memory Manager\n\n");
	term_reset();
}

static void
usage(const char *name)
{
	printf("usage: %s [options] pathname\n\n"
	       "  -- options --\n"
	       "    truncate : clear SCM content\n"
	       "    nocolor  : do not use terminal colors\n"
	       "\n",
	       name);
}

int
main(int argc, char *argv[])
{
	char *pathname = NULL;
	int truncate = 0;
	int nocolor = 0;
	struct avl *avl;
	int i;

	for (i=1; i<argc; ++i) {
		if (!strcmp(argv[i], "--truncate") && !truncate) {
			truncate = 1;
		}
		else if (!strcmp(argv[i], "--nocolor") && !nocolor) {
			nocolor = 1;
		}
		else if (!strcmp(argv[i], "--help")) {
			usage(argv[0]);
			return 0;
		}
		else if (('-' != argv[i][0]) && !pathname) {
			pathname = argv[i];
		}
		else {
			printf("invalid command line argument %s\n", argv[i]);
			return -1;
		}
	}
	if (!safe_strlen(pathname)) {
		usage(argv[0]);
		return -1;
	}
	if (!(avl = avl_open(pathname, truncate))) {
		TRACE(0);
		return -1;
	}
	term_init(nocolor);
	greetings();
	shell(shell_fnc, avl);
	avl_close(avl);
	return 0;
}

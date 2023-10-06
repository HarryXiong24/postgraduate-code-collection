/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * main.c
 */

#include "jitc.h"
#include "parser.h"
#include "system.h"

/* export LD_LIBRARY_PATH=. */
static void 
reflect(const struct parser_dag *dag, FILE *file) {


}

static void
generate(const struct parser_dag *dag, FILE *file)
{
	/* YOUR CODE HERE */
  fprintf(file, "double evaluate(void) {\n");
  reflect(dag, file);
  fprintf(file, "return t%d;\n");
  fprintf(file, "\n}");
}

typedef double (*evaluate_t)(void);

int
main(int argc, char *argv[])
{
	const char *SOFILE = "out.so";
	const char *CFILE = "out.c";
	struct parser *parser;
	struct jitc *jitc;
	evaluate_t fnc;
	FILE *file;

	/* usage */

	if (2 != argc) {
		printf("usage: %s expression\n", argv[0]);
		return -1;
	}

	/* parse */

	if (!(parser = parser_open(argv[1]))) {
		TRACE(0);
		return -1;
	}

	/* generate C */

	if (!(file = fopen(CFILE, "w"))) {
		TRACE("fopen()");
		return -1;
	}
	generate(parser_dag(parser), file);
	parser_close(parser);
	fclose(file);

	/* JIT compile */

	if (jitc_compile(CFILE, SOFILE)) {
		file_delete(CFILE);
		TRACE(0);
		return -1;
	}
	file_delete(CFILE);

	/* dynamic load */

	if (!(jitc = jitc_open(SOFILE)) ||
	    !(fnc = (evaluate_t)jitc_lookup(jitc, "evaluate"))) {
		file_delete(SOFILE);
		jitc_close(jitc);
		TRACE(0);
		return -1;
	}
	printf("%f\n", fnc());

	/* done */

	file_delete(SOFILE);
	jitc_close(jitc);
	return 0;
}
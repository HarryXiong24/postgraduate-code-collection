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
#include "math.h"

/* export LD_LIBRARY_PATH=. */

static void
reflect(const struct parser_dag *dag, FILE *file)
{
	if (dag)
	{
		reflect(dag->left, file);
		reflect(dag->right, file);
		if (PARSER_DAG_VAL == dag->op)
		{
			fprintf(file,
							"double t%d = %f;\n",
							dag->id,
							dag->val);
		}
		else if (PARSER_DAG_NEG == dag->op)
		{
			fprintf(file,
							"double t%d = - t%d;\n",
							dag->id,
							dag->right->id);
		}
		else if (PARSER_DAG_MUL == dag->op)
		{
			fprintf(file,
							"double t%d = t%d * t%d;\n",
							dag->id,
							dag->left->id,
							dag->right->id);
		}
		else if (PARSER_DAG_DIV == dag->op)
		{
			fprintf(file,
							"double t%d = t%d ? (t%d / t%d) : 0.0;\n",
							dag->id,
							dag->right->id,
							dag->left->id,
							dag->right->id);
		}
		else if (PARSER_DAG_ADD == dag->op)
		{
			fprintf(file,
							"double t%d = t%d + t%d;\n",
							dag->id,
							dag->left->id,
							dag->right->id);
		}
		else if (PARSER_DAG_SUB == dag->op)
		{
			fprintf(file,
							"double t%d = t%d - t%d;\n",
							dag->id,
							dag->left->id,
							dag->right->id);
		}
		else
		{
			EXIT("software");
		}
	}
}

double sigmoid(double x)
{
	/* reference https://en.wikipedia.org/wiki/Sigmoid_function */
	return 1.0 / (1.0 + exp(-x));
}

static void
generate(const struct parser_dag *dag, FILE *file)
{
	fprintf(file, "extern double sigmoid(double x);\n");
	fprintf(file, "double evaluate(void) {\n");
	reflect(dag, file);
	/* fprintf(file, "return t%d;\n}\n", dag->id); */
	fprintf(file, "return sigmoid(t%d);\n}\n", dag->id);
}

typedef double (*evaluate_t)(void);

int main(int argc, char *argv[])
{
	const char *SOFILE = "out.so";
	const char *CFILE = "out.c";
	struct parser *parser;
	struct jitc *jitc;
	evaluate_t fnc;
	FILE *file = NULL;

	/* usage */

	if (2 != argc)
	{
		printf("usage: %s expression\n", argv[0]);
		return -1;
	}

	/* parse */

	if (!(parser = parser_open(argv[1])))
	{
		TRACE(0);
		return -1;
	}

	/* generate C */

	if (!(file = fopen(CFILE, "w")))
	{
		TRACE("fopen()");
		return -1;
	}
	/* generate evaluate function */
	generate(parser_dag(parser), file);
	parser_close(parser);
	fclose(file);

	/* JIT compile */

	if (jitc_compile(CFILE, SOFILE))
	{
		file_delete(CFILE);
		TRACE(0);
		return -1;
	}
	file_delete(CFILE);

	/* dynamic load */

	if (!(jitc = jitc_open(SOFILE)) ||
			!(fnc = (evaluate_t)jitc_lookup(jitc, "evaluate"))) /* find evaluate function */
	{
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

/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * parser.c
 */

#include "lexer.h"
#include "parser.h"

#define MKD(p,d,o)					\
	do {						\
		size_t n = sizeof (struct parser_dag);	\
		if (!((d) = malloc(n))) {		\
			TRACE("out of memory");		\
			return NULL;			\
		}					\
		memset((d), 0, n);			\
		(d)->op = (o);				\
		(d)->id = ++(p)->id;			\
	}						\
	while (0)

#define TRACE_ONCE(p,m)				\
	do {					\
		if (!(p)->stop) {		\
			(p)->stop = 1;		\
			TRACE(m);		\
		}				\
	} while (0)

struct parser {
	int id; /* global id */
	int stop;
	uint64_t i; /* current token */
	uint64_t n; /* total tokens */
	struct lexer *lexer;
	struct parser_dag *dag;
};

static void
free_dag(struct parser_dag *dag)
{
	if (dag) {
		free_dag(dag->left);
		free_dag(dag->right);
	}
	FREE(dag);
}

static const struct lexer_token *
next(const struct parser *parser)
{
	static const struct lexer_token SENTINEL = { LEXER_OP_, 0.0 };

	if (parser->i < parser->n) {
		return lexer_lookup(parser->lexer, parser->i);
	}
	return &SENTINEL;
}

static int /* BOOL */
match(const struct parser *parser, enum lexer_token_op op)
{
	const struct lexer_token *token;

	if ((token = next(parser)) && (op == token->op)) {
		return 1;
	}
	return 0;
}

static void
forward(struct parser *parser)
{
	if (parser->i < parser->n) {
		++parser->i;
	}
}

/**
 * expr_primary : VAL
 *              | '(' expr ')'
 */

static struct parser_dag *expr(struct parser *parser);

static struct parser_dag *
expr_primary(struct parser *parser)
{
	struct parser_dag *dag;

	dag = NULL;
	if (match(parser, LEXER_OP_VAL)) {
		MKD(parser, dag, PARSER_DAG_VAL);
		dag->val = next(parser)->val;
		forward(parser);
	}
	else if (match(parser, LEXER_OP_OPEN)) {
		forward(parser);
		if (!(dag = expr(parser))) {
			TRACE_ONCE(parser, "invalid sub-expression");
			return NULL;
		}
		if (!match(parser, LEXER_OP_CLOSE)) {
			TRACE_ONCE(parser, "expecting ')'");
			return NULL;
		}
		forward(parser);
	}
	return dag;
}

/**
 * expr_unary : [ '+' '-' ] exr_unary
 *            | expr_primary
 */

static struct parser_dag *
expr_unary(struct parser *parser)
{
	struct parser_dag *dag;

	dag = NULL;
	if (match(parser, LEXER_OP_ADD)) {
		forward(parser);
		if (!(dag = expr_unary(parser))) {
			TRACE_ONCE(parser, "invalid unary '+' operand");
			return NULL;
		}
	}
	else if (match(parser, LEXER_OP_SUB)) {
		MKD(parser, dag, PARSER_DAG_NEG);
		forward(parser);
		if (!(dag->right = expr_unary(parser))) {
			TRACE_ONCE(parser, "invalid unary '-' operand");
			return NULL;
		}
	}
	else {
		dag = expr_primary(parser);
	}
	return dag;
}

/**
 * expr_multiplicative_ : { [ '*' '/' ] expr_unary expr_multiplicative_ }
 *                      | ∅
 *
 * expr_multiplicative : expr_unary expr_multiplicative_
 */

static struct parser_dag *
expr_multiplicative_(struct parser *parser, struct parser_dag *left)
{
	const char * const TBL[] = { "*", "/" };
	struct parser_dag *dag;
	char buf[64];

	dag = left;
	for (;;) {
		if (match(parser, LEXER_OP_MUL)) {
			MKD(parser, dag, PARSER_DAG_MUL);
			dag->left = left;
			forward(parser);
		}
		else if (match(parser, LEXER_OP_DIV)) {
			MKD(parser, dag, PARSER_DAG_DIV);
			dag->left = left;
			forward(parser);
		}
		else {
			break;
		}
		if (!(dag->right = expr_unary(parser))) {
			safe_sprintf(buf,
				     sizeof (buf),
				     "invalid '%s' operand",
				     TBL[dag->op - PARSER_DAG_MUL]);
			TRACE_ONCE(parser, buf);
			return NULL;
		}
		if (!(dag = expr_multiplicative_(parser, dag))) {
			TRACE_ONCE(parser, 0);
			return NULL;
		}
	}
	return dag;
}

static struct parser_dag *
expr_multiplicative(struct parser *parser)
{
	struct parser_dag *dag;

	if (!(dag = expr_unary(parser))) {
		return NULL;
	}
	return expr_multiplicative_(parser, dag);
}

/**
 * expr_additive_ : { [ '+' '-' ] expr_multiplicative expr_additive_ }
 *                | ∅
 *
 * expr_additive : expr_multiplicative expr_additive_
 */

static struct parser_dag *
expr_additive_(struct parser *parser, struct parser_dag *left)
{
	const char * const TBL[] = { "+", "-" };
	struct parser_dag *dag;
	char buf[64];

	dag = left;
	for (;;) {
		if (match(parser, LEXER_OP_ADD)) {
			MKD(parser, dag, PARSER_DAG_ADD);
			dag->left = left;
			forward(parser);
		}
		else if (match(parser, LEXER_OP_SUB)) {
			MKD(parser, dag, PARSER_DAG_SUB);
			dag->left = left;
			forward(parser);
		}
		else {
			break;
		}
		if (!(dag->right = expr_multiplicative(parser))) {
			safe_sprintf(buf,
				     sizeof (buf),
				     "invalid '%s' operand",
				     TBL[dag->op - PARSER_DAG_MUL]);
			TRACE_ONCE(parser, buf);
			return NULL;
		}
		if (!(dag = expr_additive_(parser, dag))) {
			TRACE_ONCE(parser, 0);
			return NULL;
		}
	}
	return dag;
}

static struct parser_dag *
expr_additive(struct parser *parser)
{
	struct parser_dag *dag;

	if (!(dag = expr_multiplicative(parser))) {
		return NULL;
	}
	return expr_additive_(parser, dag);
}

/**
 * expr : expr_additive
 */

static struct parser_dag *
expr(struct parser *parser)
{
	return expr_additive(parser);
}

/**
 * top : expr
 */

static struct parser_dag *
top(struct parser *parser)
{
	struct parser_dag *dag;

	if (!(dag = expr(parser))) {
		TRACE_ONCE(parser, "invalid expression");
		return NULL;
	}
	if (!match(parser, LEXER_OP_)) {
		TRACE_ONCE(parser, "bogus trailing content");
		return NULL;
	}
	return dag;
}

struct parser *
parser_open(const char *s)
{
	struct parser *parser;

	assert( safe_strlen(s) );

	if (!(parser = malloc(sizeof (struct parser)))) {
		TRACE("out of memory");
		return NULL;
	}
	memset(parser, 0, sizeof (struct parser));
	if (!(parser->lexer = lexer_open(s)) ||
	    !(parser->n = lexer_size(parser->lexer)) ||
	    !(parser->dag = top(parser))) {
		parser_close(parser);
		TRACE(0);
		return NULL;
	}
	lexer_close(parser->lexer);
	parser->lexer = NULL;
	return parser;
}

void
parser_close(struct parser *parser)
{
	if (parser) {
		free_dag(parser->dag);
		lexer_close(parser->lexer);
		memset(parser, 0, sizeof (struct parser));
	}
	FREE(parser);
}

const struct parser_dag *
parser_dag(const struct parser *parser)
{
	assert( parser );

	return parser->dag;
}

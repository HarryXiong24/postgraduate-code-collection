/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * parser.h
 */

#ifndef _PARSER_H_
#define _PARSER_H_

struct parser_dag {
	enum parser_dag_op {
		PARSER_DAG_,
		PARSER_DAG_VAL, /* val */
		PARSER_DAG_NEG, /* - right */
		PARSER_DAG_MUL, /* left * right */
		PARSER_DAG_DIV, /* left / right */
		PARSER_DAG_ADD, /* left + right */
		PARSER_DAG_SUB  /* left - right */
	} op;
	int id; /* guaranteed to be unique */
	double val;
	struct parser_dag *left;
	struct parser_dag *right;
};

struct parser;

struct parser *parser_open(const char *s);

void parser_close(struct parser *parser);

const struct parser_dag *parser_dag(const struct parser *parser);

#endif /* _PARSER_H_ */

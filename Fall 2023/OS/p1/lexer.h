/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * lexer.h
 */

#ifndef _LEXER_H_
#define _LEXER_H_

#include "system.h"

struct lexer_token {
	enum lexer_token_op {
		LEXER_OP_,
		LEXER_OP_VAL,
		LEXER_OP_ADD,  /* '+' */
		LEXER_OP_SUB,  /* '-' */
		LEXER_OP_MUL,  /* '*' */
		LEXER_OP_DIV,  /* '/' */
		LEXER_OP_OPEN, /* '(' */
		LEXER_OP_CLOSE /* ')' */
	} op;
	double val;
};

struct lexer;

struct lexer *lexer_open(const char *s);

void lexer_close(struct lexer *lexer);

uint64_t lexer_size(const struct lexer *lexer);

const struct lexer_token *lexer_lookup(const struct lexer *lexer, uint64_t i);

#endif /* _LEXER_H_ */

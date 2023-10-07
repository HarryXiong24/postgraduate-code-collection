/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * term.c
 */

#include "system.h"
#include "term.h"

static int nocolor;

void
term_init(int nocolor_)
{
	nocolor = nocolor_ ? 1 : 0;
}

void
term_color(enum term_color color)
{
	assert( (0 <= color) && (7 >= color) );

	if (!nocolor) {
		printf("\033[%dm", 30 + color);
		fflush(stdout);
	}
}

void
term_bold(void)
{
	if (!nocolor) {
		printf("\033[1m");
		fflush(stdout);
	}
}

void
term_reset(void)
{
	if (!nocolor) {
		printf("\033[?25h");
		printf("\033[0m");
		fflush(stdout);
	}
}

/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * term.h
 */

#ifndef _TERM_H_
#define _TERM_H_

enum term_color {
	TERM_COLOR_BLACK,
	TERM_COLOR_RED,
	TERM_COLOR_GREEN,
	TERM_COLOR_YELLOW,
	TERM_COLOR_BLUE,
	TERM_COLOR_MAGENTA,
	TERM_COLOR_CYAN,
	TERM_COLOR_GRAY
};

void term_init(int nocolor);

void term_color(enum term_color color);

void term_bold(void);

void term_reset(void);

#endif /* _TERM_H_ */

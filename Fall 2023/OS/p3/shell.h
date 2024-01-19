/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * shell.h
 */

#ifndef _SHELL_H_
#define _SHELL_H_

typedef int (*shell_fnc_t)(void *arg, const char *s);

void shell(shell_fnc_t fnc, void *arg);

void shell_strtrim(char *s);

#endif /* _SHELL_H_ */

/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * shell.c
 */

#include <termios.h>
#include <unistd.h>
#include "system.h"
#include "term.h"
#include "shell.h"

/**
 * Needs
 *   tcgetattr()
 *   tcsetattr()
 */

#define H  96 /* history slots  */
#define N 160 /* bytes per slot */

#define PROMPT "CS238P -> "

#define CLEAR()  do { printf("\033[0J"); /*---------*/ } while (0)
#define POS(r,c) do { printf("\033[%d;%dH", (r), (c)); } while (0)

static struct {
	int i, n;
	char history[H][N];
	struct termios termios;
} state;

static void
configure(void)
{
	struct termios termios;

	if (tcgetattr(STDIN_FILENO, &state.termios)) {
		EXIT("tcgetattr()");
	}
	termios = state.termios;
	termios.c_lflag &= ~(ECHO | ICANON);
	termios.c_cc[VMIN] = 1;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &termios)) {
		EXIT("tcgetattr()");
	}
}

static void
restore(void)
{
	if (tcsetattr(STDIN_FILENO, TCSANOW, &state.termios)) {
		EXIT("tcgetattr()");
	}
}

static void
location(int *r, int *c)
{
	printf("\033[6n");
	for (;;) {
		while (27 != getchar()) {
			/* ignore */
		}
		if (2 == scanf("[%d;%dR", r, c)) {
			break;
		}
	}
}

static const char *
read_line(void)
{
	int i, j, n, r, c, k;
	char *buf;

	buf = state.history[j = state.i = state.n];
	i = n = (int)safe_strlen(buf);
	location(&r, &c);
	for (;;) {
		POS(r, 1);
		CLEAR();
		term_color(TERM_COLOR_BLUE);
		term_bold();
		printf(PROMPT);
		term_reset();
		printf("%s", buf);
		POS(r, i + 1 + (int)safe_strlen(PROMPT));
		k = getchar();
		if (27 == k) {
			getchar();
			switch (getchar()) {
			case 'A':
				if (0 < j) {
					buf[0] = '\0';
					if (buf != state.history[--j]) {
						strncpy(buf,
							state.history[j],
							N);
					}
					i = n = (int)safe_strlen(buf);
				}
				break;
			case 'B':
				if (state.n > j) {
					buf[0] = '\0';
					if (buf != state.history[++j]) {
						strncpy(buf,
							state.history[j],
							N);
					}
					i = n = (int)safe_strlen(buf);
				}
				break;
			case 'C':
				i = (i < n) ? (i + 1) : n;
				break;
			case 'D':
				i = (i > 0) ? (i - 1) : 0;
				break;
			default:
				break; /* ignored */
			}
			continue;
		}
		else if (11 == k) {
			buf[i] = '\0';
			i = n = (int)safe_strlen(buf);
			continue;
		}
		else if (12 == k) {
			CLEAR();
			POS(r = 1, c = 1);
			continue;
		}
		else if (4 == k) {
			if (n > i) {
				memmove(buf + i + 0,
					buf + i + 1,
					safe_strlen(buf + i + 1) + 1);
				--n;
			}
		}
		else if ((8 == k) || (127 == k)) {
			if (0 < i) {
				memmove(buf + i - 1,
					buf + i + 0,
					safe_strlen(buf + i) + 1);
				--i;
				--n;
			}
			continue;
		}
		else if ((' ' == k) || ('\t' == k)) {
			if (N > n) {
				memmove(buf + i + 1,
					buf + i + 0,
					safe_strlen(buf + i) + 1);
				buf[i] = ' ';
				++i;
				++n;
			}
			continue;
		}
		else if ('\n' == k) {
			break;
		}
		else if (isprint(k)) {
			if ((n + 1) < N) {
				if (i < n) {
					memmove(buf + i + 1,
						buf + i + 0,
						safe_strlen(buf + i));
				}
				buf[i++] = (char)k;
				buf[++n] = '\0';
			}
		}
		else {
			/* ignored */
		}
	}
	printf("\n");
	shell_strtrim(buf);
	if (safe_strlen(buf)) {
		if (state.i == state.n) {
			++state.n;
			state.n = (H > state.n) ? state.n : H;
			state.history[state.n][0] = '\0';
		}
		return buf;
	}
	return NULL;
}

void
shell(shell_fnc_t fnc, void *arg)
{
	const char *s;

	assert( fnc );

	configure();
	for (;;) {
		if ((s = read_line())) {
			if (fnc(arg, s)) {
				break;
			}
			CLEAR();
		}
	}
	restore();
}

void
shell_strtrim(char *s)
{
	const char *b, *e;
	uint64_t n;

	b = s;
	e = s + safe_strlen(s) - 1;
	while ((b <= e) && isspace((unsigned char)(*b))) {
		++b;
	}
	while ((e >= s) && isspace((unsigned char)(*e))) {
		--e;
	}
	n = (b <= e) ? (e - b + 1) : 0;
	memmove(s, b, n);
	s[n] = '\0';
}

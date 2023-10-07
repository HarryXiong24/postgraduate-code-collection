/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * avl.h
 */

#ifndef _AVL_H_
#define _AVL_H_

#include "system.h"

struct avl;

typedef void (*avl_fnc_t)(void *arg, const char *item, uint64_t count);

struct avl *avl_open(const char *pathname, int truncate);

void avl_close(struct avl *avl);

int avl_insert(struct avl *avl, const char *item);

uint64_t avl_exists(const struct avl *avl, const char *item);

void avl_traverse(const struct avl *avl, avl_fnc_t fnc, void *arg);

uint64_t avl_items(const struct avl *avl);

uint64_t avl_unique(const struct avl *avl);

size_t avl_scm_utilized(const struct avl *avl);

size_t avl_scm_capacity(const struct avl *avl);

#endif /* _AVL_H_ */

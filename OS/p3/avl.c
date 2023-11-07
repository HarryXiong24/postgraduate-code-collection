/**
 * Tony Givargis
 * Copyright (C), 2023
 * University of California, Irvine
 *
 * CS 238P - Operating Systems
 * avl.c
 */

#include "scm.h"
#include "avl.h"

struct avl
{
	struct state
	{
		uint64_t items;
		uint64_t unique;
		struct node
		{
			int depth;
			uint64_t count;
			const char *item;
			struct node *left;
			struct node *right;
		} *root;
	} *state; /* SCM */
	struct scm *scm;
};

static int
delta(const struct node *node)
{
	return node ? node->depth : -1;
}

static int
balance(const struct node *node)
{
	return delta(node->left) - delta(node->right);
}

static int
depth(const struct node *a, const struct node *b)
{
	return (delta(a) > delta(b)) ? (delta(a) + 1) : (delta(b) + 1);
}

static struct node *
rotate_right(struct node *node)
{
	struct node *root;

	root = node->left;
	node->left = root->right;
	root->right = node;
	node->depth = depth(node->left, node->right);
	root->depth = depth(root->left, node);
	return root;
}

static struct node *
rotate_left(struct node *node)
{
	struct node *root;

	root = node->right;
	node->right = root->left;
	root->left = node;
	node->depth = depth(node->left, node->right);
	root->depth = depth(root->right, node);
	return root;
}

static struct node *
rotate_left_right(struct node *node)
{
	node->left = rotate_left(node->left);
	return rotate_right(node);
}

static struct node *
rotate_right_left(struct node *node)
{
	node->right = rotate_right(node->right);
	return rotate_left(node);
}

static struct node *
update(struct avl *avl, struct node *root, const char *item)
{
	int d;

	if (!root)
	{
		if (!(root = scm_malloc(avl->scm, sizeof(struct node))))
		{
			TRACE(0);
			return NULL;
		}
		memset(root, 0, sizeof(struct node));
		printf("102 scm_malloc end\n");
		if (!(root->item = scm_strdup(avl->scm, item)))
		{
			TRACE(0);
			return NULL;
		}
		printf("108 scm_strdup end\n");
		++root->count;
		++avl->state->items;
		++avl->state->unique;
		return root;
	}
	if (!(d = strcmp(item, root->item)))
	{
		++root->count;
		++avl->state->items;
	}
	else if (0 > d)
	{
		root->left = update(avl, root->left, item);
		if (1 < abs(balance(root)))
		{
			if (0 > strcmp(item, root->left->item))
			{
				root = rotate_right(root);
			}
			else
			{
				root = rotate_left_right(root);
			}
		}
	}
	else if (0 < d)
	{
		root->right = update(avl, root->right, item);
		if (1 < abs(balance(root)))
		{
			if (0 < strcmp(item, root->right->item))
			{
				root = rotate_left(root);
			}
			else
			{
				root = rotate_right_left(root);
			}
		}
	}
	root->depth = depth(root->left, root->right);
	return root;
}

static void
traverse(struct node *node, avl_fnc_t fnc, void *arg)
{
	if (node)
	{
		traverse(node->left, fnc, arg);
		fnc(arg, node->item, node->count);
		traverse(node->right, fnc, arg);
	}
}

struct avl *
avl_open(const char *pathname, int truncate)
{
	struct avl *avl;

	assert(pathname);

	if (!(avl = malloc(sizeof(struct avl))))
	{
		TRACE("out of memory");
		return NULL;
	}
	memset(avl, 0, sizeof(struct avl));
	if (!(avl->scm = scm_open(pathname, truncate)))
	{
		avl_close(avl);
		TRACE(0);
		return NULL;
	}
	if (scm_utilized(avl->scm))
	{
		avl->state = scm_mbase(avl->scm);
	}
	else
	{
		if (!(avl->state = scm_malloc(avl->scm,
																	sizeof(struct state))))
		{
			avl_close(avl);
			TRACE(0);
			return NULL;
		}
		printf("196 scm_malloc end\n");
		memset(avl->state, 0, sizeof(struct state));
		printf("198 scm_mbase start\n");
		assert(avl->state == scm_mbase(avl->scm));
		printf("200 scm_mbase end\n");
	}
	return avl;
}

static struct node *
minValueNode(struct node *node)
{
	struct node *current = node;

	/* Traverse the tree to the leftmost leaf */
	while (current && current->left != NULL)
		current = current->left;

	return current;
}

static struct node *
remove_node(struct avl *avl, struct node *root, const char *item)
{
	int bal;
	int d;

	if (root == NULL)
	{
		/* The item was not found. */
		return root;
	}

	d = strcmp(item, root->item);

	/* If the item to be deleted is smaller than the root's item, then it lies in left subtree */
	if (d < 0)
	{
		root->left = remove_node(avl, root->left, item);
	}
	/* If the item to be deleted is greater than the root's item, then it lies in right subtree */
	else if (d > 0)
	{
		root->right = remove_node(avl, root->right, item);
	}
	/* If item is same as root's item, then this is the node to be deleted */
	else
	{
		/* Node with only one child or no child */
		if ((root->left == NULL) || (root->right == NULL))
		{
			struct node *temp = root->left ? root->left : root->right;

			if (temp == NULL)
			{
				temp = root;
				root = NULL;
			}
			else
			{
				/* Copy the contents of the non-empty child */
				*root = *temp;
			}
			scm_free(avl->scm, temp);
		}
		else
		{
			/* Node with two children: Get the inorder successor (smallest in the right subtree) */
			struct node *temp = minValueNode(root->right);

			/* Copy the inorder successor's data to this node */
			root->item = temp->item;

			/* Delete the inorder successor */
			root->right = remove_node(avl, root->right, temp->item);
		}
	}

	if (root == NULL)
	{
		return root;
	}

	/* Update the depth of the current node */
	root->depth = depth(root->left, root->right);

	/* Balance the node if it has become unbalanced */
	bal = balance(root);

	/* If this node becomes unbalanced, then there are 4 cases */
	/* Left Left Case */
	if (bal > 1 && balance(root->left) >= 0)
		return rotate_right(root);

	/* Left Right Case */
	if (bal > 1 && balance(root->left) < 0)
	{
		root->left = rotate_left(root->left);
		return rotate_right(root);
	}

	/* Right Right Case */
	if (bal < -1 && balance(root->right) <= 0)
		return rotate_left(root);

	/* Right Left Case */
	if (bal < -1 && balance(root->right) > 0)
	{
		root->right = rotate_right(root->right);
		return rotate_left(root);
	}

	return root;
}

int avl_remove(struct avl *avl, const char *item)
{
	if (!avl || !item)
	{
		TRACE("Invalid AVL tree or item.");
		return -1;
	}

	if (!avl->state->root)
	{
		TRACE("Item not found.");
		return -1;
	}

	avl->state->root = remove_node(avl, avl->state->root, item);
	return 0;
}

void avl_close(struct avl *avl)
{
	if (avl)
	{
		scm_close(avl->scm);
		memset(avl, 0, sizeof(struct avl));
	}
	FREE(avl);
}

int avl_insert(struct avl *avl, const char *item)
{
	struct node *root;

	assert(avl);
	assert(safe_strlen(item));

	if (!(root = update(avl, avl->state->root, item)))
	{
		TRACE(0);
		return -1;
	}
	avl->state->root = root;
	return 0;
}

uint64_t
avl_exists(const struct avl *avl, const char *item)
{
	const struct node *node;
	int d;

	assert(avl);
	assert(safe_strlen(item));

	node = avl->state->root;
	while (node)
	{
		if (!(d = strcmp(item, node->item)))
		{
			return node->count;
		}
		node = (0 > d) ? node->left : node->right;
	}
	return 0;
}

void avl_traverse(const struct avl *avl, avl_fnc_t fnc, void *arg)
{
	assert(avl);
	assert(fnc);

	traverse(avl->state->root, fnc, arg);
}

uint64_t
avl_items(const struct avl *avl)
{
	assert(avl);

	return avl->state->items;
}

uint64_t
avl_unique(const struct avl *avl)
{
	assert(avl);

	return avl->state->unique;
}

size_t
avl_scm_utilized(const struct avl *avl)
{
	assert(avl);

	return scm_utilized(avl->scm);
}

size_t
avl_scm_capacity(const struct avl *avl)
{
	assert(avl);

	return scm_capacity(avl->scm);
}

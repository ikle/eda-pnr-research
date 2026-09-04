/*
 * Surface Path Bounding Box
 *
 * Copyright (c) 2008-2026 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <limits.h>

#include "s-path-trace.h"

struct ctx {
	struct s_path_trace trace;
	int lx, ly, hx, hy;
};

static int on_move (void *o, int tx, int ty)
{
	struct ctx *c = o;

	if (tx < c->lx)  c->lx = tx;
	if (ty < c->ly)  c->ly = ty;
	if (tx > c->hx)  c->hx = tx;
	if (ty > c->hy)  c->hy = ty;

	return 1;
}

int s_path_bbox (struct s_path *o, int *lx, int *ly, int *hx, int *hy)
{
	struct ctx c = {
		{0, 0, 0, 0, on_move, on_move, on_move},
		INT_MAX, INT_MAX, INT_MIN, INT_MIN
	};

	if (o->count == 0 || !s_path_trace (&c.trace, o))
		return 0;

	*lx = c.lx; *ly = c.ly;
	*hx = c.hx; *hy = c.hy;
	return 1;
}

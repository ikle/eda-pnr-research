/*
 * Surface Path
 *
 * Copyright (c) 2008-2026 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef S_PATH_H
#define S_PATH_H  1

#include <stdlib.h>

enum s_path_type {
	S_PATH_MOVE,
	S_PATH_LINE,
	S_PATH_CONIC,
	S_PATH_ARC,
	S_PATH_CUBIC,
	S_PATH_CLOSE,
};

union s_path_term {
	struct { int type, len; } action;
	struct { int x, y;      } point;
};

static inline void s_path_action (union s_path_term *o, int type, int len)
{
	o->action.type = type;
	o->action.len  = len;
}

static inline void s_path_point (union s_path_term *o, int x, int y)
{
	o->point.x = x;
	o->point.y = y;
}

struct s_path {
	union s_path_term *term;
	size_t count;
};

static inline void s_path_init (struct s_path *o)
{
	o->term  = NULL;
	o->count = 0;
}

static inline void s_path_fini (struct s_path *o)
{
	free (o->term);
}

int s_path_move  (struct s_path *o, int x, int y);
int s_path_line  (struct s_path *o, int x, int y);
int s_path_conic (struct s_path *o, int cx, int cy, int x, int y);
int s_path_arc   (struct s_path *o, int cx, int cy, int x, int y);
int s_path_cubic (struct s_path *o, int ax, int ay, int bx, int by, int x, int y);
int s_path_close (struct s_path *o);

#endif  /* S_PATH_H */

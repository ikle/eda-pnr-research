/*
 * Surface Path
 *
 * Copyright (c) 2008-2026 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "s-path.h"

static union s_path_term *s_path_push (struct s_path *o, int type, int len)
{
	union s_path_term *term;
	const int count = o->count, next = count + len;

	/* to do: throw overflow error if next < count */

	if ((term = realloc (o->term, next * sizeof (o->term[0]))) == NULL)
		return NULL;

	o->term  = term;
	o->count = next;

	s_path_action (term += count, type, len);
	return term;
}

int s_path_move (struct s_path *o, int x, int y)
{
	union s_path_term *term;

	if ((term = s_path_push (o, S_PATH_MOVE, 2)) == NULL)
		return 0;

	s_path_point (term + 1, x, y);
	return 1;
}

int s_path_line (struct s_path *o, int x, int y)
{
	union s_path_term *term;

	if ((term = s_path_push (o, S_PATH_LINE, 2)) == NULL)
		return 0;

	s_path_point (term + 1, x, y);
	return 1;
}

int s_path_conic (struct s_path *o, int cx, int cy, int x, int y)
{
	union s_path_term *term;

	if ((term = s_path_push (o, S_PATH_CONIC, 3)) == NULL)
		return 0;

	s_path_point (term + 1, cx, cy);
	s_path_point (term + 2,  x,  y);
	return 1;
}

int s_path_arc (struct s_path *o, int cx, int cy, int x, int y)
{
	union s_path_term *term;

	if ((term = s_path_push (o, S_PATH_ARC, 3)) == NULL)
		return 0;

	s_path_point (term + 1, cx, cy);
	s_path_point (term + 2,  x,  y);
	return 1;
}

int s_path_cubic (struct s_path *o, int ax, int ay, int bx, int by, int x, int y)
{
	union s_path_term *term;

	if ((term = s_path_push (o, S_PATH_CUBIC, 4)) == NULL)
		return 0;

	s_path_point (term + 1, ax, ay);
	s_path_point (term + 2, bx, by);
	s_path_point (term + 3,  x,  y);
	return 1;
}

int s_path_close (struct s_path *o)
{
	return s_path_push (o, S_PATH_CLOSE, 1) != NULL;
}


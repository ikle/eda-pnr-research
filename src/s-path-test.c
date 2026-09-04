/*
 * Surface Path Tests
 *
 * Copyright (c) 2008-2026 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>

#include "s-path-trace.h"

#define ARRAY_SIZE(a)  (sizeof (a) / sizeof ((a)[0]))

static const char *s_path_term_name (int type)
{
	static const char *name[] = {
		"move", "line", "conic", "arc", "cubic", "close"
	};

	return type >= 0 && type < ARRAY_SIZE (name) ? name[type] : "unknown";
}

static void s_path_term_show (union s_path_term *o)
{
	const int type = o->action.type, len = o->action.len;
	const char *name = s_path_term_name (type);
	int i;

	printf ("%s", name);

	for (i = 1; i < len; ++i)
		printf (" %d %d", o[i].point.x, o[i].point.y);

	printf (type == S_PATH_CLOSE ? "\n\n" : "\n");
}

static void s_path_show (struct s_path *o)
{
	size_t i;

	for (i = 0; i < o->count; i += o->term[i].action.len)
		s_path_term_show (o->term + i);
}

static int on_open (void *o, int tx, int ty)
{
	printf ("open %d %d\n", tx, ty);
	return 1;
}

static int on_step (void *o, int tx, int ty)
{
	printf ("step %d %d\n", tx, ty);
	return 1;
}

static int on_close (void *o, int tx, int ty)
{
	printf ("close %d %d\n\n", tx, ty);
	return 1;
}

int main (int argc, char *argv[])
{
	struct s_path path, *o = &path;
	struct s_path_trace c = {0, 0, 0, 0, on_open, on_step, on_close};
	int lx, ly, hx, hy;

	s_path_init (o);

	s_path_move  (o, 200, 100);
	s_path_conic (o, 300, 100, 300, 200);
	s_path_conic (o, 300, 300, 200, 300);
	s_path_conic (o, 100, 300, 100, 200);
	s_path_conic (o, 100, 100, 200, 100);
	s_path_close (o);

	s_path_move  (o, 200, 100);
	s_path_arc   (o, 300, 100, 300, 200);
	s_path_arc   (o, 300, 300, 200, 300);
	s_path_arc   (o, 100, 300, 100, 200);
	s_path_arc   (o, 100, 100, 200, 100);
	s_path_close (o);

	s_path_show (o);
	s_path_trace (&c, o);

	s_path_bbox (o, &lx, &ly, &hx, &hy);
	printf ("bbox %d %d - %d %d\n", lx, ly, hx, hy);

	s_path_fini (o);
	return 0;
}

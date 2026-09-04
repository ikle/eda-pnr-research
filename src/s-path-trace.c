/*
 * Surface Path Tracer
 *
 * Copyright (c) 2008-2026 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <math.h>

#include "s-path-trace.h"

static inline int M (int a, int b)
{
	return (a + b) >> 1;  /* assume ariphmetic shift exists */
}

static int do_move (struct s_path_trace *o, int tx, int ty)
{
	return o->open (o, tx, ty) && (o->sx = o->ox = tx, o->sy = o->oy = ty, 1);
}

static int do_line (struct s_path_trace *o, int tx, int ty)
{
	return o->step (o, tx, ty) && (o->sx = tx, o->sy = ty, 1);
}

static int do_close (struct s_path_trace *o, int tx, int ty)
{
	return o->close (o, tx, ty) && (o->sx = tx, o->sy = ty, 1);
}

static int do_conic (struct s_path_trace *o, int cx, int cy, int tx, int ty)
{
	const int sx = o->sx, sy = o->sy;
	const int ox = M (sx, tx), oy = M (sy, ty);
	const int mx = M (ox, cx), my = M (oy, cy);

	if (mx == ox && my == oy)
		return do_line (o, tx, ty);

	return do_conic (o, M (sx, cx), M (sy, cy), mx, my) &&
	       do_conic (o, M (cx, tx), M (cy, ty), tx, ty);
}

#define P(a, b)		((double) a##x * b##x + (double) a##y * b##y)
#define L(a)		sqrt (P (a, a))
#define C(o, k, v)	o##x + (k) * v##x, o##y + (k) * v##y

static int do_arc (struct s_path_trace *o, int cx, int cy, int tx, int ty)
{
	const int sx = o->sx, sy = o->sy;
	const int ax = sx - cx, ay = sy - cy;
	const int bx = tx - cx, by = ty - cy;

	if (((ax | ay) & (bx | by)) == 0)
		return do_line (o, tx, ty);

	const double A = L (a), B = L (b), vx = ax/A + bx/B, vy = ay/A + by/B;
	const double VV = P (v, v);

	if (VV == 0)
		return do_line (o, tx, ty);

	const double k = (2 - sqrt (4 - VV)) / VV;

	const int mx = cx + k * (ax + bx), my = cy + k * (ay + by);
	const int ox = M (sx, tx), oy = M (sy, ty);

	if (mx == ox && my == oy)
		return do_line (o, tx, ty);

	const double h = k * 2;

	return do_arc (o, C (c, h, a), mx, my) &&
	       do_arc (o, C (c, h, b), tx, ty);
}

static int do_cubic (struct s_path_trace *o, int ax, int ay, int bx, int by,
		     int tx, int ty)
{
	const int sx = o->sx, sy = o->sy;
	const int ox = M (sx, tx), oy = M (sy, ty);

	const int cx  = M (ax, bx), cy  = M (ay, by);
	const int aax = M (sx, ax), aay = M (sy, ay);
	const int bbx = M (tx, bx), bby = M (ty, by);

	const int abx = M (aax, cx), aby = M (aay, cy);
	const int bax = M (bbx, cx), bay = M (bby, cy);

	const int mx = M (abx, bax), my = M (aby, bay);

	if (mx == ox && my == oy)
		return do_line (o, tx, ty);

	return do_cubic (o, aax, aay, abx, aby, mx, my) &&
	       do_cubic (o, bax, bay, bbx, bby, tx, ty);
}

#define P1	pt[1].point.x, pt[1].point.y
#define P2	pt[2].point.x, pt[2].point.y
#define P3	pt[3].point.x, pt[3].point.y
#define O	o->ox, o->oy

static int s_path_trace_one (struct s_path_trace *o, union s_path_term *pt)
{
	switch (pt->action.type) {
	case S_PATH_MOVE:   return do_move  (o, P1);
	case S_PATH_LINE:   return do_line  (o, P1);
	case S_PATH_CONIC:  return do_conic (o, P1, P2);
	case S_PATH_ARC:    return do_arc   (o, P1, P2);
	case S_PATH_CUBIC:  return do_cubic (o, P1, P2, P3);
	case S_PATH_CLOSE:  return do_close (o, O);
	}

	return 0;
}

int s_path_trace (struct s_path_trace *o, struct s_path *path)
{
	size_t i;

	for (i = 0; i < path->count; i += path->term[i].action.len)
		if (!s_path_trace_one (o, path->term + i))
			return 0;

	return 1;
}


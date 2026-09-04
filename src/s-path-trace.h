/*
 * Surface Path Tracer
 *
 * Copyright (c) 2008-2026 Alexei A. Smekalkine <ikle@ikle.ru>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef S_PATH_TRACE_H
#define S_PATH_TRACE_H  1

#include "s-path.h"

struct s_path_trace {
	int ox, oy, sx, sy;

	int (*open)  (void *o, int tx, int ty);
	int (*step)  (void *o, int tx, int ty);
	int (*close) (void *o, int tx, int ty);
};

int s_path_trace (struct s_path_trace *o, struct s_path *path);

#endif  /* S_PATH_TRACE_H */

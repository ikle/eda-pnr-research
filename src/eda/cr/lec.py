#!/usr/bin/python3
#
# EDA Channel Router: Left Edge Constrained
#
# Copyright (c) 2025 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

from eda.cr import get_ends, get_spans, get_vcg, vcg_top

eps = frozenset ()

def get_left (F, edge, L):
	F = filter (lambda n: L[n] > edge, F)
	return min (F, key = lambda n: L[n], default = 0)

def route (U, D, LE = eps, RE = eps):
	L, R = get_ends (U, D, LE, RE)
	N, V = get_spans (U, D, L, R, LE, RE), get_vcg (U, D)

	s, t, T = -1, 1, [0] * len (L)

	while N:
		if not (F := vcg_top (N, V)):
			raise ValueError ("Cycle detected")

		if (n := get_left (F, s, L)) == 0:
			s, t = -1, t + 1
		else:
			N.remove (n)
			V = {e for e in V if e[0] != n}
			s = R[n]
			T[n] = t

	return L, R, T

__all__ = ['route']

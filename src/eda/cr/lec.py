#!/usr/bin/python3
#
# EDA Channel Router: Left Edge Constrained
#
# Copyright (c) 2025 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

from eda.cr import get_ends
from eda.cr.vcg import VCG

eps = frozenset ()

def get_left (F, edge, L):
	F = filter (lambda n: L[n] > edge, F)
	return min (F, key = lambda n: L[n], default = 0)

def route (U, D, LE = eps, RE = eps):
	L, R = get_ends (U, D, LE, RE)
	V = VCG (U, D, LE, RE)

	s, t, T = -1, 1, [0] * len (L)

	while V:
		if not (F := V.top):
			raise ValueError ("Cycle detected")

		while (n := get_left (F, s, L)) != 0:
			V.remove (n)
			s = R[n]
			T[n] = t

		s, t = -1, t + 1

	return L, R, T

__all__ = ['route']

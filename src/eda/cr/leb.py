#!/usr/bin/python3
#
# EDA Channel Router: Left Edge Balanced
#
# Copyright (c) 2025 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

from functools import reduce

from eda.cr import get_ends, get_spans, get_vcg, vcg_top
from eda.cr.density import build as cd_density, reduce as cd_reduce

eps = frozenset ()

def left (a, b, L, R, W):
	if R[a] < L[b]:  return a		# disjoint, a on left
	if R[b] < L[a]:  return b		# disjoint, b on left

	if L[a] < L[b]:
		if R[b] < R[a]:  return a	# b in a, return bigger

		LW = max (W [L[a]   : L[b]  ])	# left  arm density (a arm)
		RW = max (W [R[a]+1 : R[b]+1])	# right arm density (b arm)

		return a if LW >= RW else b
	else:
		if R[a] < R[b]:  return b	# a in b, return bigger

		LW = max (W [L[b]   : L[a]  ])	# left  arm density (b arm)
		RW = max (W [R[b]+1 : R[a]+1])	# right arm density (a arm)

		return b if LW >= RW else a

def get_left (F, edge, L, R, W):
	F = [n for n in F if L[n] > edge]

	return reduce (lambda a, b: left (a, b, L, R, W), F) if F else 0

def route (D, U, LE = eps, RE = eps):
	L, R = get_ends (U, D, LE, RE)
	N, V = get_spans (U, D, L, R, LE, RE), get_vcg (U, D)
	W = cd_density (L, R, len (U))

	s, t, T = -1, 1, [0] * len (L)

	while N:
		if not (F := vcg_top (N, V)):
			raise ValueError ("Cycle detected")

		if (n := get_left (F, s, L, R, W)) == 0:
			s, t = -1, t + 1
		else:
			N.remove (n)
			V = {e for e in V if e[0] != n}
			cd_reduce (W, L[n], R[n])
			s = R[n]
			T[n] = t

	return L, R, T

__all__ = ['route']

#!/usr/bin/python3
#
# EDA Channel Router: Left Edge Weighted
#
# Copyright (c) 2025 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

from eda.cr import get_ends
from eda.cr.density import build as cd_build, reduce as cd_reduce
from eda.cr.vcg import VCG

eps = frozenset ()

def get_weight (P, n, L, R, MD, LD, V):
	def fn (LD):
		return 10 / (MD - LD + 1)
		G = [10, 5, 3, 2, 1]
		w = MD - LD
		return G[w] if w < len (G) else 0

#	return sum (map (fn, LD [L[n] : R[n]+1]), P)
	return (sum (map (fn, LD [L[n] : R[n]+1]), P) +
#	        (3 * V.depth (n) + len (V.child[n])) * (R[n]+1 - L[n]) * 0.1)
	        0.3 * V.depth (n) * (R[n]+1 - L[n]))

def get_depths (V, F):
	D = [0] * len (V.child)

	for n in F:
		D[n] = V.depth (n)

	base = max (D) + 2
	return list (map (lambda x: 1 + 10 / (base - x), D))

def get_track (F, L, R, LD, size, V):
	ZP, ZA, P, A, MD = [0] * (size + 1), [0] * (size + 1), 0, 0, max (LD)
	H = get_depths (V, F)

	for n in F:  ZA[L[n]] = n  # max one net in top can start in same x

	for i in reversed (range (size)):
		if (n := ZA[i]) > 0:
#			NP = get_weight (ZP[R[n]+1], n, L, R, MD, LD, V)
			NP = ZP[R[n]+1] + H[n] * (R[n]+1 - L[n])

			if NP >= P:
				P, A = NP, n

		ZP[i], ZA[i] = P, A

#	print ('ZP =', ZP)
#	print ('ZA =', ZA)

	while (n := ZA[i]) > 0:  # ZA for rightmost n end is always zero
		yield n
		i = R[n]+1

def route (U, D, LE = eps, RE = eps):
	L, R = get_ends (U, D, LE, RE)
	V = VCG (U, D, LE, RE)
	W = cd_build (L, R, len (U))

	t, T = 1, [0] * len (L)

	while V:
		if not (F := V.top):
			raise ValueError ("Cycle detected")

#		LD = V.density (W, L, R)

		for n in get_track (F, L, R, W, len (U), V):
			V.remove (n)
#			cd_reduce (W, L[n], R[n])
			T[n] = t

		t = t + 1

	return L, R, T

__all__ = ['route']

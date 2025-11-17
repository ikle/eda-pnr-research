#!/usr/bin/python3
#
# EDA Channel Router: Vertical Constraint Graph
#
# Copyright (c) 2025 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

from itertools import chain

eps = frozenset ()

class VCG:
	def __init__ (o, U, D, LE = eps, RE = eps):
		size = max (chain (U, D, LE, RE)) + 1
		o.child = [set () for _ in range (size)]
		o.ref = [0] * size

		for (u, d) in zip (U, D):
			if u == d:  continue

			if u > 0:
				o.child[0].add (u)

				if d > 0:
					o.child[u].add (d)

			if d > 0:
				o.child[0].add (d)

		o.child[0].update (LE & RE)

		for S in o.child:
			for d in S:
				o.ref[d] += 1

		o.top = {n for n in range (1, size) if o.ref[n] == 1}

	def __bool__ (o):
		return bool (o.child[0])

	def remove (o, n):
		if o.ref[n] != 1:
			raise ValueError ('Can remove top net only')

		o.top.remove (n)

		for d in o.child[n]:
			o.ref[d] -= 1

			if o.ref[d] == 1:
				o.top.add (d)

		o.child[n] = set ()
		o.child[0].remove (n)
		o.ref[n] = 0

__all__ = ['VCG']


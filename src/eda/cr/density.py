#!/usr/bin/python3
#
# EDA Channel Router: Channel Density
#
# Copyright (c) 2025 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

def build (L, R, size):			# O(n+k)
	LD = [0] * (size + 1)

	for n, (l, r) in enumerate (zip (L, R)):
		if n > 0 and l < r:
			LD[l  ] += 1
			LD[r+1] -= 1

	d = LD[0]

	for i in range (1, len (LD)):
		LD[i-1] = d
		d += LD[i]

	return LD[:-1]

__all__ = ['build']


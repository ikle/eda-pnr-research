#!/usr/bin/python3
#
# EDA Channel Router
#
# Copyright (c) 2025 Alexei A. Smekalkine <ikle@ikle.ru>
#
# SPDX-License-Identifier: BSD-2-Clause
#

from itertools import chain

eps = frozenset ()

#
# Validate channel arguments
#
def validate (U, D, LE = eps, RE = eps):
	if len (U) != len (D):
		raise ValueError ('The pin lists are of different sizes')

	if len (U) == 0:
		raise ValueError ('Empty pin set')

	if min (chain (U, D), default = 0) < 0:
		raise ValueError ('Invalid pin network number')

	if min (chain (LE, RE), default = 1) <= 0:
		raise ValueError ('Invalid side network number')

#
# Returns mappings to left, right and track coordinates. All nets are mapped
# to track 0
#
def get_ends (U, D, LE = eps, RE = eps):
	size = max (chain (U, D, LE, RE)) + 1
	rx = len (U) - 1

	L, R, T = [rx] * size, [0] * size, [0] * size

	for x, u, d in zip (range (rx, -1, -1), reversed (U), reversed (D)):
		L[u], L[d] = x, x

	for x, (u, d) in enumerate (zip (U, D)):
		R[u], R[d] = x, x

	for n in LE: L[n] = 0
	for n in RE: R[n] = rx

	return L, R, T

#
# Returns the set of nets which requires horizontal spans to route
#
def get_spans (U, D, L, R, LE = eps, RE = eps):
	N = chain (U, D, LE & RE)
	return set (filter (lambda n: n > 0 and L[n] < R[n], N))

#
# Returns vertical constraint graph (VCG) as a set of arrows from top pin to
# bottom pin
#
def get_vcg (U, D):
	return {(u, d) for (u, d) in zip (U, D) if u > 0 and d > 0}

#
# Returns top nets. N -- set of nets, V -- VCG
#
def vcg_top (N, V):
	return N - {e[1] for e in V}

#
# Returns bottom nets. N -- set of nets, V -- VCG
#
def vcg_bot (N, V):
	return N - {e[0] for e in V}

__all__ = ['get_ends', 'get_spans', 'get_vcg', 'vcg_top', 'vcg_bot']


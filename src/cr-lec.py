#!/usr/bin/python3

from eda.cr import get_ends, get_spans, get_vcg, vcg_top

def get_left (N, V, edge, L):
	if not (T := vcg_top (N, V)):
		raise ValueError ("Cycle detected")

	F = filter (lambda n: L[n] > edge, T)
	return min (F, key = lambda n: L[n], default = 0)

def cr_lec (D, U):
	L, R, T = get_ends (U, D)
	N = get_spans (U, D, L, R)

	V, s, t = get_vcg (U, D), -1, 1

	while N:
		if (n := get_left (N, V, s, L)) == 0:
			s, t = -1, t + 1
		else:
			T[n] = t
			N.remove (n)
			V = {e for e in V if e[0] != n}
			s = R[n]

	return L, R, T

def get_density (U, D):				# O(n+k)
	L, R, _ = get_ends (U, D)
	LD = [0] * (len (D) + 1)

	for n, (l, r) in enumerate (zip (L, R)):
		if n > 0 and l < r:
			LD[l  ] += 1
			LD[r+1] -= 1

	d = LD[0]

	for i in range (1, len (LD)):
		LD[i-1] = d
		d += LD[i]

	return LD[:-1]

def show_tracks (T):
	S = [list () for t in range (max (T))]

	for n, t in enumerate (T):
		if n > 0 and t > 0:
			S[t-1].append (n)

	print (S)

#    0  1  2  3  4  5  6  7  8  9  10  11
U = [0, 1, 4, 5, 1, 6, 7, 0, 4, 9, 10, 10]
D = [2, 3, 5, 3, 5, 2, 6, 8, 9, 8,  7,  9]

if False:					# O(n*k)
	L, R, T = get_ends (U, D)

	S = [set () for _ in range (len (U))]

	for n, (l, r) in enumerate (zip (L, R)):
		if n > 0 and l < r:
			for i in range (l, r + 1):
				S[i].add (n)

	LD = [len (x) for x in S]

	print (S)	# nets per column
	print (LD)	# local density

L, R, T = cr_lec (D, U)

show_tracks (T)
print (get_density (U, D))


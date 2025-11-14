#!/usr/bin/python3

from eda.cr     import get_ends
from eda.cr.lec import route as cr_lec

def get_density (U, D):				# O(n+k)
	L, R = get_ends (U, D)
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
	S = [[] for t in range (max (T))]

	for n, t in enumerate (T):
		if n > 0 and t > 0:
			S[t-1].append (n)

	print (S)

#    0  1  2  3  4  5  6  7  8  9  10  11
U = [0, 1, 4, 5, 1, 6, 7, 0, 4, 9, 10, 10]
D = [2, 3, 5, 3, 5, 2, 6, 8, 9, 8,  7,  9]

L, R, T = cr_lec (D, U)

show_tracks (T)
print (get_density (U, D))

if False:					# O(n*k)
	L, R = get_ends (U, D)

	S = [set () for _ in range (len (U))]

	for n, (l, r) in enumerate (zip (L, R)):
		if n > 0 and l < r:
			for i in range (l, r + 1):
				S[i].add (n)

	LD = [len (x) for x in S]

	print (S)	# nets per column
	print (LD)	# local density


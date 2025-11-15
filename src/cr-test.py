#!/usr/bin/python3

from eda.cr.lec import route as cr_lec
from eda.cr.leb import route as cr_leb

def show_tracks (prefix, T):
	S = [[] for t in range (max (T))]

	for n, t in enumerate (T):
		if n > 0 and t > 0:
			S[t-1].append (n)

	print (prefix, S)

def test_lec (U, D):
	L, R, T = cr_lec (D, U)

	show_tracks ('LEC =', T)

def test_leb (U, D):
	L, R, T = cr_leb (D, U)

	show_tracks ('LEB =', T)

#    0  1  2  3  4  5  6  7  8  9  10  11
U = [0, 1, 4, 5, 1, 6, 7, 0, 4, 9, 10, 10]
D = [2, 3, 5, 3, 5, 2, 6, 8, 9, 8,  7,  9]

test_lec (U, D)
test_leb (U, D)

if False:					# O(n*k)
	S = [set () for _ in range (len (U))]

	for n, (l, r) in enumerate (zip (L, R)):
		if n > 0 and l < r:
			for i in range (l, r + 1):
				S[i].add (n)

	LD = [len (x) for x in S]

	print (S)	# nets per column
	print (LD)	# local density


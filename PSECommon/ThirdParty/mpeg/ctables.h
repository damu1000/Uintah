/*************************************************************
Copyright (C) 1990, 1991, 1993 Andy C. Hung, all rights reserved.
PUBLIC DOMAIN LICENSE: Stanford University Portable Video Research
Group. If you use this software, you agree to the following: This
program package is purely experimental, and is licensed "as is".
Permission is granted to use, modify, and distribute this program
without charge for any purpose, provided this license/ disclaimer
notice appears in the copies.  No warranty or maintenance is given,
either expressed or implied.  In no event shall the author(s) be
liable to you or a third party for any special, incidental,
consequential, or other damages, arising out of the use or inability
to use the program for any purpose (or the loss of data), even if we
have been advised of such possibilities.  Any public reference or
advertisement of this source code should refer to it as the Portable
Video Research Group (PVRG) code, and not by any author(s) (or
Stanford University) name.
*************************************************************/
/*
************************************************************
tables.h

These are the Huffman tables used in the mpeg coder.  This is
converted by htable into ctables.h which is used by the Huffman coder.

************************************************************
*/

/* Table B1 */
/* Macroblock addressing */

int MBACoeff[] = {
1,1,1,
2,3,3,
3,3,2,
4,4,3,
5,4,2,
6,5,3,
7,5,2,
8,7,7,
9,7,6,
10,8,11,
11,8,10,
12,8,9,
13,8,8,
14,8,7,
15,8,6,
16,10,23,
17,10,22,
18,10,21,
19,10,20,
20,10,19,
21,10,18,
22,11,35,
23,11,34,
24,11,33,
25,11,32,
26,11,31,
27,11,30,
28,11,29,
29,11,28,
30,11,27,
31,11,26,
32,11,25,
33,11,24,
34,11,15,             /* Stuffing */
35,11,8,             /* Escape */
36,23,0, /* MBSC Head */
-1,-1};

/* Table B2 */

int IntraTypeCoeff[] = {
0,1,1,
1,2,1,
-1,-1};

int PredictedTypeCoeff[] = {
0,1,1,
1,2,1,
2,3,1,
3,5,3,
4,5,2,
5,5,1,
6,6,1,
-1,-1};

int InterpolatedTypeCoeff[] = {
0,2,2,
1,2,3,
2,3,2,
3,3,3,
4,4,2,
5,4,3,
6,5,3,
7,5,2,
8,6,3,
9,6,2,
10,6,1,
-1,-1};

int DCTypeCoeff[] = {
0,1,1,
-1,-1};

/* Table B3 */

int CBPCoeff[] = {
60,3,7,
4,4,13,
8,4,12,
16,4,11,
32,4,10,
12,5,19,
48,5,18,
20,5,17,
40,5,16,
28,5,15,
44,5,14,
52,5,13,
56,5,12,
1,5,11,
61,5,10,
2,5,9,
62,5,8,
24,6,15,
36,6,14,
3,6,13,
63,6,12,
5,7,23,
9,7,22,
17,7,21,
33,7,20,
6,7,19,
10,7,18,
18,7,17,
34,7,16,
7,8,31,
11,8,30,
19,8,29,
35,8,28,
13,8,27,
49,8,26,
21,8,25,
41,8,24,
14,8,23,
50,8,22,
22,8,21,
42,8,20,
15,8,19,
51,8,18,
23,8,17,
43,8,16,
25,8,15,
37,8,14,
26,8,13,
38,8,12,
29,8,11,
45,8,10,
53,8,9,
57,8,8,
30,8,7,
46,8,6,
54,8,5,
58,8,4,
31,9,7,
47,9,6,
55,9,5,
59,9,4,
27,9,3,
39,9,2,
-1,-1};

/* Table B4 (Correct!) */

int MVDCoeff[] = {
17,11,25,
18,11,27,
19,11,29,
20,11,31,
21,11,33,
22,11,35,
23,10,19,
24,10,21,
25,10,23,
26,8,7,
27,8,9,
28,8,11,
29,7,7,
30,5,3,
31,4,3,
32,3,3,
0,1,1,
1,3,2,
2,4,2,
3,5,2,
4,7,6,
5,8,10,
6,8,8,
7,8,6,
8,10,22,
9,10,20,
10,10,18,
11,11,34,
12,11,32,
13,11,30,
14,11,28,
15,11,26,
16,11,24,
-1,-1};

/* Table B5 */

int DCLumCoeff[] = {
0,3,4,
1,2,0,
2,2,1,
3,3,5,
4,3,6,
5,4,14,
6,5,30,
7,6,62,
8,7,126,
-1,-1};
/* 8,6,63, 1/15/93*/

int DCChromCoeff[] = {
0,2,0,
1,2,1,
2,2,2,
3,3,6,
4,4,14,
5,5,30,
6,6,62,
7,7,126,
8,8,254,
-1,-1};
/* 8,6,63, 1/15/93*/

int TCoeff1[] = {
0,2,2, /* EOF */
1,2,3, /* Not First Coef */
257,3,3,
2,4,4,
513,4,5,
3,5,5,
769,5,7,
1025,5,6,
258,6,6,
1281,6,7,
1537,6,5,
1793,6,4,
4,7,6,
514,7,4,
2049,7,7,
2305,7,5,
5,8,38,
6,8,33,
259,8,37,
770,8,36,
2561,8,39,
2817,8,35,
3073,8,34,
3329,8,32,
7,10,10,
260,10,12,
515,10,11,
1026,10,15,
1282,10,9,
3585,10,14,
3841,10,13,
4097,10,8, /* end of first table */
8,12,29,
9,12,24,
10,12,19,
11,12,16,
261,12,27,
516,12,20,
771,12,28,
1027,12,18,
1538,12,30,
1794,12,21,
2050,12,17,
4353,12,31,
4609,12,26,
4865,12,25,
5121,12,23,
5377,12,22,
12,13,26,
13,13,25,
14,13,24,
15,13,23,
262,13,22,
263,13,21,
517,13,20,
772,13,19,
1283,13,18,
2306,13,17,
2562,13,16,
5633,13,31,
5889,13,30,
6145,13,29,
6401,13,28,
6657,13,27, /* end of second table */
16,14,31,
17,14,30,
18,14,29,
19,14,28,
20,14,27,
21,14,26,
22,14,25,
23,14,24,
24,14,23,
25,14,22,
26,14,21,
27,14,20,
28,14,19,
29,14,18,
30,14,17,
31,14,16,
32,15,24,
33,15,23,
34,15,22,
35,15,21,
36,15,20,
37,15,19,
38,15,18,
39,15,17,
40,15,16,
264,15,31,
265,15,30,
266,15,29,
267,15,28,
268,15,27,
269,15,26,
270,15,25, /* end of third table */
271,16,19,
272,16,18,
273,16,17,
274,16,16,
1539,16,20,
2818,16,26,
3074,16,25,
3330,16,24,
3586,16,23,
3842,16,22,
4098,16,21,
6913,16,31,
7169,16,30,
7425,16,29,
7681,16,28,
7937,16,27, /* End of fourth table */
7167,6,1, /* Escape */
-1,-1
};

int TCoeff2[] = {
1,1,1, /* First Coef */
257,3,3,
2,4,4,
513,4,5,
3,5,5,
769,5,7,
1025,5,6,
258,6,6,
1281,6,7,
1537,6,5,
1793,6,4,
4,7,6,
514,7,4,
2049,7,7,
2305,7,5,
5,8,38,
6,8,33,
259,8,37,
770,8,36,
2561,8,39,
2817,8,35,
3073,8,34,
3329,8,32,
7,10,10,
260,10,12,
515,10,11,
1026,10,15,
1282,10,9,
3585,10,14,
3841,10,13,
4097,10,8, /* end of first table */
8,12,29,
9,12,24,
10,12,19,
11,12,16,
261,12,27,
516,12,20,
771,12,28,
1027,12,18,
1538,12,30,
1794,12,21,
2050,12,17,
4353,12,31,
4609,12,26,
4865,12,25,
5121,12,23,
5377,12,22,
12,13,26,
13,13,25,
14,13,24,
15,13,23,
262,13,22,
263,13,21,
517,13,20,
772,13,19,
1283,13,18,
2306,13,17,
2562,13,16,
5633,13,31,
5889,13,30,
6145,13,29,
6401,13,28,
6657,13,27, /* end of second table */
16,14,31,
17,14,30,
18,14,29,
19,14,28,
20,14,27,
21,14,26,
22,14,25,
23,14,24,
24,14,23,
25,14,22,
26,14,21,
27,14,20,
28,14,19,
29,14,18,
30,14,17,
31,14,16,
32,15,24,
33,15,23,
34,15,22,
35,15,21,
36,15,20,
37,15,19,
38,15,18,
39,15,17,
40,15,16,
264,15,31,
265,15,30,
266,15,29,
267,15,28,
268,15,27,
269,15,26,
270,15,25, /* end of third table */
271,16,19,
272,16,18,
273,16,17,
274,16,16,
1539,16,20,
2818,16,26,
3074,16,25,
3330,16,24,
3586,16,23,
3842,16,22,
4098,16,21,
6913,16,31,
7169,16,30,
7425,16,29,
7681,16,28,
7937,16,27, /* End of fourth table */
7167,6,1, /* Escape */
-1,-1
};

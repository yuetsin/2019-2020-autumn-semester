#!/usr/bin/env python

raw_ply = """0.894756 1.014203 -0.123518 0.000000 0.000000 -1.000000 0.996915 0.994523 1.0 0.0
0.894756 -1.019531 -0.123518 0.000000 0.000000 -1.000000 0.014263 0.992784 0.0 0.0
-0.896903 -1.019531 -0.123518 0.000000 0.000000 -1.000000 0.003084 -0.005472 0.0 1.0
-0.896903 1.014203 -0.123518 0.000000 -0.000000 -1.000000 0.996318 -0.007123 1.0 1.0
0.894756 1.014203 -0.107438 0.000000 1.000000 -0.000000 0.994614 0.995740 1.0 0.0
0.894756 1.014203 -0.123518 0.000000 1.000000 -0.000000 0.996915 0.994523 0.0 0.0
-0.896903 1.014203 -0.123518 0.000000 1.000000 -0.000000 0.996318 -0.007123 0.0 1.0
-0.896903 1.014203 -0.107438 0.000000 1.000000 0.000000 0.997998 -0.003295 1.0 1.0
-0.896903 -1.019531 -0.123518 -1.000000 0.000000 0.000000 0.003084 -0.005472 1.0 0.0
-0.896903 -1.019531 -0.107438 -1.000000 0.000000 0.000000 0.000094 0.000094 0.0 0.0
-0.896903 1.014203 -0.107438 -1.000000 0.000000 0.000000 0.997998 -0.003295 0.0 1.0
-0.896903 1.014203 -0.123518 -1.000000 0.000000 0.000000 0.996318 -0.007123 1.0 1.0
0.894756 -1.019531 -0.123518 -0.000000 -1.000000 -0.000000 0.014263 0.992784 1.0 0.0
0.894756 -1.019531 -0.107438 -0.000000 -1.000000 -0.000000 0.007291 0.992349 0.0 0.0
-0.896903 -1.019531 -0.107438 -0.000000 -1.000000 -0.000000 0.000094 0.000094 0.0 1.0
-0.896903 -1.019531 -0.123518 -0.000000 -1.000000 0.000000 0.003084 -0.005472 1.0 0.0
0.894756 1.014203 -0.123518 1.000000 -0.000000 0.000000 0.996915 0.994523 0.0 0.0
0.894756 1.014203 -0.107438 1.000000 -0.000000 0.000000 0.994614 0.995740 0.0 1.0
0.894756 -1.019531 -0.107438 1.000000 -0.000000 0.000000 0.007291 0.992349 1.0 1.0
0.894756 -1.019531 -0.123518 1.000000 -0.000000 0.000000 0.014263 0.992784 1.0 0.0
0.894757 1.014202 0.123518 -0.000000 0.000000 1.000000 0.168515 0.001903 0.0 0.0
-0.896903 1.014203 0.123518 -0.000000 0.000000 1.000000 0.824592 -0.012387 0.0 1.0
-0.896903 -1.020661 0.123518 -0.000000 0.000000 1.000000 0.832407 1.002662 1.0 1.0
0.894756 -1.020662 0.123518 0.000000 -0.000000 1.000000 0.180895 0.994128 0.0 0.0
0.894757 1.014202 0.107438 1.000000 -0.000000 0.000000 0.000095 0.000095 0.0 0.0
0.894757 1.014202 0.123518 1.000000 -0.000000 0.000000 0.058957 0.079507 0.0 1.0
0.894756 -1.020662 0.123518 1.000000 -0.000000 0.000000 0.071337 0.852615 1.0 1.0
0.894756 -1.020662 0.107438 1.000000 -0.000000 -0.000000 0.000095 0.906531 0.0 0.0
0.894756 -1.020662 0.107438 -0.000001 -1.000000 -0.000007 0.000095 0.906531 0.0 0.0
0.894756 -1.020662 0.123518 -0.000001 -1.000000 -0.000007 0.071337 0.852615 0.0 1.0
-0.896903 -1.020661 0.123518 -0.000001 -1.000000 -0.000007 0.841537 0.920493 1.0 1.0
0.894756 -1.020662 0.107438 -0.000000 -1.000000 -0.000000 0.000095 0.906531 0.0 0.0
-0.896903 -1.020661 0.123518 -0.000000 -1.000000 -0.000000 0.841537 0.920493 0.0 0.0
-0.896903 -1.020661 0.107438 -0.000000 -1.000000 -0.000000 0.900398 0.999905 0.0 1.0
-0.896903 -1.020661 0.107438 -1.000000 0.000000 -0.000007 0.900398 0.999905 1.0 1.0
-0.896903 -1.020661 0.123518 -1.000000 0.000000 -0.000007 0.841537 0.920493 0.0 0.0
-0.896903 1.014203 0.123518 -1.000000 0.000000 -0.000007 0.829157 0.147386 0.0 0.0
-0.896903 1.014203 0.107438 -1.000000 0.000000 -0.000007 0.900399 0.093470 0.0 1.0
0.894757 1.014202 0.123518 0.000000 1.000000 -0.000000 0.058957 0.079507 1.0 1.0
0.894757 1.014202 0.107438 0.000000 1.000000 -0.000000 0.000095 0.000095 0.0 0.0
-0.896903 1.014203 0.107438 0.000000 1.000000 -0.000000 0.900399 0.093470 0.0 0.0
-0.896903 1.014203 0.123518 0.000000 1.000000 0.000000 0.829157 0.147386 0.0 1.0
0.894756 -1.019531 -0.107438 1.000000 0.000000 -0.000002 0.007291 0.992349 1.0 1.0
0.894756 1.014203 -0.107438 1.000000 0.000000 -0.000002 0.994614 0.995740 0.0 0.0
0.894757 1.014202 0.107438 1.000000 0.000000 -0.000002 0.000095 0.000095 0.0 0.0
0.894756 -1.019531 -0.107438 1.000000 -0.000000 0.000002 0.007291 0.992349 0.0 1.0
0.894757 1.014202 0.107438 1.000000 -0.000000 0.000002 0.000095 0.000095 1.0 1.0
0.894756 -1.020662 0.107438 1.000000 -0.000000 0.000002 0.000095 0.906531 0.0 0.0"""


fin_str = "float vertices[] = {\n"

for line in raw_ply.split('\n'):
    n = [float(i) for i in line.split(' ')]
    fin_str += '%.2ff, %.2ff,%.2ff,%.2ff,%.2ff,%.2ff,%.2ff,%.2ff,\n' % (
        n[0], n[1], n[2], n[3], n[4], n[5], n[6], n[7])

fin_str += "};"

print(fin_str)

#! /usr/bin/python

proc = [1] + range(4, 512 + 4, 4)
matrixes = [4, 16, 32, 64] + range(128, 3200 + 128, 128)
clusters = [2, 4, 4, 8] + range(128 / 64, 3200 / 64 + 128 / 64, 128 / 64)

proc.reverse()

with open("bgp-set-bench.txt", "w") as f:
    for p in proc:
        for n in xrange(len(matrixes)):
            f.write(str(matrixes[n]) + " " + str(clusters[n]) + " " + str(p) + " 0\n")

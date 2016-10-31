#! /usr/bin/python

a = [250, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500, 2750, 3000, 3250, 3500, 3750, 4000, 4250, 4500, 4750, 5000]
commands = []
for i in a:
    commands.append(str(i) + " 1 0")
with open("bgp-1proc.txt", "w") as f:
    f.write("\n".join(commands))


def Generate(fin, a, b):
    b.reverse()
    commands = []
    for j in b:
        for i in a:
            commands.append(str(i) + " " + str(j) + " 0")

    with open(fin, "w") as f:
        f.write("\n".join(commands))


Generate(
    "bgp-256-small.txt",
    [250, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500],
    [16, 32, 64, 128, 256])

Generate(
    "bgp-256-medium.txt",
    [2750, 3000, 3250, 3500, 3750, 4000],
    [16, 32, 64, 128, 256])

Generate(
    "bgp-256-big.txt",
    [4250, 4500, 4750, 5000],
    [16, 32, 64, 128, 256])




Generate(
    "bgp-512-small.txt",
    [250, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500],
    [384, 512])

Generate(
    "bgp-512-medium.txt",
    [2750, 3000, 3250, 3500, 3750, 4000],
    [384, 512])

Generate(
    "bgp-512-big.txt",
    [4250, 4500, 4750, 5000],
    [384, 512])




Generate(
    "bgp-1024-small.txt",
    [250, 500, 750, 1000, 1250, 1500, 1750, 2000, 2250, 2500],
    [640, 768, 896, 1024])

Generate(
    "bgp-1024-medium.txt",
    [2750, 3000, 3250, 3500, 3750, 4000],
    [640, 768, 896, 1024])

Generate(
    "bgp-1024-big.txt",
    [4250, 4500, 4750, 5000],
    [640, 768, 896, 1024])

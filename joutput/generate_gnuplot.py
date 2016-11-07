#! /usr/bin/python

log_files = [
    "output-bgp-1proc.txt",
    "output-bgp-256-milli.txt",
    "output-bgp-256-small.txt",
    "output-bgp-256-medium.txt",
    "output-bgp-256-big.txt",
    "output-bgp-512-milli.txt",
    "output-bgp-512-small.txt",
    "output-bgp-512-medium.txt",
    "output-bgp-512-big.txt",
    "output-bgp-1024-milli.txt",
    "output-bgp-1024-small.txt",
    "output-bgp-1024-medium.txt",
    "output-bgp-1024-big.txt",
]

data = []
for log_file in log_files:
    with open(log_file, "r") as f:
        while True:
            obj = {}
            line = f.readline()
            if line.strip() == "":
                break

            values = line.split()
            obj["n"] = int(values[1])
            obj["m"] = int(values[3])
            obj["k"] = int(values[5])
            obj["p"] = int(values[7])
            obj["b-time"] = float(f.readline().split()[1])
            obj["s-time"] = float(f.readline().split()[1])
            obj["a-time"] = int(f.readline().split()[1])
            if obj["a-time"] == 0:
                obj["a-time"] = 1.0
            else:
                obj["a-time"] = float(obj["a-time"])
            obj["it"] = int(f.readline().split()[1]) + 1

            data.append(obj)

            f.readline()


def cmpFunc(x, y):

    if x["p"] < y["p"]:
        return 1
    elif x["p"] == y["p"]:
        if x["n"] < y["n"]:
            return 1
        elif x["n"] == y["n"]:
            return 0
        else:
            return -1
    else:
        return -1


data.sort(cmp=cmpFunc, reverse=False)


def countGFlops(value):
    n = value["n"]
    k = value["k"]
    overall_time = value["a-time"]
    if overall_time == 0:
        overall_time = 1
    return float(((n * n * (2 * k - 1) - k * k * (n - 1.0 / 2) + n - (3.0 / 2) * k) + value["it"] * (k * n * n - 3 * n * k + k * k + (n - 1) * (n - k) * k))) / overall_time / 1000000.0


with open("gflops.dat", "w") as gnuplot_file:
    gnuplot_file.write("# x1 x2 y\n")

    prev_value_p = 0
    for value in data:
        if value["p"] > 1:
            if prev_value_p != value["p"]:
                prev_value_p = value["p"]
                gnuplot_file.write("\n")

            GFlops = countGFlops(value)

            gnuplot_file.write(str(value["p"]) + " " + str(value["n"]) + " " + str(GFlops) + "\n")


def find_arr(arr, cfn):
    for i in xrange(len(arr)):
        if cfn(arr[i]) is True:
            return i
    return -1


with open("percents.dat", "w") as gnuplot_file:
    gnuplot_file.write("# x1 x2 y\n")

    prev_value_p = 0
    for value in data:
        if value["p"] > 1:
            # if value["p"] > 1 and value["n"] <= 4000:
            if prev_value_p != value["p"]:
                prev_value_p = value["p"]
                gnuplot_file.write("\n")

            etalon = find_arr(data, lambda x: x["p"] == 1 and x["n"] == value["n"])
            if etalon >= 0:

                # Normalized efficiency (%)
                # attitude = (data[etalon]["a-time"] / data[etalon]["it"]) / (value["a-time"] / value["it"] * value["p"])
                #
                # The same result ?
                # attitude = countGFlops(value) / (countGFlops(data[etalon]) * value["p"]) * 100

                # Efficiency (%)
                attitude = (data[etalon]["a-time"]) / (value["a-time"] * value["p"])

                # Iterations amount
                # attitude = value["it"]

                # Strong scaling
                # gigaflops
                # attitude = countGFlops(value)

                # Weak scaling
                # gigaflops per processes
                # attitude = countGFlops(value) / value["p"]

                gnuplot_file.write(str(value["p"]) + " " + str(value["n"]) + " " + str(attitude) + "\n")

#! /usr/bin/python

log_files = [
    "output-bgp-bench.txt"
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
    return float(((n * n * (2 * k - 1) - k * k * (n - 1.0 / 2) + n - (3.0 / 2) * k) + value["it"] * (k * n * n - 3 * n * k + k * k + (n - 1) * (n - k) * k))) / value["a-time"] / 1000000000.0


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


x_cells_num = 0

with open("efficiency.dat", "w") as gnuplot_file:
    gnuplot_file.write("# x1 x2 y\n")

    prev_value_p = 0
    for value in data:

        # if value["p"] > 1:
            if prev_value_p != value["p"]:
                prev_value_p = value["p"]
                gnuplot_file.write("\n")

                if x_cells_num == 0:
                    x_cells_num = data.index(value)

            etalon = find_arr(data, lambda x: x["p"] == 1 and x["n"] == value["n"])
            if etalon >= 0:

                # Normalized efficiency (%)
                attitude = (data[etalon]["a-time"] / data[etalon]["it"]) / (value["a-time"] / value["it"] * value["p"])
                #
                # The same result ?
                # attitude = countGFlops(value) / (countGFlops(data[etalon]) * value["p"])

                # Efficiency (%)
                # attitude = (data[etalon]["a-time"]) / (value["a-time"] * value["p"])

                # Iterations amount
                # attitude = value["it"]

                # Strong scaling
                # gigaflops
                # attitude = countGFlops(value)

                # Weak scaling
                # gigaflops per processes
                # attitude = countGFlops(value) / value["p"]

                value["u"] = attitude

                gnuplot_file.write(str(value["p"]) + " " + str(value["n"]) + " " + str(attitude * 100) + "\n")

y_cells_num = len(data) / x_cells_num - 1  # no p == 1

data = data[:x_cells_num * y_cells_num]
data.reverse()

markProc = 0.0
markData = 0.0
markAll = 0.0

maxP_minP = data[y_cells_num * x_cells_num - 1]["p"] - data[0]["p"]
maxD_minD = data[x_cells_num - 1]["n"] - data[0]["n"]

for j in xrange(y_cells_num - 1):
    for i in xrange(x_cells_num - 1):
        dEP = (data[j * x_cells_num + i]["u"] - data[(j + 1) * x_cells_num + i]["u"] + data[j * x_cells_num + i + 1]["u"] - data[(j + 1) * x_cells_num + i + 1]["u"]) / 2.0
        markP = dEP * (data[j * x_cells_num + i]["p"] - data[(j + 1) * x_cells_num + i]["p"]) / maxP_minP

        dED = (data[j * x_cells_num + i]["u"] + data[(j + 1) * x_cells_num + i]["u"] - data[j * x_cells_num + i + 1]["u"] - data[(j + 1) * x_cells_num + i + 1]["u"]) / 2.0
        markD = dED * (data[j * x_cells_num + i]["n"] - data[j * x_cells_num + i + 1]["n"]) / maxD_minD

        markA = (dEP + dED) / 2.0 * (data[j * x_cells_num + i]["p"] - data[(j + 1) * x_cells_num + i]["p"]) * (data[j * x_cells_num + i]["n"] - data[j * x_cells_num + i + 1]["n"]) / maxP_minP / maxD_minD

        markProc += markP
        markData += markD
        markAll += markA

markProc = markProc / ((x_cells_num - 1) * (y_cells_num - 1))
markData = markData / ((x_cells_num - 1) * (y_cells_num - 1))
markAll = markAll / ((x_cells_num - 1) * (y_cells_num - 1))

print "minP=", data[0]["p"]
print "maxP=", data[y_cells_num * x_cells_num - 1]["p"]
print "minD=", data[0]["n"]
print "maxD=", data[x_cells_num - 1]["n"]
print "markProc= ", markProc
print "markData= ", markData
print "markAll= ", markAll
print "minEn=", min(data, key=lambda x: x["u"])["u"]
print "maxEn=", max(data, key=lambda x: x["u"])["u"]

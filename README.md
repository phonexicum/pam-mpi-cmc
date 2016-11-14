# pam-mpi-cmc

This repository contains **Partitioning Around Medoids (PAM)** algorithm written using MPI technology for supercomputers at Moscow State University (The faculty of Computational Mathematics and Cybernetics)

Considered supercomputers:

- Blue Gene/P (g++ -std=gnu++98)
- *Lomonosov (could have been used)*

In realization synchronized MPI Send/Recv are used.

OpenMP not used (but it could have been used).

## Repository structure

- `./main.cpp` - C++ file containing benchmark for PAM algorithm
- `./pam.h` and `./pam.cpp` - C++ PAM realization

- `./data/` - contains data to be processed clusterized
- `./joutput/` and `./joutput-detailed/` - contains results of PAM benchmark on *Blue Gene/P* supercomputer

    - `./generate_gnuplot.py` - generates `./*.dat` files for their plotting
    - `./gnuplot.script` - gnuplot script which generates plots representing *matrix multiplication performance*

- `./settings/` and `./settings-detailed/` - directory contains sets of settings for PAM testing


## Algorithm description

***Partitioning Around Medoids (PAM)*** is a clusterization algorithm.

PAM must not be messed up with *k-medoids* algorithm, in its classical meaning, though they are quite similar.

k-medoids is similar to k-medians, with the difference of selecting element from initially given multitude, but not some average point in the area for current cluster. However, both algorithms operates with points themselves and counts distance between them.

PAM consists of two steps:

- BUILD step - selection of first approximation for k clusters. (whereas k-medoids and k-means in their classic description, selects clusters randomly)
- SWAP step - algorithm searchs best pair (medoid, non-medoid) to swap these elements and improve objective function (cluster devision)

Detailed PAM algorithm description can be found on [Algowiki project (russian only)](https://algowiki-project.org/ru/Partitioning_Around_Medoids_(PAM))


## Experiment visualiztion

`./data/generate_experiment.sh` generates a lot of pseudo-random numbers to be used as point coordinates for benchmark.

- Gflops (FLoating-point Operations Per Second) = amount of *floating* operations (for PAM that are `sum` and `max`) / algorithm execution time
- Efficiency (%) = GFlops for `p` processes for `n` sized multitude / GFlops for 1 process for `n` sized multitude / `p` processes * 100%
- Absolute efficiency (%) = GFlops for `p` processes for `n` sized multitude / (3.4 GFlops * p) * 100%

3.4 Gflops - is maximum performance of 1 process kernel for PowerPC 450 (850 MHz) on BlueGene/P.

Algorithm works **considerably** faster, if `n * (n-k) % p == 0`.
It is hard to generate good graphic for this property, therefore I prefered to work around those points in graphic (for not detailed settings)
(n= 1250 k= 25 p= 896, n= 4000 m= 5 k= 80 p= 640)

In detailed settings this special cases can be easily seen (on pictures they are ordered in lines).

## supercomputer commands

### Blue Gene

```
mpisubmit.bg -n 128 -m VN -w 00:15:00 -t PREFER_TORUS ./pam settings-detailed/bgp-set-bench.txt data/experiment-detailed.txt output/output-bgp-bench.txt


mpisubmit.bg -n 1 -m smp -w 02:00:00 -t PREFER_TORUS ./pam settings/bgp-1proc.txt data/experiment.txt output/output-bgp-1proc.txt

mpisubmit.bg -n 64 -m VN -w 02:00:00 -t PREFER_TORUS ./pam settings/bgp-256-milli.txt data/experiment.txt output/output-bgp-256-milli.txt
mpisubmit.bg -n 64 -m VN -w 02:00:00 -t PREFER_TORUS ./pam settings/bgp-256-small.txt data/experiment.txt output/output-bgp-256-small.txt
mpisubmit.bg -n 64 -m VN -w 02:00:00 -t PREFER_TORUS ./pam settings/bgp-256-medium.txt data/experiment.txt output/output-bgp-256-medium.txt
mpisubmit.bg -n 64 -m VN -w 02:00:00 -t PREFER_TORUS ./pam settings/bgp-256-big.txt data/experiment.txt output/output-bgp-256-big.txt

mpisubmit.bg -n 128 -m VN -w 00:15:00 -t PREFER_TORUS ./pam settings/bgp-512-milli.txt data/experiment.txt output/output-bgp-512-milli.txt
mpisubmit.bg -n 128 -m VN -w 00:15:00 -t PREFER_TORUS ./pam settings/bgp-512-small.txt data/experiment.txt output/output-bgp-512-small.txt
mpisubmit.bg -n 128 -m VN -w 00:15:00 -t PREFER_TORUS ./pam settings/bgp-512-medium.txt data/experiment.txt output/output-bgp-512-medium.txt
mpisubmit.bg -n 128 -m VN -w 00:15:00 -t PREFER_TORUS ./pam settings/bgp-512-big.txt data/experiment.txt output/output-bgp-512-big.txt

mpisubmit.bg -n 256 -m VN -w 00:10:00 -t PREFER_TORUS ./pam settings/bgp-1024-milli.txt data/experiment.txt output/output-bgp-1024-milli.txt
mpisubmit.bg -n 256 -m VN -w 00:10:00 -t PREFER_TORUS ./pam settings/bgp-1024-small.txt data/experiment.txt output/output-bgp-1024-small.txt
mpisubmit.bg -n 256 -m VN -w 00:10:00 -t PREFER_TORUS ./pam settings/bgp-1024-medium.txt data/experiment.txt output/output-bgp-1024-medium.txt
mpisubmit.bg -n 256 -m VN -w 00:10:00 -t PREFER_TORUS ./pam settings/bgp-1024-big.txt data/experiment.txt output/output-bgp-1024-big.txt


mpisubmit.bg -n 16 -w 02:00:00 -t PREFER_TORUS
mpisubmit.bg -n 64 -w 02:00:00 -t PREFER_TORUS
mpisubmit.bg -n 128 -w 00:15:00 -t PREFER_TORUS
mpisubmit.bg -n 256 -w 00:10:00 -t PREFER_TORUS
mpisubmit.bg -n 512 -w 00:05:00 -t PREFER_TORUS
mpisubmit.bg -n 1024 -w 00:02:00 -t PREFER_TORUS
```

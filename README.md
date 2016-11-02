# pam-mpi-cmc

This repository contains **Partitioning Around Medoids (PAM)** algorithm written using MPI technology for supercomputers at Moscow State University (The faculty of Computational Mathematics and Cybernetics)

Considered supercomputers:

- Lomonosov
- Blue Gene/P (g++ -std=gnu++98)
- Regatta

In realization synchronized MPI Send/Recv are used.

## Repository structure

- `./main.cpp` - C++ file containing benchmark for PAM algorithm
- `./pam.h` and `./pam.cpp` - C++ PAM realization

- `./data/` - contains data to be processed clusterized
- `./joutput/` - contains results of PAM benchmark on *Blue Gene/P* supercomputer

    - `./generate_gnuplot.py` - generates `./*.dat` files for plotting them
    - `./gnuplot.script` - gnuplot script which generates plots representing *matrix multiplication performance*

- `./mpisubmit.sh` - script for starting tasks on Regatta supercomputer. This perl script was taken from BlueGene and adapted to Regatta.
- `./scripts/` - directory contains various scripts to upload to supercomputers files, compile them and run
- `./settings/` - directory contains sets of settings for PAM testing


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
- Efficiency (%) = GFlops for `p` processes and `n` sized multitude / GFlops for 1 process and `n` sized multitude


## supercomputer commands

### Blue Gene

```
mpisubmit.bg -n 1 -m dual -w 02:00:00 -t PREFER_TORUS ./pam settings/bgp-1proc.txt data/experiment.txt output/output-bgp-1proc.txt

mpisubmit.bg -n 256 -m dual -w 00:10:00 -t PREFER_TORUS ./pam settings/bgp-256-milli.txt data/experiment.txt output/output-bgp-256-milli.txt
mpisubmit.bg -n 256 -m dual -w 00:10:00 -t PREFER_TORUS ./pam settings/bgp-256-small.txt data/experiment.txt output/output-bgp-256-small.txt
mpisubmit.bg -n 256 -m dual -w 00:10:00 -t PREFER_TORUS ./pam settings/bgp-256-medium.txt data/experiment.txt output/output-bgp-256-medium.txt
mpisubmit.bg -n 256 -m dual -w 00:10:00 -t PREFER_TORUS ./pam settings/bgp-256-big.txt data/experiment.txt output/output-bgp-256-big.txt

mpisubmit.bg -n 512 -m dual -w 00:05:00 -t PREFER_TORUS ./pam settings/bgp-512-milli.txt data/experiment.txt output/output-bgp-512-milli.txt
mpisubmit.bg -n 512 -m dual -w 00:05:00 -t PREFER_TORUS ./pam settings/bgp-512-small.txt data/experiment.txt output/output-bgp-512-small.txt
mpisubmit.bg -n 512 -m dual -w 00:05:00 -t PREFER_TORUS ./pam settings/bgp-512-medium.txt data/experiment.txt output/output-bgp-512-medium.txt
mpisubmit.bg -n 512 -m dual -w 00:05:00 -t PREFER_TORUS ./pam settings/bgp-512-big.txt data/experiment.txt output/output-bgp-512-big.txt

mpisubmit.bg -n 1024 -m dual -w 00:02:00 -t PREFER_TORUS ./pam settings/bgp-1024-milli.txt data/experiment.txt output/output-bgp-1024-milli.txt
mpisubmit.bg -n 1024 -m dual -w 00:02:00 -t PREFER_TORUS ./pam settings/bgp-1024-small.txt data/experiment.txt output/output-bgp-1024-small.txt
mpisubmit.bg -n 1024 -m dual -w 00:02:00 -t PREFER_TORUS ./pam settings/bgp-1024-medium.txt data/experiment.txt output/output-bgp-1024-medium.txt
mpisubmit.bg -n 1024 -m dual -w 00:02:00 -t PREFER_TORUS ./pam settings/bgp-1024-big.txt data/experiment.txt output/output-bgp-1024-big.txt


mpisubmit.bg -n 16 -m dual -w 02:00:00 -t PREFER_TORUS
mpisubmit.bg -n 64 -m dual -w 02:00:00 -t PREFER_TORUS
mpisubmit.bg -n 128 -m dual -w 00:15:00 -t PREFER_TORUS
mpisubmit.bg -n 512 -m dual -w 00:05:00 -t PREFER_TORUS
```

### Lomonosov

Lomonosov execute tasks in context of `~/_scratch` directory.

```
sbatch -p regular4 -n 1024 impi ./pam settings/r4-set-1.txt input_data.txt output/r4-out-1.txt
sbatch -p regular4 -n 384 impi ./pam settings/r4-set-2.txt input_data.txt output/r4-out-2.txt
sbatch -p regular4 -n 1024 impi ./pam settings/r4-set-3.txt input_data.txt output/r4-out-3.txt

sbatch -p regular6 -n 16 impi ./pam ./settings/regular-set-1.txt input_data.txt ./output/r6-out-1.txt
sbatch -p regular6 -n 64 impi ./pam ./settings/regular-set-2.txt input_data.txt ./output/r6-out-2.txt
sbatch -p regular6 -n 512 impi ./pam ./settings/regular-set-3.txt input_data.txt ./output/r6-out-3.txt

sbatch -p smp -n 16 impi ./pam ./settings/smp-set-1.txt input_data.txt ./output/smp-out-1.txt
sbatch -p smp -n 64 impi ./pam ./settings/smp-set-2.txt input_data.txt ./output/smp-out-2.txt
sbatch -p smp -n 128 impi ./pam ./settings/smp-set-3.txt input_data.txt ./output/smp-out-3.txt

sbatch -p hdd4 -n 16 impi ./pam ./settings/hdd4-set-1.txt input_data.txt ./output/hdd4-out-1.txt
sbatch -p hdd4 -n 64 impi ./pam ./settings/hdd4-set-2.txt input_data.txt ./output/hdd4-out-2.txt
sbatch -p hdd4 -n 256 impi ./pam ./settings/hdd4-set-3.txt input_data.txt ./output/hdd4-out-3.txt

sbatch -p hdd6 -n 16 impi ./pam ./settings/hdd6-set-1.txt input_data.txt ./output/hdd6-out-1.txt
sbatch -p hdd6 -n 64 impi ./pam ./settings/hdd6-set-2.txt input_data.txt ./output/hdd6-out-2.txt
sbatch -p hdd6 -n 128 impi ./pam ./settings/hdd6-set-3.txt input_data.txt ./output/hdd6-out-3.txt

sbatch -p gpu -n 16 impi ./pam ./settings/gpu-set-1.txt input_data.txt ./output/gpu-out-1.txt
sbatch -p gpu -n 64 impi ./pam ./settings/gpu-set-2.txt input_data.txt ./output/gpu-out-2.txt
sbatch -p gpu -n 256 impi ./pam ./settings/gpu-set-3.txt input_data.txt ./output/gpu-out-3.txt
sbatch -p gpu -n 64 impi ./pam ./settings/gpu-set-4.txt input_data.txt ./output/gpu-out-4.txt
sbatch -p gpu -n 256 impi ./pam ./settings/gpu-set-5.txt input_data.txt ./output/gpu-out-5.txt
sbatch -p gpu -n 8 impi ./pam ./settings/gpu-set-6.txt input_data.txt ./output/gpu-out-6.txt
sbatch -p gpu -n 192 impi ./pam ./settings/gpu-set-7.txt input_data.txt ./output/gpu-out-7.txt
```

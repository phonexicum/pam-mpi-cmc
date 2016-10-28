# pam-mpi-lomonosov

This repository contains **Partitioning Around Medoids (PAM)** algorithm written using MPI technology for Lomonosov supercomputer at Moscow State University (The faculty of Computational Mathematics and Cybernetics)


**Partitioning Around Medoids (PAM)** algorithm is clusterization algorithm.

PAM algorithm description can be found on [Algowiki project (russian only)](https://algowiki-project.org/ru/Partitioning_Around_Medoids_(PAM))

PAM algorithm must not be mixed with *k-medoids* algorithm. PAM is based on two phases: *BUILD* and *SWAP*, - each iteration of SWAP phase search for some element in a given set of elements to replace one of already selected *medoids* (cluster centers) to minimize objective function.

# Experiments conditions

- matrix order: 1500, 2500, 3500, 4500, 5500
- processes number: 1, 25, 50, 75, 100, ... 300
- vector size (for each element) = 10 double numbers

metrics:

- Gflops (FLoating-point Operations Per Second) = amount of operations / algorithm execution time
- Efficiency (%) = 

For experiments I need to generate random numbers in amount of = (n=5500)*(m=10) = 55 000 random numbers

# Temporary notes

home directory is not seen from the processes
to pass information to your programm put files into:
`~/_scratch`


```
sbatch -p regular4 -n 1024 impi ./pam settings/r4-set-1.txt input_data.txt ./output/r4-out-1.txt
sbatch -p regular4 -n 384 impi ./pam ./settings/r4-set-2.txt input_data.txt ./output/r4-out-2.txt
sbatch -p regular4 -n 1024 impi ./pam ./settings/r4-set-3.txt input_data.txt ./output/r4-out-3.txt

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

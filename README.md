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

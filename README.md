# pam-mpi-lomonosov

This repository contains **Partitioning Around Medoids (PAM)** algorithm written using MPI technology for Lomonosov supercomputer at Moscow State University (The faculty of Computational Mathematics and Cybernetics)


**Partitioning Around Medoids (PAM)** algorithm is clusterization algorithm.

PAM algorithm description can be found on [Algowiki project (russian only)](https://algowiki-project.org/ru/Partitioning_Around_Medoids_(PAM))

PAM algorithm must not be mixed with *k-medoids* algorithm. PAM is based on two phases: *BUILD* and *SWAP*, - each iteration of SWAP phase search for some element in a given set of elements to replace one of already selected *medoids* (cluster centers) to minimize objective function.

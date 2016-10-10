#! /bin/bash

# Entering compiler host
# ssh compiler

module add slurm

# module available
module add openmpi/1.5.5-gcc

mpicxx pam.cpp -o pam

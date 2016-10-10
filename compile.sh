#! /bin/bash

# Entering compiler host
# ssh compiler

module add slurm

# module available
module add impi/5.0.1

mpicxx pam.cpp -o pam
cp ./pam ~/_scratch/pam

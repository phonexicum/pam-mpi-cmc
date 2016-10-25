#! /bin/bash

# Computational nodes can access ONLY ~/_scratch/ directory

module add slurm
module add impi/5.0.1

cd ~/_scratch/

# Starting program
# 
# ./binary n m k input_data.txt output_data.txt t
# 
sbatch -p test -n 20 impi ./pam 4 2 2 test_data.txt output.txt 0
# sbatch -p test -n 2 impi ./pam 10 1 2 input_data.txt output.txt 0

# Looking into queue
squeue -p test

# Looking queue info
# sinfo TASK_ID

# Cancelling task
# scancel TASK_ID

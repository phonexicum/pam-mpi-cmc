#! /bin/bash

module add slurm
module add impi/5.0.1

cd ~/_scratch/

# Starting program
sbatch -p test -n 8 impi ./pam

# Looking into queue
squeue -p test

# Looking queue info
# sinfo TASK_ID

# Cancelling task
# scancel TASK_ID

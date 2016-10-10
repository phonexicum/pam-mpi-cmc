#! /bin/bash

module add slurm
module add openmpi/1.5.5-gcc

# Starting program
sbatch -p test -n 8 ompi ./pam

# Looking into queue
squeue -p test

# Looking queue info
# sinfo TASK_ID

# Cancelling task
# scancel TASK_ID

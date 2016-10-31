#! /bin/bash

# Computational nodes can access ONLY ~/_scratch/ directory

module add slurm
module add impi/5.0.1

cd ~/_scratch/

sbatch -p test -n 16 impi ./pam settings/settings.txt data/experiment.txt output/output.txt

# squeue -p test
# sinfo TASK_ID
# scancel TASK_ID

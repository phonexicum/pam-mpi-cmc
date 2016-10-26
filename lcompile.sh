#! /bin/bash

# Entering compiler host
# ssh compiler

module add slurm

# module available
module add impi/5.0.1

mpicxx main.cpp pam.cpp pam.h -o pam -std=c++0x -Wall

cp ./pam ~/_scratch/pam
cp ./test_data.txt ~/_scratch/ 
cp ./input_data.txt ~/_scratch/ 
cp -R ./settings* ~/_scratch/ 

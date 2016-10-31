#! /bin/bash

module add slurm

module add impi/5.0.1

mpicxx main.cpp pam.cpp pam.h -o pam -std=c++0x -Wall

cp ./pam ~/_scratch/pam
cp -R ./data ~/_scratch/
cp -R ./settings* ~/_scratch/

mkdir -p ~/_scratch/output

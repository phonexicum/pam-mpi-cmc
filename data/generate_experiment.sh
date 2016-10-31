#! /bin/bash

awk -v "seed=$[(RANDOM & 32767) + 32768 * (RANDOM & 32767)]" 'BEGIN { srand(seed); for ($i=0; $i< 100000 ; $i=$i+1) { printf("%.7f ", rand() * 100.0) } }' >>experiment.txt

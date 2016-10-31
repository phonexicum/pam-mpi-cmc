#! /bin/bash

mkdir -p ../mount/
sshfs -o nonempty edu-cmc-stud16-621-02@bluegene.hpc.cs.msu.ru:/home/edu-cmc-stud16-621-02 ../mount/

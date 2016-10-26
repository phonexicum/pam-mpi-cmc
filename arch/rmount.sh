#! /bin/bash

mkdir -p ./sc-mount/
sshfs -o nonempty edu-cmc-stud16-621-02@regatta.cs.msu.su:/home/edu-cmc-stud16-621-02 ./sc-mount/

#! /bin/bash

mkdir -p ./sc-mount/
sshfs -o nonempty avasilenko2_1854@lomonosov.parallel.ru:/mnt/data/users/dm4/vol12/avasilenko2_1854 ./sc-mount/

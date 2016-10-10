#! /bin/bash

mkdir -p ./lom-mount/
sshfs -o nonempty avasilenko2_1854@lomonosov.parallel.ru:/mnt/data/users/dm4/vol12/avasilenko2_1854 ./lom-mount/

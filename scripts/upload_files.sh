#! /bin/bash

cp lcompile.sh ../mount/
cp lrun.sh ../mount/

cp jcompile.sh ../mount/
cp rcompile.sh ../mount/
cp run.jcf ../mount/

cd ../

cp mpisubmit.sh ./mount/

cp main.cpp ./mount/
cp pam.cpp ./mount/
cp pam.h ./mount/

cp -R settings* ./mount/
cp -R data ./mount/
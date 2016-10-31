#! /bin/bash

mpicxx main.cpp pam.cpp pam.h -o pam -std=gnu++98 -Wall

mkdir -p ~/output

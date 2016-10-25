CC=mpicxx
CFLAGS=-Wall -std=c++11

pam: main.cpp pam.cpp pam.h
	$(CC) -o pam main.cpp pam.cpp pam.h $(CFLAGS)

.PHONY: clean

run:
	mpirun -np 2 ./pam 11 2 3 test_data.txt output.txt 0
# ./binary n m k input_data.txt output_data.txt t

clean:
	rm -f pam output.txt

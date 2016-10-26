CC=mpicxx
CFLAGS=-Wall -std=c++11

pam: main.cpp pam.cpp pam.h
	$(CC) -o pam main.cpp pam.cpp pam.h $(CFLAGS)

.PHONY: clean

run:
	# mpirun -np 4 ./pam 11 2 3 input_data.txt output.txt 0
# ./binary n m k input_data.txt output_data.txt t
	
	mpirun -np 4 ./pam settings.txt input_data.txt output.txt

clean:
	rm -f pam output.txt working.txt

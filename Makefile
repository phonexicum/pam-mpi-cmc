CC=mpicxx
CFLAGS=-Wall

pam: pam.cpp
	$(CC) -o pam pam.cpp $(CFLAGS)

.PHONY: clean

run:
	mpirun -np 2 ./pam

clean:
	rm -f pam output.txt

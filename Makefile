CC=mpicxx
CFLAGS=-Wall -std=c++11

pam: main.cpp pam.cpp pam.h
	$(CC) -o pam main.cpp pam.cpp pam.h $(CFLAGS)

.PHONY: clean

run:
	mpirun -np 4 ./pam settings.txt experiment.txt output.txt

clean:
	rm -f pam output.txt working.txt

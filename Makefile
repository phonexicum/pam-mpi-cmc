pam: main.cpp pam.cpp pam.h
	mpicxx -o pam main.cpp pam.cpp pam.h -Wall -std=c++11

.PHONY: clean

run:
	mpirun -np 4 ./pam settings/settings.txt data/experiment.txt output.txt

clean:
	rm -f pam output.txt working.txt



jmount:
	mkdir -p ./mount/
	sshfs -o nonempty edu-cmc-stud16-621-02@bluegene.hpc.cs.msu.ru:/home/edu-cmc-stud16-621-02/PAM ./mount/

lmount:
	mkdir -p ./mount/
	sshfs -o nonempty avasilenko2_1854@lomonosov.parallel.ru:/mnt/data/users/dm4/vol12/avasilenko2_1854 ./mount/



jcompile:
	mpicxx main.cpp pam.cpp pam.h -o pam -std=gnu++98 -Wall
	mkdir -p ./output

lcompile:
	mpicxx main.cpp pam.cpp pam.h -o pam -std=c++0x -Wall
	cp ./pam ~/_scratch/pam
	cp -R ./data ~/_scratch/
	cp -R ./settings* ~/_scratch/
	mkdir -p ~/_scratch/output



upload:
	cp Makefile ./mount/
	cp main.cpp ./mount/
	cp pam.cpp ./mount/
	cp pam.h ./mount/

	cp -R settings* ./mount/
	cp -R data ./mount/

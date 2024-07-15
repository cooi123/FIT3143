ALL: helloworld

helloworld: helloworld.c
	mpicc -o helloworld helloworld.c
run: 
	mpirun -np 4 helloworld


clean:
	/bin/rm -f helloworld *.o

# The makefile for MP1.

lab1 : atom_list.c lab1.c
	gcc -Wall -g atom_list.c lab1.c -o lab1



test: lab1 
	./lab1 10 < input1.txt
clean :
	rm -f *.o lab1 core a.out


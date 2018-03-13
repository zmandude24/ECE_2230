# makefile for MP4
#
# -lm is used to link in the math library
#
# -Wall turns on all warning messages 
#
comp = gcc
comp_flags = -g -Wall -std=gnu99
comp_libs = -lm  

lab4 : list.o mem.o lab4.o
	$(comp) $(comp_flags) list.o mem.o lab4.o -o lab4 $(comp_libs)

list.o : list.c datatypes.h list.h
	$(comp) $(comp_flags) -c list.c

mem.o : mem.c mem.h
	$(comp) $(comp_flags) -c mem.c

lab4.o : lab4.c datatypes.h list.h mem.h
	$(comp) $(comp_flags) -c lab4.c

clean :
	rm -f *.o lab4 core


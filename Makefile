all : main.o blowfish.o
	gcc main.o b.o -Wall -o blowfish
main.o : main.c blowfish.h
	gcc main.c -c
blowfish.o : blowfish.c blowfish.h
	gcc blowfish.c -c

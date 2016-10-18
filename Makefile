all : main.o b.o
	gcc main.o b.o -Wall -o blowfish
main.o : main.c blowfish.h
	gcc main.c -c
b.o : b.c blowfish.h
	gcc b.c -c

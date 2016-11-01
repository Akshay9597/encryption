all : main.o blowfish.o
	gcc main.o blowfish.o -Wall -o project
main.o : main.c blowfish.h
	gcc main.c -c
blowfish.o : blowfish.c blowfish.h
	gcc blowfish.c -c
clear : *.txt
	rm -rv *.txt
clearenc : *.enc
	rm -rv *.enc
cleardec : *.dec
	rm -rv *.dec

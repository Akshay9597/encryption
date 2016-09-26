/*I want arguments from user as ./enc [KEY] [source file] [target file] or for ./dec........ */
/*Read 16 bytes and divide into 2 halves*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "blowfish.h"
int main(int argc, char *argv) {
/*	unsigned long L =74, R = 10;
	BLOWFISH_CTX ctx;
	Blowfish_Init(&ctx, (unsigned char *) "TESTKEY", 7);
	Blowfish_Encrypt(&ctx, &L, &R);
	printf("%08lX %08lX\n", L, R);
	Blowfish_Decrypt(&ctx, &L, &R);
	printf("%lu %lu\n", L, R);*/
	int sfile, tfile, keylen;
	char key[56], data[16], l[8], r[8];
	strcpy(key, argv[1]);	
	keylen = strlen(key);
	if(keylen < 4 || keylen > 56) {			/*Argv[1] is my key length 32-448 bits*/
		perror("Key should be greater than 4 bytes and less than 56 bytes: ");
		return -1;
	}
	if(argc != 4) {
		errno = EINVAL;
		perror("bad arguments: ");
		return errno;
	}
	sfile = open (argv[2], O_RDONLY);
	tfile = open (argv[3], O_WRONLY | O_CREAT);
	if(sfile == -1 || tfile == -1) {
		perror("Opened failed : ");
		return errno;
	}
	BLOWFISH_CTX ctx;
	Blowfish_Init(&ctx, key, keylen);
	read(sfile, data, 16);
	for(i = 0; i < 8; i++) {		//Dividing into 8 bytes array
		l[i] = data[i];
		r[i] = data[i + 8];
	}
	return 0;
}

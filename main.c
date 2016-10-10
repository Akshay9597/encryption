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
//#include "enc.c"
//#include "dec.c"
int main(int argc, char *argv[]) {
/*	unsigned long L =74, R = 10;
	BLOWFISH_CTX ctx;
	Blowfish_Init(&ctx, (unsigned char *) "TESTKEY", 7);
	Blowfish_Encrypt(&ctx, &L, &R);
	printf("%08lX %08lX\n", L, R);
	Blowfish_Decrypt(&ctx, &L, &R);
	printf("%lu %lu\n", L, R);*/
	unsigned int keylen, datalen;
	FILE *sfile, *tfile;
	unsigned int retstrlen;
	unsigned char key[56], data[17], l[8], r[8];
	short i;
	strcpy(key, argv[3]);	
	keylen = strlen(key);
	if(keylen < 4 || keylen > 56) {			/*Argv[1] is my key length 32-448 bits*/
		perror("Key should be greater than 4 bytes and less than 56 bytes: ");
		return -1;
	}
	if(argc != 6 ) {
		errno = EINVAL;
		perror("./a.out enc/dec <key> <source file> <target file>");
		return errno;
	}
	sfile = fopen(argv[4], "r");
	tfile = fopen(argv[5], "w");
	if(sfile == NULL || tfile == NULL) {
		perror("Opened failed : ");
		return errno;
	}
	BLOWFISH_CTX ctx;
	Blowfish_Init(&ctx, key, keylen);
	while(!feof(sfile)) {		//Main loop
		retstrlen = fread(data, sizeof(char), 16, sfile);		
		//I have to do padding if datalen != 16... Datalen is != 16 if it reaches at the end.
		for(i = 0; i < 8; i++) {		//Dividing into 8 bytes array. 2 halves
			l[i] = data[i];
			r[i] = data[i + 8];
		}
		if(!strcmp(argv[2], "enc"))
			Blowfish_Encrypt(&ctx, l, r);
		else
			Blowfish_Decrypt(&ctx, l, r);
		//fputs() writes data without '\0'
		for(i = 0; i < 8; i++)
			data[i] = l[i];
		for(i = 0; i < 8; i++)
			data[i + 8] = r[i];
		data[16] = '\0';
		fputs(data, tfile);
	}
	return 0;
}

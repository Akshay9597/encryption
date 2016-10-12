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
#include "b1.h"
//#include "enc.c"
//#include "dec.c"
int main(int argc, char *argv[]) {
	unsigned long L, R;
	BLOWFISH_CTX ctx;
	Blowfish_Init(&ctx, (unsigned char *) "TESTKEY", 7);
	char x, y;
	char l[1], r[1];
	FILE *sfile, *tfile;
	sfile = fopen("info.txt", "r");
	tfile = fopen("output.txt", "w");
	while(!feof(sfile)) {
		fread(l, sizeof(char), 1, sfile);
		fread(r, sizeof(char), 1, sfile);
		L = l[0];
		R = r[0];
		Blowfish_Encrypt(&ctx, &L, &R);
	//	printf("%lu%lu", L, R);
		fprintf(tfile, "%lu%lu", L, R );
		fscanf(tfile, "%lu%lu", &L, &R);
/*		x[i] = L;
		y[i] = R;
	}
//	printf("%08lX %08lX\n", L, R);
	for(i = 0; i < 5; i++) {
		L = x[i];
		R = y[i];*/
		Blowfish_Decrypt(&ctx, &L, &R);
		x = L;
		y = R;
		printf("%c%c", x, y);
	}
/*	unsigned int datalen;
	int keylen;
	FILE *sfile, *tfile;
	unsigned long l, r;
	unsigned char key[56], data[3];
	short i;
	strcpy(key, argv[2]);	
	keylen = strlen(key);
	printf("%d", keylen);
	if(keylen < 4 || keylen > 56) {			//Argv[1] is my key length 32-448 bits
		perror("Key should be greater than 4 bytes and less than 56 bytes: ");
		return -1;
	}
	if(argc != 5 ) {
		errno = EINVAL;
		perror("./a.out enc/dec <key> <source file> <target file>");
		return errno;
	}
	sfile = fopen(argv[3], "r");
	tfile = fopen(argv[4], "w");
	if(sfile == NULL || tfile == NULL) {
		perror("Opened failed : ");
		return errno;
	}
	BLOWFISH_CTX ctx;
	Blowfish_Init(&ctx, key, keylen);
	while(!feof(sfile)) {		//Main loop
		fread(data, sizeof(char), 2, sfile);		
		//I have to do padding if datalen != 16... Datalen is != 16 if it reaches at the end.
			l = data[0];
			r = data[1];
		if(!strcmp(argv[1], "enc"))
			Blowfish_Encrypt(&ctx, &l, &r);
		else
			Blowfish_Decrypt(&ctx, &l, &r);
		//fputs() writes data without '\0'
		data[0] = l;
		data[1] = r;
		data[2] = '\0';
		fputs(data, tfile);
	}*/
	return 0;
}

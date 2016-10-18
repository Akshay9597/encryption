/*Read 16 bytes and divide into 2 halves*/
/*Make key strong... Try to type password again if it is not strong .... i.e check for Capitals and symbols*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "blowfish.h"
int main(int argc, char *argv[]) {
	/*-----------HELP----------------*/
	if(argc == 2) {
		if(!strcmp(argv[1], "-h")) {
			printf("enc : ./blowfish enc <key> <source file> <target file>\n");
			printf("dec : ./blowfish dec <key> <source file> <target file>");
			return 0;
		}
	}
	/*-----------HELP----------------*/
	unsigned int datalen;
	int keylen, flag = 0;
	char k;
	FILE *sfile, *tfile;
	unsigned int retstrlen;
	unsigned char key[56], data[17], l[8], r[8];
	short i = -1;
	if(argc != 5 ) {
		errno = EINVAL;
		perror("./blowfish enc/dec <key> <source file> <target file> : ");
		return errno;
	}
	strcpy(key, argv[2]);
	keylen = strlen(key);
	/*******Strong key*********/
	if(!strcmp(argv[1], "enc")) {
		while(++i < keylen) {
			if(key[i] < 97 || key[i] > 122) {
				flag = 1;
				break;
			}
		}
		if(flag == 0) {
			printf("Key is not strong enough.\nDo you wish to continue y/n\n");
			k = getchar();
			if(k == 'n')
				exit(1);
/*			if(k == 'y')
				scanf("%s", key);*/
		}
	}
	/*******Strong key*********/
	keylen = strlen(key);
	if(keylen < 4 || keylen > 56) {			/*Argv[1] is my key length 32-448 bits*/
		perror("Key should be greater than 4 bytes and less than 56 bytes: ");
		return -1;
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
		retstrlen = fread(data, sizeof(char), 16, sfile);		
		//I have to do padding if datalen != 16... Datalen is != 16 if it reaches at the end.
		//printf("\n%c\t", data[retstrlen - 1]);
		if(retstrlen != 16) {
			for(i = retstrlen - 1; i < 16; i++) {
				data[i] = ' ';
			}
		//	data[16] = '\n';
		}
		//For the last 16 bytes if it reaches end it is taking '\n'... But we don't want.
		//printf("\n");
	/*	for(i = 0; i < 16; i++) {
			printf("%c", data[i]);
		}*/
		for(i = 0; i < 8; i++) {		//Dividing into 8 bytes array. 2 halves
			l[i] = data[i];
			r[i] = data[i + 8];
		}
		if(!strcmp(argv[1], "enc"))
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
	fclose(sfile);
	fclose(tfile);
	if(!strcmp(argv[1], "enc"))
		printf("Your data is successfully encrypted in %s\n", argv[4]);
	else
		printf("Your data is successfully decrypted in %s\n", argv[4]);
	return 0;
}

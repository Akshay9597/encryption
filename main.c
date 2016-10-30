/*Read 16 bytes and divide into 2 halves*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
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
	int keylen, flag = 0;
	char k, tmp;
	FILE *sfile, *tfile;
	unsigned int retstrlen, retstrlen1;
	unsigned char key[56];
	unsigned long l , r;
	unsigned char *cr, *cl;
	short i = -1;
//	short count1, count2;
	if(argc != 5 ) {
		errno = EINVAL;
		perror("./blowfish enc/dec <key> <source file> <target file> : ");
		return errno;
	}
	strcpy(key, argv[2]);
	keylen = strlen(key);
	/*******Strong key*********/
	while((!strcmp(argv[1], "enc")) && (flag == 0)) {
		i = 0;
		while(++i < keylen) {
			if(key[i] < 97 || key[i] > 122) {
				flag = 1;
				break;
			}
		}
		if(flag == 0) {
			printf("Key is not strong enough.\nDo you wish to continue y/n\n");
			k = getchar();
			tmp = getchar();
			if(k == 'n')
				scanf("%s", key);
			else if(k == 'y')
				break;
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
	//Even if retstrlen and retstrlen1 is somentimes 0 it contains data
	while(!feof(sfile)) {		//Main loop
		l = r = 0X00000000;
//		count1 = count2 = 0;
		retstrlen1 = fread(&l, sizeof(unsigned long), 1, sfile);
		retstrlen = fread(&r, sizeof(unsigned long), 1, sfile);
		i = 0;
//		cl = &l;
//		cr = &r;
/*		while(i++ < 8) {
			if(cl[i - 1])
				count1++;
		}
		i = 0;
		while(i++ < 8) {
			if(cl[i - 1])
				count2++;
		}*/
	//	printf("\n%d %d", retstrlen1, retstrlen);
	//	if(retstrlen != 1)
	//		r = 0X00000000;
		if(!strcmp(argv[1], "enc"))
			Blowfish_Encrypt(&ctx, &l, &r);
		else
			Blowfish_Decrypt(&ctx, &l, &r);
		//printf("%lu ", r);
		cl = &l;
		cr = &r;
		for(i = 0; i < 8; i++) {
//			if(cl[i])
				fprintf(tfile, "%c", cl[i]);
		}
//		if(r == 0X00000000)	//Padding part
//			break;
		for(i = 0; i < 8; i++) {
//			if(cr[i])
				fprintf(tfile, "%c", cr[i]);
		}
	}
	fclose(sfile);
	fclose(tfile);
	if(!strcmp(argv[1], "enc"))
		printf("Your data is successfully encrypted in %s\n", argv[4]);
	else
		printf("Your data is successfully decrypted in %s\n", argv[4]);
	return 0;
}

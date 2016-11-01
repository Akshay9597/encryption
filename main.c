/*Read 16 bytes and divide into 2 halves*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "blowfish.h"
/*
#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||";
#define PBWIDTH 60

void printprogress(double percentage) {
	int val = (int) (percentage * 100);
	int lpad = (int) (percentage * PBWIDTH);
	int rpad = PBWIDTH - lpad;
	printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
	fflush(stdout);
}
*/
int main(int argc, char *argv[]) {

	/*-----------HELP----------------*/
	if(argc == 2) {
		if(!strcmp(argv[1], "-h")) {
			printf("enc : ./project enc <key> <source file1> <target file1> <source file2> <target file2>........\n");
			printf("dec : ./project dec <key> <source file1> <target file1> <source file2> <target file2>........\n");
			printf("-c  : Creates it's own outputfile. Extensions .enc or .dec will be added respectively. \n");
			printf("Usage -c: ./project enc/dec <key> <source file1> <source file2>....... -c\n");
			printf("-cd : Create output file and delete source files.\n");
			printf("Usage -cd : ./project enc/dec <key> <source file1> <source file2>....... -cd\n");
			printf("-d : Delete source files.\n");
			printf("Usage -d : ./project enc/dec <key> <source file1> <target file1> <source file2> <target file2>........ -d\n");
			return 0;
		}
	}
	/*-----------HELP----------------*/
	int keylen, flag = 0;
	char k, tmp;
	char filename[50];
/*	if(!strcmp(argv[argc - 1], "-c"))
		FILE *file[argc - 4]; //If option is selected like -c
	else
		FILE *file[argc - 3]
	if(!strcmp(argv[argc - 1], "-c"))
		FILE *target[argc - 4];*/
	int fileindex = 0, filecount, delfile = 1;
	unsigned int retstrlen, retstrlen1;
	unsigned char key[56];
	unsigned long l, r, filelen;
	unsigned char *cr, *cl;
	short i = -1;
	unsigned long percent;
	unsigned long count;
//      short count1, count2;
	if(argc < 5) {
		errno = EINVAL;
		perror("./project enc/dec <key> <source file1> <target file1> <source file1> <target file2>....... [OPTIONS]\nTry ./blowfish -h :");
		return errno;
	}
	strcpy(key, argv[2]);
	keylen = strlen(key);
	/************************Strong key*********************/
	while((!strcmp(argv[1], "enc")) && (flag == 0)) {
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
	keylen = strlen(key);
	if(keylen < 4 || keylen > 56) {	/*Argv[1] is my key length 32-448 bits */
		perror("Key should be greater than 4 bytes and less than 56 bytes : ");
		return -1;
	}
	/**********************Strong key***********************/
	/**********************[OPTION -c]*************************/
	if((!strcmp(argv[argc - 1], "-c")) || (!strcmp(argv[argc - 1], "-cd"))) {
		FILE *file[argc - 4];
		FILE *target[argc - 4];
		filecount = argc - 4;
		for(; fileindex < filecount; fileindex++) {
			count = 0;
			strcpy(filename, argv[fileindex + 3]);
			file[fileindex] = fopen(argv[fileindex + 3], "r");
			if(file[fileindex] == NULL) {
				perror("Opened failed : ");
				return errno;
			}
			BLOWFISH_CTX ctx;
			Blowfish_Init(&ctx, key, keylen);
			//Even if retstrlen and retstrlen1 is somentimes 0 it contains data
			percent = 0;
			if((!strcmp(argv[1], "enc"))) {
				strcat(filename, ".enc");
				target[fileindex] = fopen(filename, "w");
				if(target[fileindex] == NULL) {
					perror("Opened failed : ");
					return errno;
				}
				fseek(file[fileindex], 0L, SEEK_END);
				filelen = ftell(file[fileindex]);
				rewind(file[fileindex]);
				filelen = filelen + keylen;	//Don't want to store bytes directly
				fprintf(target[fileindex], "%lu", filelen);
				while(!feof(file[fileindex])) {
					l = r = 0X00000000;
					retstrlen1 = fread(&l, sizeof(unsigned long), 1, file[fileindex]);
					retstrlen = fread(&r, sizeof(unsigned long), 1, file[fileindex]);
					i = 0;
					//      printf("\n%d %d", retstrlen1, retstrlen);
					Blowfish_Encrypt(&ctx, &l, &r);
					cl = &l;
					cr = &r;
					for(i = 0; i < 8; i++) {
						fprintf(target[fileindex], "%c", cl[i]);
					}
					for(i = 0; i < 8; i++) {
						fprintf(target[fileindex], "%c", cr[i]);
					}
					printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 9)));
					fflush(stdout);
				}
			} 
			else if(!strcmp(argv[1], "dec")) {
				strcat(filename, ".dec");
				target[fileindex] = fopen(filename, "w");
				if(target[fileindex] == NULL) {
					perror("Opened failed : ");
					return errno;
				}
				fscanf(file[fileindex], "%lu", &filelen);
				filelen = filelen - keylen;
				while(!feof(file[fileindex])) {
					l = r = 0X00000000;
					retstrlen1 = fread(&l, sizeof(unsigned long), 1, file[fileindex]);
					retstrlen = fread(&r, sizeof(unsigned long), 1, file[fileindex]);
					i = 0;
					//      printf("\n%d %d", retstrlen1, retstrlen);
					Blowfish_Decrypt(&ctx, &l, &r);
					cl = &l;
					cr = &r;
					for(i = 0; i < 8; i++) {
						if(count > filelen - 1)
							break;
						fprintf(target[fileindex], "%c", cl[i]);
						count++;
					}
					for(i = 0; i < 8; i++) {
						if(count > filelen - 1)
							break;
						fprintf(target[fileindex], "%c", cr[i]);
						count++;
					}
					printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 9)));
					fflush(stdout);
				}
			} 
			else {
				printf("Wrong arguments\n");
				exit(1);
			}
			fclose(file[fileindex]);
			fclose(target[fileindex]);
			printf("\n");
			if(!strcmp(argv[1], "enc"))
				printf("Your data is successfully encrypted in %s\n", filename);
			else
				printf("Your data is successfully decrypted in %s\n", filename);
			if(!strcmp(argv[argc - 1], "-cd"))
				delfile = remove(argv[fileindex + 3]);
			if(!delfile)
				printf("File %s deleted\n", argv[fileindex + 3]);
		}
		return 0;
	}
/*******************************[OPTION -c] [OPTION -cd]*********************************/
/*******************************NO OPTION [OPTION -d]*********************************/
	else {
		FILE *file[argc - 3];
		if(!strcmp(argv[argc - 1], "-d"))
			filecount = argc - 4;
		else
			filecount = argc - 3;
		for(; fileindex < filecount; fileindex += 2) {
			count = 0;
			file[fileindex] = fopen(argv[fileindex + 3], "r");
			file[fileindex + 1] = fopen(argv[fileindex + 4], "w");
			if(file[fileindex] == NULL || file[fileindex + 1] == NULL) {
				perror("Opened failed : ");
				return errno;
			}
			BLOWFISH_CTX ctx;
			Blowfish_Init(&ctx, key, keylen);
			//Even if retstrlen and retstrlen1 is somentimes 0 it contains data
			percent = 0;
			if((!strcmp(argv[1], "enc"))) {
				fseek(file[fileindex], 0L, SEEK_END);
				filelen = ftell(file[fileindex]);
				rewind(file[fileindex]);
				filelen = filelen + keylen;	//Don't want to store bytes directly
				fprintf(file[fileindex + 1], "%lu", filelen);
				while(!feof(file[fileindex])) {
					l = r = 0X00000000;
					retstrlen1 = fread(&l, sizeof(unsigned long), 1, file[fileindex]);
					retstrlen = fread(&r, sizeof(unsigned long), 1, file[fileindex]);
					i = 0;
					//      printf("\n%d %d", retstrlen1, retstrlen);
					Blowfish_Encrypt(&ctx, &l, &r);
					cl = &l;
					cr = &r;
					for(i = 0; i < 8; i++) {
						fprintf(file[fileindex + 1], "%c", cl[i]);
					}
					for(i = 0; i < 8; i++) {
						fprintf(file[fileindex + 1], "%c", cr[i]);
					}
					printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 9)));
					fflush(stdout);
				}
			} 
			else if(!strcmp(argv[1], "dec")) {
				fscanf(file[fileindex], "%lu", &filelen);
				filelen = filelen - keylen;
				while(!feof(file[fileindex])) {
					l = r = 0X00000000;
					retstrlen1 = fread(&l, sizeof(unsigned long), 1, file[fileindex]);
					retstrlen = fread(&r, sizeof(unsigned long), 1, file[fileindex]);
					i = 0;
					//      printf("\n%d %d", retstrlen1, retstrlen);
					Blowfish_Decrypt(&ctx, &l, &r);
					cl = &l;
					cr = &r;
					for(i = 0; i < 8; i++) {
						if(count > filelen - 1)
							break;
						fprintf(file[fileindex + 1], "%c", cl[i]);
						count++;
					}
					for(i = 0; i < 8; i++) {
						if(count > filelen - 1)
							break;
						fprintf(file[fileindex + 1], "%c", cr[i]);
						count++;
					}
					printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 9)));
					fflush(stdout);
				}
			} 
			else {
				printf("Wrong arguments\n");
				exit(1);
			}
			fclose(file[fileindex]);
			fclose(file[fileindex + 1]);
			printf("\n");
			if(!strcmp(argv[1], "enc"))
				printf("Your data is successfully encrypted in %s\n", argv[fileindex + 4]);
			else
				printf("Your data is successfully decrypted in %s\n", argv[fileindex + 4]);
			if(!strcmp(argv[argc - 1], "-d"))
				delfile = remove(argv[fileindex + 3]);
			if(!delfile)
				printf("File %s deleted", argv[fileindex + 3]);
		}
		return 0;
	}
}

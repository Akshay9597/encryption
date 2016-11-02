/*
 *  Blowfish Implementation
 *
 *  Copyright (C) 2016 Akshay Abhyankar
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/*Read 16 bytes and divide into 2 halves*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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
			printf("enc : ./project enc <key> <source file1> <target file1> <source file2> <target file2>........[OPTIONS]\n");
			printf("dec : ./project dec <key> <source file1> <target file1> <source file2> <target file2>........[OPTIONS]\n");
			printf("OPTIONS : \n");
			printf("-c  : Creates it's own outputfile. Extensions .enc or .dec will be added respectively. \n");
			printf("Usage -c: ./project enc/dec <key> <source file1> <source file2>....... -c\n");
			printf("-cd : Create output file and delete source files.\n");
			printf("Usage -cd : ./project enc/dec <key> <source file1> <source file2>....... -cd\n");
			printf("-d : Delete source files.\n");
			printf("Usage -d : ./project enc/dec <key> <source file1> <target file1> <source file2> <target file2>........ -d\n");
			printf("-cD : Encrypts all files in a DIRECTORY and creates and stores in a new directory.\n");
			printf("Usage -cD : ./project enc/dec <key> <Directory> .........-cD\n");
			printf("-cdD : Encrypts all files in a DIRECTORY and creates and stores in a new directory and deletes source directory.\n");
			printf("Usage -cdD : ./project enc/dec <key> <Directory>............. -cdD\n");
			printf("-dD : Encrypts all files in a DIRECTORY and deletes source directory.\n");
			printf("Usage -dD : ./project enc/dec <key> <source Directory1> <target Directory1> <source Directory2> <target directory2>............. -dD\n");
			printf("-D : Encrypts all files in a DIRECTORY.\n");
			printf("Usage -D : ./project enc/dec <key> <source Directory1> <target Directory1> <source Directory2> <target directory2>............. -D\n");
			return 0;
		}
	}
	/*-----------HELP----------------*/
	int keylen, flag = 0;
	char k, tmp;
	char filename[50], dirname[50], filenametmp[50], sdir[50];
	int fileindex = 0, filecount, dirindex = 0, dircount, delfile;
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
		perror("./project enc/dec <key> <source file1> <target file1> <source file1> <target file2>....... [OPTIONS]\nTry ./project -h :");
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
	/**********************Strong key***************************************/
	/**********************[OPTION -c] [OPTION -cd]*************************/
	if((!strcmp(argv[argc - 1], "-c")) || (!strcmp(argv[argc - 1], "-cd"))) {
		FILE *fp1;
		FILE *fp2;
		filecount = argc - 4;
		for(; fileindex < filecount; fileindex++) {
			count = 0;
			delfile = 1;
			strcpy(filename, argv[fileindex + 3]);
			fp1 = fopen(argv[fileindex + 3], "r");
			if(fp1 == NULL) {
				perror("Opened failed : ");
				return errno;
			}
			BLOWFISH_CTX ctx;
			Blowfish_Init(&ctx, key, keylen);
			//Even if retstrlen and retstrlen1 is somentimes 0 it contains data
			percent = 0;
			if((!strcmp(argv[1], "enc"))) {
				strcat(filename, ".enc");
				fp2 = fopen(filename, "w");
				if(fp2 == NULL) {
					perror("Opened failed : ");
					return errno;
				}
				fseek(fp1 , 0L, SEEK_END);
				filelen = ftell(fp1);
				rewind(fp1);
				filelen = filelen + keylen;	//Don't want to store bytes directly
				fprintf(fp2, "%lu", filelen);
				while(!feof(fp1)) {
					l = r = 0X00000000;
					retstrlen1 = fread(&l, sizeof(unsigned long), 1, fp1);
					retstrlen = fread(&r, sizeof(unsigned long), 1, fp1);
					i = 0;
					//      printf("\n%d %d", retstrlen1, retstrlen);
					Blowfish_Encrypt(&ctx, &l, &r);
					cl = &l;
					cr = &r;
					for(i = 0; i < 8; i++) {
						fprintf(fp2, "%c", cl[i]);
					}
					for(i = 0; i < 8; i++) {
						fprintf(fp2, "%c", cr[i]);
					}
					printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 8)));
					fflush(stdout);
				}
			} 
			else if(!strcmp(argv[1], "dec")) {
				strcat(filename, ".dec");
				fp2 = fopen(filename, "w");
				if(fp2 == NULL) {
					perror("Opened failed : ");
					return errno;
				}
				fscanf(fp1, "%lu", &filelen);
				filelen = filelen - keylen;
				while(!feof(fp1)) {
					l = r = 0X00000000;
					retstrlen1 = fread(&l, sizeof(unsigned long), 1, fp1);
					retstrlen = fread(&r, sizeof(unsigned long), 1, fp1);
					i = 0;
					//      printf("\n%d %d", retstrlen1, retstrlen);
					Blowfish_Decrypt(&ctx, &l, &r);
					cl = &l;
					cr = &r;
					for(i = 0; i < 8; i++) {
						if(count > filelen - 1)
							break;
						fprintf(fp2, "%c", cl[i]);
						count++;
					}
					for(i = 0; i < 8; i++) {
						if(count > filelen - 1)
							break;
						fprintf(fp2, "%c", cr[i]);
						count++;
					}
					printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 8)));
					fflush(stdout);
				}
			}
			
			else {
				printf("Wrong arguments\n");
				exit(1);
			}
			fclose(fp1);
			fclose(fp2);
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
//*******************************[OPTION -cD] [OPTION -cdD]*********************************************/
	else if(!strcmp(argv[argc - 1], "-cD") || !strcmp(argv[argc - 1], "-cdD")) {
		DIR *FD1, *FD2;
		FILE *fp1, *fp2;
		struct dirent *sfile, *tfile;
		int d;
		char dirnametmp[50];
		dircount = argc - 4;
		for(; dirindex < dircount; dirindex++) {
			delfile = 1;
			FD1 = opendir(argv[dirindex + 3]);
			count = 0;
			strcpy(dirname, argv[dirindex + 3]);
			strcpy(sdir, dirname);
			if(!strcmp(argv[1], "enc"))
				strcat(dirname, ".enc");
			else
				strcat(dirname, ".dec");
			d = mkdir(dirname, 0700);
			FD2 = opendir(dirname);
			if(FD1 == NULL || FD2 == NULL) {
				perror("Failed to open dir :");
				return errno;
			}
			while((sfile = readdir(FD1))) {
				if(sfile->d_type == DT_DIR)
					continue;
				if(!strcmp(sfile->d_name, "."))
					continue;
				if(!strcmp(sfile->d_name, ".."))
					continue;
				fp1 = fopen(sfile->d_name, "r");
				if(!fp1) {
					perror("Error opening file : ");
					return errno;
				}
				strcpy(filename, sfile->d_name);
				BLOWFISH_CTX ctx;
				Blowfish_Init(&ctx, key, keylen);
				//Even if retstrlen and retstrlen1 is somentimes 0 it contains data
				percent = 0;
				if((!strcmp(argv[1], "enc"))) {
					strcat(filename, ".enc");
					strcpy(filenametmp, filename);
					strcpy(filename, dirname);
					strcat(filename, "/");
					strcat(filename, filenametmp);
					fp2 = fopen(filename, "w");
					if(!fp2) {
						perror("Error opening file : ");
						return errno;
					}
					fseek(fp1, 0L, SEEK_END);
					filelen = ftell(fp1);
					rewind(fp1);
					filelen = filelen + keylen;	//Don't want to store bytes directly
					fprintf(fp2, "%lu", filelen);
					while(!feof(fp1)) {
						l = r = 0X00000000;
						retstrlen1 = fread(&l, sizeof(unsigned long), 1, fp1);
						retstrlen = fread(&r, sizeof(unsigned long), 1, fp1);
						i = 0;
						//      printf("\n%d %d", retstrlen1, retstrlen);
						Blowfish_Encrypt(&ctx, &l, &r);
						cl = &l;
						cr = &r;
						for(i = 0; i < 8; i++) {
							fprintf(fp2, "%c", cl[i]);
						}
						for(i = 0; i < 8; i++) {
							fprintf(fp2, "%c", cr[i]);
						}
						printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 8)));
						fflush(stdout);
					}
				} 
				else if(!strcmp(argv[1], "dec")) {
					strcat(filename, ".dec");
					strcpy(filenametmp, filename);
					strcpy(filename, dirname);
					strcat(filename, "/");
					strcat(filename, filenametmp);
					fp2 = fopen(filename, "w");
					fscanf(fp1, "%lu", &filelen);
					filelen = filelen - keylen;
					while(!feof(fp1)) {
						l = r = 0X00000000;
						retstrlen1 = fread(&l, sizeof(unsigned long), 1, fp1);
						retstrlen = fread(&r, sizeof(unsigned long), 1, fp1);
						i = 0;
						//      printf("\n%d %d", retstrlen1, retstrlen);
						Blowfish_Decrypt(&ctx, &l, &r);
						cl = &l;
						cr = &r;
						for(i = 0; i < 8; i++) {
							if(count > filelen - 1)
								break;
							fprintf(fp2, "%c", cl[i]);
							count++;
						}
						for(i = 0; i < 8; i++) {
							if(count > filelen - 1)
								break;
							fprintf(fp2, "%c", cr[i]);
							count++;
						}
						printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 8)));
						fflush(stdout);
					}
				} 
				else {
					printf("Wrong arguments\n");
					exit(1);
				}
				fclose(fp1);
				fclose(fp2);
				printf("\n");
				if(!strcmp(argv[1], "enc"))
					printf("Your data is successfully encrypted in %s\n", filename);
				else
					printf("Your data is successfully decrypted in %s\n", filename);
				if(!strcmp(argv[argc - 1], "-cdD")) {
					strcpy(dirnametmp, argv[dirindex + 3]);
					strcat(dirnametmp, "/");
					strcat(dirnametmp, sfile->d_name);
					delfile = remove(dirnametmp);
				}
				if(!delfile)
					printf("File %s deleted\n", dirnametmp);
			}
			closedir(FD1);
			closedir(FD2);
			if(!strcmp(argv[argc - 1], "-cdD")) {
				if(!rmdir(sdir));
					printf("Directory %s deleted\n", sdir);
			}
		}
		return 0;
	}
/*******************************[OPTION -cD] [OPTION -cdD]*******************************************/
/*******************************[OPTION -D] [OPTION -dD]*********************************************/
	else if(!strcmp(argv[argc - 1], "-D") || !strcmp(argv[argc - 1], "-dD")) {
		DIR *FD1, *FD2;
		FILE *fp1, *fp2;
		struct dirent *sfile, *tfile;
		int d;
		char dirnametmp[50];
		dircount = argc - 4;
		for(; dirindex < dircount; dirindex+=2) {
			delfile = 1;
			FD1 = opendir(argv[dirindex + 3]);
			strcpy(sdir, argv[dirindex + 3]);
			count = 0;
			struct stat st = {0};
			if(stat(argv[dirindex + 4], &st) == -1)
				mkdir(argv[dirindex + 4], 0700);
			FD2 = opendir(argv[dirindex + 4]);
			strcpy(dirnametmp, argv[dirindex + 4]);
			if(FD1 == NULL || FD2 == NULL) {
				perror("Failed to open dir :");
				return errno;
			}
			while((sfile = readdir(FD1))) {
				if(sfile->d_type == DT_DIR)
					continue;
				if(!strcmp(sfile->d_name, "."))
					continue;
				if(!strcmp(sfile->d_name, ".."))
					continue;
				fp1 = fopen(sfile->d_name, "r");
				if(!fp1) {
					perror("Error opening file : ");
					return errno;
				}
				strcpy(filename, sfile->d_name);
				BLOWFISH_CTX ctx;
				Blowfish_Init(&ctx, key, keylen);
				//Even if retstrlen and retstrlen1 is somentimes 0 it contains data
				percent = 0;
				if((!strcmp(argv[1], "enc"))) {
					strcat(filename, ".enc");
					strcpy(filenametmp, filename);
					strcpy(filename, dirname);
					strcat(filename, "/");
					strcat(filename, filenametmp);
					fp2 = fopen(filename, "w");
					if(!fp2) {
						perror("Error opening file : ");
						return errno;
					}
					fseek(fp1, 0L, SEEK_END);
					filelen = ftell(fp1);
					rewind(fp1);
					filelen = filelen + keylen;	//Don't want to store bytes directly
					fprintf(fp2, "%lu", filelen);
					while(!feof(fp1)) {
						l = r = 0X00000000;
						retstrlen1 = fread(&l, sizeof(unsigned long), 1, fp1);
						retstrlen = fread(&r, sizeof(unsigned long), 1, fp1);
						i = 0;
						//      printf("\n%d %d", retstrlen1, retstrlen);
						Blowfish_Encrypt(&ctx, &l, &r);
						cl = &l;
						cr = &r;
						for(i = 0; i < 8; i++) {
							fprintf(fp2, "%c", cl[i]);
						}
						for(i = 0; i < 8; i++) {
							fprintf(fp2, "%c", cr[i]);
						}
						printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 8)));
						fflush(stdout);
					}
				} 
				else if(!strcmp(argv[1], "dec")) {
					strcat(filename, ".dec");
					strcpy(filenametmp, filename);
					strcpy(filename, dirname);
					strcat(filename, "/");
					strcat(filename, filenametmp);
					fp2 = fopen(filename, "w");
					fscanf(fp1, "%lu", &filelen);
					filelen = filelen - keylen;
					while(!feof(fp1)) {
						l = r = 0X00000000;
						retstrlen1 = fread(&l, sizeof(unsigned long), 1, fp1);
						retstrlen = fread(&r, sizeof(unsigned long), 1, fp1);
						i = 0;
						//      printf("\n%d %d", retstrlen1, retstrlen);
						Blowfish_Decrypt(&ctx, &l, &r);
						cl = &l;
						cr = &r;
						for(i = 0; i < 8; i++) {
							if(count > filelen - 1)
								break;
							fprintf(fp2, "%c", cl[i]);
							count++;
						}
						for(i = 0; i < 8; i++) {
							if(count > filelen - 1)
								break;
							fprintf(fp2, "%c", cr[i]);
							count++;
						}
						printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 8)));
						fflush(stdout);
					}
				} 
				else {
					printf("Wrong arguments\n");
					exit(1);
				}
				fclose(fp1);
				fclose(fp2);
				printf("\n");
				if(!strcmp(argv[1], "enc"))
					printf("Your data is successfully encrypted in %s\n", filename);
				else
					printf("Your data is successfully decrypted in %s\n", filename);
				if(!strcmp(argv[argc - 1], "-dD")) {
					strcpy(dirnametmp, argv[dirindex + 4]);
					strcat(dirnametmp, "/");
					strcat(dirnametmp, sfile->d_name);
					delfile = remove(dirnametmp);
				}
				if(!delfile)
					printf("File %s deleted\n", dirnametmp);
			}
			closedir(FD1);
			closedir(FD2);
			if(!strcmp(argv[argc - 1], "-dD")) {
				if(!rmdir(sdir));
					printf("Directory %s deleted\n", sdir);
			}
		}
		return 0;
	}
/*******************************[OPTION -D] [OPTION -dD]*******************************************/
/*******************************NO OPTION [OPTION -d]*********************************/
	else {
		FILE *fp1, *fp2;
		if(!strcmp(argv[argc - 1], "-d"))
			filecount = argc - 4;
		else
			filecount = argc - 3;
		for(; fileindex < filecount; fileindex += 2) {
			count = 0;
			delfile = 1;
			fp1 = fopen(argv[fileindex + 3], "r");
			fp2 = fopen(argv[fileindex + 4], "w");
			if(fp1 == NULL || fp2 == NULL) {
				perror("Opened failed : ");
				return errno;
			}
			BLOWFISH_CTX ctx;
			Blowfish_Init(&ctx, key, keylen);
			//Even if retstrlen and retstrlen1 is somentimes 0 it contains data
			percent = 0;
			if((!strcmp(argv[1], "enc"))) {
				fseek(fp1, 0L, SEEK_END);
				filelen = ftell(fp1);
				rewind(fp1);
				filelen = filelen + keylen;	//Don't want to store bytes directly
				fprintf(fp2, "%lu", filelen);
				while(!feof(fp1)) {
					l = r = 0X00000000;
					retstrlen1 = fread(&l, sizeof(unsigned long), 1, fp1);
					retstrlen = fread(&r, sizeof(unsigned long), 1, fp1);
					i = 0;
					//      printf("\n%d %d", retstrlen1, retstrlen);
					Blowfish_Encrypt(&ctx, &l, &r);
					cl = &l;
					cr = &r;
					for(i = 0; i < 8; i++) {
						fprintf(fp2, "%c", cl[i]);
					}
					for(i = 0; i < 8; i++) {
						fprintf(fp2, "%c", cr[i]);
					}
					printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 8)));
					fflush(stdout);
				}
			} 
			else if(!strcmp(argv[1], "dec")) {
				fscanf(fp1, "%lu", &filelen);
				filelen = filelen - keylen;
				while(!feof(fp1)) {
					l = r = 0X00000000;
					retstrlen1 = fread(&l, sizeof(unsigned long), 1, fp1);
					retstrlen = fread(&r, sizeof(unsigned long), 1, fp1);
					i = 0;
					//      printf("\n%d %d", retstrlen1, retstrlen);
					Blowfish_Decrypt(&ctx, &l, &r);
					cl = &l;
					cr = &r;
					for(i = 0; i < 8; i++) {
						if(count > filelen - 1)
							break;
						fprintf(fp2, "%c", cl[i]);
						count++;
					}
					for(i = 0; i < 8; i++) {
						if(count > filelen - 1)
							break;
						fprintf(fp2, "%c", cr[i]);
						count++;
					}
					printf("\rIn progress %d%%", (int)((16 * percent++ * 100) / (filelen - 8)));
					fflush(stdout);
				}
			} 
			else {
				printf("Wrong arguments\n");
				exit(1);
			}
			fclose(fp1);
			fclose(fp2);
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

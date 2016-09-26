#define MAXKEYBYTES 56			/* 448 bits */
    typedef struct BLOWFISH_CTX{
	unsigned long P[18];	//16+2
	unsigned long S[4][256];
} BLOWFISH_CTX;
void Blowfish_Init(BLOWFISH_CTX * ctx, char *key, int keyLen);
void Blowfish_Encrypt(BLOWFISH_CTX * ctx, char *xl, char *xr);
void Blowfish_Decrypt(BLOWFISH_CTX * ctx, char *xl, char *xr);

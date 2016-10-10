#include "blowfish.h"
#ifndef __ENC_
#define __ENC_
void Blowfish_Encrypt(BLOWFISH_CTX * ctx, unsigned char *xl, unsigned char *xr) {
	unsigned char temp;
	short i, j;
	for(i = 0; i < 16; ++i) {	/*N=16 bcz we want this thing to run 16 times... Remember the algorithm */
		for(j = 0; j < 8; ++j) {
			xl[j] = xl[j] ^ ctx->P[i];
			xr[j] = F(ctx, xl[j]) ^ xr[j];
			temp = xl[j];
			xl[j] = xr[j];
			xr[j] = temp;
		}
	}
	for(j = 0; j < 8; ++j) {
	temp = xl[j];
	xl[j] = xr[j];
	xr[j] = temp;
	xr[j] = xr[j] ^ ctx->P[N];
	xl[j] = xl[j] ^ ctx->P[N + 1];
	}
}
#endif

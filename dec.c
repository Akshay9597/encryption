#include "blowfish.h"
#ifndef __DEC_
#define __DEC_
void Blowfish_Decrypt(BLOWFISH_CTX * ctx, unsigned char *xl, unsigned char *xr) {
	unsigned char temp;
	short i, j;
	for(i = 16 + 1; i > 1; --i) {
		for(j = 0; j < 8; ++j) {
			xl[j] = xl[j] ^ ctx->P[i];
			xr[j] = F(ctx, xl[j]) ^ xr[j];
			/* Exchange Xl and Xr */
			temp = xl[j];
			xl[j] = xr[j];
			xr[j] = temp;
		}
	}
	/* Exchange Xl and Xr */
	for(j = 0; j < 8; ++j) {		
		temp = xl[j];
		xl[j] = xr[j];
		xr[j] = temp;
		xr[j] = xr[j] ^ ctx->P[1];
		xl[j] = xl[j] ^ ctx->P[0];
	}
}
#endif

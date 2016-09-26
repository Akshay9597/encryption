#ifndef __Blowfish
#define __Blowfish
#include "blowfish.c"
#endif
void Blowfish_Decrypt(BLOWFISH_CTX * ctx, unsigned long *xl, unsigned long *xr) {
	unsigned long Xl;
	unsigned long Xr;
	unsigned long temp;
	short i;
	Xl = *xl;
	Xr = *xr;
	for(i = N + 1; i > 1; --i) {
		Xl = Xl ^ ctx->P[i];
		Xr = F(ctx, Xl) ^ Xr;
		/* Exchange Xl and Xr */
		temp = Xl;
		Xl = Xr;
		Xr = temp;
	}
	/* Exchange Xl and Xr */
	temp = Xl;
	Xl = Xr;
	Xr = temp;
	Xr = Xr ^ ctx->P[1];
	Xl = Xl ^ ctx->P[0];
	*xl = Xl;
	*xr = Xr;
}

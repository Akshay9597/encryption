#ifndef __Blowfish
#define __Blowfish
#include "blowfish.c"
#endif
void Blowfish_Encrypt(BLOWFISH_CTX * ctx, unsigned long *xl, unsigned long *xr) {
	unsigned long Xl;
	unsigned long Xr;
	unsigned long temp;
	short i;
	Xl = *xl;
	Xr = *xr;
	for(i = 0; i < N; ++i) {	/*N=16 bcz we want this thing to run 16 times... Remember the algorithm */
		Xl = Xl ^ ctx->P[i];
		Xr = F(ctx, Xl) ^ Xr;
		temp = Xl;
		Xl = Xr;
		Xr = temp;
	}
	temp = Xl;
	Xl = Xr;
	Xr = temp;
	Xr = Xr ^ ctx->P[N];
	Xl = Xl ^ ctx->P[N + 1];
	*xl = Xl;
	*xr = Xr;
}

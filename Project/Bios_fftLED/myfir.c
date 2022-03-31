#include "myfir.h"

void myfir(int16_t *in,int16_t *delayLine,int16_t *out,uint16_t numOfSamps,uint16_t numOfCoeffs,const int16_t * filterCoeffs) {


	int40_t accumulator = 0;
	int i, k, n;
	for(k=0; k < numOfSamps; k++) {
		accumulator = (int32_t)in[k] * (int32_t)filterCoeffs[0];

		for(i=1; i < numOfCoeffs; i++) {

			//Q0.30
			//accumulator = accumulator + ((int32_t)delayLine[i-1] * (int32_t)filterCoeffs[i]);
			accumulator = _smacr(accumulator, (int32_t)delayLine[i-1], (int32_t)filterCoeffs[i] ); // Use built-in

		}
		//Stored in
		accumulator = accumulator >> 16;
		out[k] = (int16_t)(accumulator);

		for(n = numOfCoeffs-2; n >= 0; n--) {

			delayLine[n+1] = delayLine[n];
		}
		delayLine[0] = in[k];
	}



}

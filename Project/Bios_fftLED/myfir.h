#ifndef BIOSPROJ2_96_MYFIR_H_
#define BIOSPROJ2_96_MYFIR_H_

#include "stdio.h"
#include "ezdsp5502.h"
#include "stdint.h"
#include "stdlib.h"

void myfir(int16_t *in, int16_t *delayLine, int16_t *out, uint16_t numOfSamps, uint16_t numOfCoeffs,const int16_t * filterCoeffs);

#endif

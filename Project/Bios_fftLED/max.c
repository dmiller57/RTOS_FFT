/*
 * max.c
 *
 *  Created on: Apr 6, 2020
 *      Author: Tom Faulconer
 */


#include "max.h"


uint16_t findMaximum(uint16_t* FFT_output_array) {
	int i;
	uint16_t max = 0;
	// Only need to look at first half of fft output array bc it's mirrored at Fs/2
	for (i = 0; i < 128; i++) {
		if (FFT_output_array[i] > max) {
			max = FFT_output_array[i];
		}
	}

	return max;
}

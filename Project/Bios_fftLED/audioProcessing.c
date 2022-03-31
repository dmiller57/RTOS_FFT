#include <std.h>
#include <stdbool.h>
#include <log.h>

#include "hellocfg.h"
#include "ezdsp5502.h"
#include "stdint.h"
#include "aic3204.h"
#include "ezdsp5502_mcbsp.h"
#include "ezdsp5502_i2cgpio.h"			// for the SW0 and SW1 macro definitions
#include "csl_mcbsp.h"

#include "FFT.h"
#include "myfir.h"

extern MCBSP_Handle aicMcbsp;
extern int16_t switchInterrupt;
extern int16_t switchCount;
extern bool btn_press1;
extern bool btn_press2;

extern void myfir(int16_t *in, int16_t *delayLine, int16_t *out, uint16_t numOfSamps, uint16_t numOfCoeffs,const int16_t * filterCoeffs);


/* Global Vars for Processing */

//LPF_50-1150 Hz -- 55dB att -- order 80, 81 coeffs

const int16_t myLPF[81] =
{
        86,     39,     48,     57,     67,     79,     91,    105,    119,    135,    151,    169,    187,    206,    226,    247,
       268,    289,    311,    334,    356,    379,    401,    424,    446,    467,    488,    508,    527,    546,    563,    579,
       593,    606,    618,    628,    636,    643,    647,    650,    651,    650,    647,    643,    636,    628,    618,    606,
       593,    579,    563,    546,    527,    508,    488,    467,    446,    424,    401,    379,    356,    334,    311,    289,
       268,    247,    226,    206,    187,    169,    151,    135,    119,    105,     91,     79,     67,     57,     48,     39,
        86,
};

//const int16_t myLPF[61] =
//{
// 		-4,   -7,    -12,    -19,    -28,    -38,    -50,    -61,    -71,    -77,    -78,    -70,    -51,    -18,     33,    102,
//        192,    304,    437,    589,    757,    938,   1126,   1315,   1498,   1668,   1819,   1943,   2036,   2094,   2114,   2094,
//        2036,   1943,   1819,   1668,   1498,   1315,   1126,    938,    757,    589,    437,    304,    192,    102,     33,    -18,
//       -51,    -70,    -78,    -77,    -71,    -61,    -50,    -38,    -28,    -19,    -12,     -7,     -4
//};

const int16_t inputTestVector[256] = {0,   5165,   9215,  11327,  11190,   9083,   5793,   2403,      0,   -624,818,   4026,
		8192,  12218,  15007,  15760,      14189,  10595,   5793,    891,  -2998,  -5057,  -4974,  -3027,      0,
		3027,4974,   5057,   2998,   -891,  -5793, -10595,     -14189, -15760, -15007, -12218,  -8192,  -4026,
		-818,    624,      0,  -2403,-5793,  -9083, -11190, -11327,  -9215,  -5165,          0,   5165,   9215,  11327,
		11190,   9083,   5793,   2403,      0,   -624,818,   4026,   8192,  12218,  15007,  15760,      14189,  10595,
		5793,    891,  -2998,  -5057,  -4974,  -3027,      0,   3027,4974,   5057,   2998,   -891,  -5793, -10595,
		-14189, -15760, -15007, -12218,  -8192,  -4026,   -818,    624,      0,
		-2403,-5793,  -9083, -11190, -11327,  -9215,  -5165,          0,   5165,   9215,  11327,  11190,   9083,   5793,
		2403,      0,   -624,818,   4026,   8192,  12218,  15007,  15760,      14189,  10595,   5793,    891,  -2998,
		-5057,  -4974,  -3027,      0,   3027,4974,   5057,   2998,   -891,  -5793, -10595,     -14189, -15760, -15007,
		-12218,  -8192,  -4026,   -818,    624,      0,  -2403,-5793,  -9083, -11190, -11327,  -9215,  -5165,          0,
		5165,   9215,  11327,  11190,   9083,   5793,   2403,      0,   -624,818,   4026,   8192,  12218,  15007,  15760,
		14189,  10595,   5793,    891,  -2998,  -5057,  -4974,  -3027,      0,   3027,4974,   5057,   2998,   -891,  -5793,
		-10595,     -14189, -15760, -15007, -12218,  -8192,  -4026,   -818,    624,      0,  -2403,-5793,  -9083, -11190,
		-11327,  -9215,  -5165,          0,   5165,   9215,  11327,  11190,   9083,   5793,   2403,      0,   -624,818,
		4026,   8192,  12218,  15007,  15760,      14189,  10595,   5793,    891,  -2998,  -5057,  -4974,  -3027,      0,
		3027,4974,   5057,   2998,   -891,  -5793, -10595,     -14189, -15760, -15007, -12218,  -8192,  -4026,   -818,
		624,      0,  -2403,-5793,  -9083, -11190, -11327,  -9215,  -5165,          0,   5165,   9215,  11327,  11190,
		9083,   5793,   2403,      0,   -624,818,   4026,   8192,  12218,  15007,  15760};
int16_t delayLine[80];
int16_t dataIn;

//FOR PROJECT 3......TSK
int16_t inBuffer[96];
int16_t outBuffer[96];
uint16_t inCount = 0;
uint16_t outCount = 0;
bool fillTop = true;
bool fillReadTop = true;

//Previously for project 2...nco
int16_t leftBuffer[96];
int16_t rightBuffer[96];
int16_t leftOut[96];
int16_t rightOut[96];

int16_t fftBuffer[256];
int16_t fftIndex = 0;

uint16_t leftCount = 0;
uint16_t rightCount = 0;
uint16_t leftCountOut = 0;
uint16_t rightCountOut = 0;
uint16_t softCount = 0;
bool useLeft = true;
bool useLeftOut = true;
bool softIndex = true;
bool fftPriority = true;

void audioProcessingInit(void) {
	dataIn = 0;
	memset(delayLine, 0, sizeof(delayLine));
	memset(leftOut, 0, sizeof(outBuffer));
	memset(rightOut, 0, sizeof(rightOut));
}

void SWI_AudioProc(void) {

}

void HWI_I2S_Rx(void) {


	/*
	 * Need to reorganize this HWI. The audio TSK will do all of the grinding in through the mailbox.
	 * Mailbox needs to pend and post only one time, so a 96 buffer is necessary to use.
	 * To make the filter implementation easier later on we should consider sorting the left half in the top and the
	 * 		right half in the bottom of the array. That way, in the TSK, we can just give the lower 48 to memory (the right
	 * 		channel), and have it filter those instead of trying to figure out which is which. That being said, I think that we
	 * 		could potentially use a for loop like in the SWI to do the same thing and just dictate which loop it will go into if the
	 * 		button is pressed or not.
	 *
	 * TODO: the HWI_Rx needs to be modified to take in one [96] buffer. Then post to the mailbox once, and reset. Buffers can be immediately
	 * 		reset after they fill a mailbox.
	 *
	 * TODO: note that inBuffer[96] and outBuffer[96] have been made to store these already. Look above in global variables. The variables stored for
	 * 		Project 2 (NCO implementation) are being kept for build purposes, we can delete them later or as we go to clean up and free memory.
	 */
//fill top half with left and bottom with right, one 96 buffer to a mailbox, post and pend once

	EZDSP5502_MCBSP_read((Int16*) &dataIn);

	if (fillTop == true) {

		inBuffer[inCount] = dataIn;
		fftBuffer[fftIndex] = dataIn;
		fftIndex++;
		fillTop = false;
	}

	else {

		inBuffer[inCount+48] = dataIn;
		inCount++;
		fillTop = true;


		if (inCount == 48) {

			MBX_post(&audioMBX, inBuffer, 0);
			inCount = 0;
		}
	}
	if (fftIndex == 256){
		MBX_post(&fftMBX, fftBuffer, 0);
		fftIndex = 0;
	}
}

void HWI_I2S_Tx(void) {

		if (fillReadTop == true) {

			EZDSP5502_MCBSP_write(outBuffer[outCount]);
			fillReadTop = false;
		}

		else {

			EZDSP5502_MCBSP_write(outBuffer[outCount+48]);
			outCount++;
			fillReadTop = true;

			if (outCount == 48) {

				outCount = 0;
			}
		}

}

void TSK_audio(Arg ztemp) {
/*
 * Single buffer has been implemented here. This is simply just a pass through version of
 *		the audio TSK, for testing. I think the dual mailbox pend was causing issues and it
 *		was no longer RT. I dont think any further modifications here will be necessary. Take a look
 *		though, I am wrong often.
 *
 * NOTE: that the SYS_FOREVER variable is literally just a -1 that has been typecasted. So we can always
 *		use -1.
 */

	int16_t tempBuffer[96];
	int index = 0;

	while(1) {

		MBX_pend(&audioMBX, tempBuffer, SYS_FOREVER);// Blocking

		// If button is pressed then filter both channels
		if (btn_press1 == 1) {

			for(index = 0; index < 48; index++) {

				/*
				 * Filter both channels  Left (0-47 samples) Right (48-95 Samples)
				 */
				myfir((int16_t*)&tempBuffer[index], delayLine, (int16_t*)&outBuffer[index], 1, 80, myLPF);
				myfir((int16_t*)&tempBuffer[index+48], delayLine, (int16_t*)&outBuffer[index+48], 1, 80, myLPF);
			}
		} else {
				/*
				 * Don't filter either channel
				 */
				for(index = 0; index < 48; index++) {
					outBuffer[index] = tempBuffer[index];
					outBuffer[index+48] = tempBuffer[index+48];
				}
		}
//		fftBuffer[fftIndex++] = tempBuffer[fftIndex];
//		if (fftIndex == 256){
//			MBX_post(&fftMBX, fftBuffer, 0);
//			fftIndex = 0;
//		}
	}
}

void TSK_button(Arg xtemp) {

	// Intro stuff

	while(1) {


		TSK_sleep(50); //Blocking,  1 tick = 1 ms
		SEM_pend(&SEM_i2c, SYS_FOREVER); // block forever until the i2c bus is available
		if (EZDSP5502_I2CGPIO_readLine(SW0) == 0) {
			btn_press1 = btn_press1 ^ 1; // XOR toggles the btn_press variable
			//EZDSP5502_waitusec( 100000 );
		}

		if (EZDSP5502_I2CGPIO_readLine(SW1) == 0) {
			btn_press2 = btn_press2 ^ 1; // XOR toggles the btn_press variable
			if (btn_press2 == 1) {
				EZDSP5502_I2CGPIO_writeLine(   LED0, HIGH ); // if btn is pressed turn on LED
			} else {
				EZDSP5502_I2CGPIO_writeLine(   LED0, LOW ); // if btn is not pressed turn off LED
			}
		}
		SEM_post(&SEM_i2c); // Release the i2c bus for use

//		TSK_yield();
//		TSK_setpri(&buttonTSK, 13);
//		TSK_setpri(&fftTSK, 14);

	}

}

// PRESTINE VERSION
void TSK_fft(Arg ytemp) {
	// Intro stuff
	int i = 0;
	FFT_initialize();
	while(1) {

		MBX_pend(&fftMBX, FFT_U.In1, -1); // Blocking
		// For loop puts the inputTestVector into the FFT USED FOR TESTING
//		for(i = 0; i < 256; i++) {
//			FFT_U.In1[i] = inputTestVector[i];
//		}

		// Computes 256 point FFT
		FFT_step();
		// Send the outputs to the lcd Mailbox
		MBX_post(&lcdMBX, FFT_Y.Out1, 0); // posts all 256 values, but we only take 128
//		TSK_yield();
		TSK_sleep(5); //Blocking,  1 tick = 1 ms
	}

}

// NEW TEST VERSION
//void TSK_fft(Arg ytemp) {
//	// Intro stuff
//	int i = 0;
//	int16_t lcdBuffer[128]; // holds the first half of FFT results
//	Uint16 cmdArray[192]; // holds 96 columns x 2 pages
//	//int16_t maxAmplitude;
//	uint16_t j;
//	uint16_t count = 0;
//	int16_t step = (MAX_VALUE / 16);
//	FFT_initialize();
//	while(1) {
//
//		MBX_pend(&fftMBX, FFT_U.In1, SYS_FOREVER); // Blocking
//		FFT_step();
//		MBX_post(&lcdMBX, FFT_Y.Out1, 0); // posts all 256 values, but we only take 128
//		MBX_pend(&lcdMBX, lcdBuffer, SYS_FOREVER); // wait forever
//
//		//		find maximum magnitude value
//				//maxAmplitude = findMaximum(lcdBuffer);
//		//		plot pixels
//				// Only have 96 pixel screen so can't plot all 128 pixels
//				for (j = 96; j > 0; j--) {
//					if ( lcdBuffer[j] >= MAX_VALUE ) {
//						cmdArray[count++] = 0xFF; // page 0
//						cmdArray[count++] = 0xFF; // page 1
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0xFF);
//		//				osd9616_send(0x40, 0xFF);
//					} else if ( lcdBuffer[j] < MAX_VALUE && lcdBuffer[j] >= (step*15) ) {
//						cmdArray[count++] = 0xFE;
//						cmdArray[count++] = 0xFF;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0xFE);
//		//				osd9616_send(0x40, 0xFF);
//					} else if ( lcdBuffer[j] < (step*15) && lcdBuffer[j] >= (step*14) ) {
//						cmdArray[count++] = 0xFC;
//						cmdArray[count++] = 0xFF;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0xFC);
//		//				osd9616_send(0x40, 0xFF);
//					} else if ( lcdBuffer[j] < (step*14) && lcdBuffer[j] >= (step*13) ) {
//						cmdArray[count++] = 0xF8;
//						cmdArray[count++] = 0xFF;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0xF8);
//		//				osd9616_send(0x40, 0xFF);
//					} else if ( lcdBuffer[j] < (step*13) && lcdBuffer[j] >= (step*12) ) {
//						cmdArray[count++] = 0xF0;
//						cmdArray[count++] = 0xFF;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0xF0);
//		//				osd9616_send(0x40, 0xFF);
//					} else if ( lcdBuffer[j] < (step*12) && lcdBuffer[j] >= (step*11) ) {
//						cmdArray[count++] = 0xE0;
//						cmdArray[count++] = 0xFF;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0xE0);
//		//				osd9616_send(0x40, 0xFF);
//					} else if ( lcdBuffer[j] < (step*11) && lcdBuffer[j] >= (step*10) ) {
//						cmdArray[count++] = 0xC0;
//						cmdArray[count++] = 0xFF;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0xC0);
//		//				osd9616_send(0x40, 0xFF);
//					} else if ( lcdBuffer[j] < (step*10) && lcdBuffer[j] >= (step*9) ) {
//						cmdArray[count++] = 0x80;
//						cmdArray[count++] = 0xFF;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x80);
//		//				osd9616_send(0x40, 0xFF);
//					} else if ( lcdBuffer[j] < (step*9) && lcdBuffer[j] >= (step*8) ) {
//						cmdArray[count++] = 0x00;
//						cmdArray[count++] = 0xFF;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x00);
//		//				osd9616_send(0x40, 0xFF);
//					} else if ( lcdBuffer[j] < (step*8) && lcdBuffer[j] >= (step*7) ) {
//						cmdArray[count++] = 0x00;
//						cmdArray[count++] = 0xFE;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x00);
//		//				osd9616_send(0x40, 0xFE);
//					} else if ( lcdBuffer[j] < (step*7) && lcdBuffer[j] >= (step*6) ) {
//						cmdArray[count++] = 0x00;
//						cmdArray[count++] = 0xFC;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x00);
//		//				osd9616_send(0x40, 0xFC);
//					} else if ( lcdBuffer[j] < (step*6) && lcdBuffer[j] >= (step*5) ) {
//						cmdArray[count++] = 0x00;
//						cmdArray[count++] = 0xF8;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x00);
//		//				osd9616_send(0x40, 0xF8);
//					} else if ( lcdBuffer[j] < (step*5) && lcdBuffer[j] >= (step*4) ) {
//						cmdArray[count++] = 0x00;
//						cmdArray[count++] = 0xF0;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x00);
//		//				osd9616_send(0x40, 0xF0);
//					} else if ( lcdBuffer[j] < (step*4) && lcdBuffer[j] >= (step*3) ) {
//						cmdArray[count++] = 0x00;
//						cmdArray[count++] = 0xE0;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x00);
//		//				osd9616_send(0x40, 0xE0);
//					} else if ( lcdBuffer[j] < (step*3) && lcdBuffer[j] >= (step*2) ) {
//						cmdArray[count++] = 0x00;
//						cmdArray[count++] = 0xC0;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x00);
//		//				osd9616_send(0x40, 0xC0);
//					} else if ( lcdBuffer[j] < (step*2) && lcdBuffer[j] >= (step*1) ) { // where it comes it at j = 18, count = 156
//						cmdArray[count++] = 0x00;
//						cmdArray[count++] = 0x80;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x00);
//		//				osd9616_send(0x40, 0x80);
//					} else if ( lcdBuffer[j] < (step*1) && lcdBuffer[j] >= (step*0) ) {
//						cmdArray[count++] = 0x00;
//						cmdArray[count++] = 0x80;
//						if (count >= 192) {
//							count = 0;
//						}
//		//				osd9616_send(0x40, 0x00);
//		//				osd9616_send(0x40, 0x80);
//					}
//				} // end FOr loop
//				osd9616_multiSend(cmdArray, 193); // cmdArray is 192 -- command is 1 value = 193
//	} // end WHILE
//
//} // end TSK



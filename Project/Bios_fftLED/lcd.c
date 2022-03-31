//////////////////////////////////////////////////////////////////////////////
// * File name: lcd.c
// *                                                                          
// * Description:  LCD functions.
// *                                                                          
// * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
// * Copyright (C) 2011 Spectrum Digital, Incorporated
// *                                                                          
// *                                                                          
// *  Redistribution and use in source and binary forms, with or without      
// *  modification, are permitted provided that the following conditions      
// *  are met:                                                                
// *                                                                          
// *    Redistributions of source code must retain the above copyright        
// *    notice, this list of conditions and the following disclaimer.         
// *                                                                          
// *    Redistributions in binary form must reproduce the above copyright     
// *    notice, this list of conditions and the following disclaimer in the   
// *    documentation and/or other materials provided with the                
// *    distribution.                                                         
// *                                                                          
// *    Neither the name of Texas Instruments Incorporated nor the names of   
// *    its contributors may be used to endorse or promote products derived   
// *    from this software without specific prior written permission.         
// *                                                                          
// *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     
// *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       
// *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   
// *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    
// *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   
// *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        
// *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   
// *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   
// *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     
// *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   
// *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    
// *                                                                          
//////////////////////////////////////////////////////////////////////////////
 
#include"ezdsp5502_i2c.h"
#include"ezdsp5502_gpio.h"
#include "csl_gpio.h"
#include"lcd.h"
#include "stdint.h"
#include "hellocfg.h"

// Project 4
extern uint16_t findMaximum(uint16_t* FFT_output_array);
#define MAX_VALUE 1500

#define OSD9616_I2C_ADDR 0x3C    // OSD9616 I2C address = 0111 100



/*
 *
 *  Int16 OSD9616_init( )
 *
 *      Initialize LCD display
 *
 */
Int16 osd9616_init( )
{
	Int16 i;
    Uint16 cmd[10];    // For multibyte commands
    
    /* Initialize LCD power */
    EZDSP5502_GPIO_init( GPIO_GPIO_PIN1 );         // Enable GPIO pin
    EZDSP5502_GPIO_setDirection( GPIO_GPIO_PIN1, GPIO_GPIO_PIN1_OUTPUT );  // Output
    EZDSP5502_GPIO_setOutput( GPIO_GPIO_PIN1, 1 ); // Enable 13V 
    
    /* Set vertical addressing mode */
	cmd[0] = 0x00 & 0x00FF; // command
	cmd[1] = 0x20; // command 1
	cmd[2] = 0x01; // value for vertical addressing
	osd9616_multiSend( cmd, 3 );


	/* Set column start and end address */
	cmd[0] = 0x00 & 0x00FF; // command = 0 / Data = 1
	cmd[1] = 0x21; // command "Set Column Address"
	cmd[2] = 0; // Start column location (0-127)
	cmd[3] = 95; // End column location (0-127)
	osd9616_multiSend( cmd, 4 );


	/*
	 * Set page start and end address
	 * Set Start page to page 0
	 * Set End page to page 1
	 */
	cmd[0] = 0x00 & 0x00FF; //  command = 0 / Data = 1
	cmd[1] = 0x22; // command "Set Page Address"
	cmd[2] = 0; // Start Page (Page 0 - Page 7)
	cmd[3] = 1; // End Page (Page 0 - Page 7)
	osd9616_multiSend( cmd, 4 );

    /* Set contrast control register */
    cmd[0] = 0x00 & 0x00FF;
    cmd[1] = 0x81;
    cmd[2] = 0x7f;
    osd9616_multiSend( cmd, 3 );

    osd9616_send(0x00,0xa1); // Set segment re-map 95 to 0
    osd9616_send(0x00,0xa6); // Set normal display

    /* Set multiplex ratio(1 to 16) */
    cmd[0] = 0x00 & 0x00FF;
    cmd[1] = 0xa8; 
    cmd[2] = 0x0f;
    osd9616_multiSend( cmd, 3 );

    osd9616_send(0x00,0xd3); // Set display offset
    osd9616_send(0x00,0x00); // Not offset
    osd9616_send(0x00,0xd5); // Set display clock divide ratio/oscillator frequency
    osd9616_send(0x00,0xf0); // Set divide ratio

	osd9616_send(0x00,0x2e);  // Deactivate Scrolling

    /* Set pre-charge period */
    cmd[0] = 0x00 & 0x00FF;
    cmd[1] = 0xd9;
    cmd[2] = 0x22;
    osd9616_multiSend( cmd, 3 );

    /* Set com pins hardware configuration */
    cmd[0] = 0x00 & 0x00FF;
    cmd[1] = 0xda;
    cmd[2] = 0x02;
    osd9616_multiSend( cmd, 3 );

    osd9616_send(0x00,0xdb); // Set vcomh
    osd9616_send(0x00,0x49); // 0.83*vref
    
    /* set DC-DC enable */
    cmd[0] = 0x00 & 0x00FF; 
    cmd[1] = 0x8d;
    cmd[2] = 0x14;
    osd9616_multiSend( cmd, 3 );

    /* Clear out what's on the screen */
	i = osd9616_send(0x00,0x00);   // Set low column address
	osd9616_send(0x00,0x10);   // Set high column address
	osd9616_send(0x00,0xb0+0); // Set page for page 0 to page 5 - moving cursor on display
	for(i=0;i<192;i++) // 96*2 = 192
	{
		osd9616_send(0x40,0x00);
	}

    osd9616_send(0x00,0xaf); // Turn on oled panel
    
    return 0;
}

/*
 *
 *  Int16 osd9616_send( Uint16 comdat, Uint16 data )
 *
 *      Sends 2 bytes of data to the osd9616
 *
 */
Int16 osd9616_send( Uint16 comdat, Uint16 data )
{
    Uint16 cmd[2];					// two 16-bit words
    cmd[0] = comdat & 0x00FF;     // Specifies whether data is Command or Data
    cmd[1] = data;                // Command / Data

    /* Write to OSD9616 */
    return EZDSP5502_I2C_write( OSD9616_I2C_ADDR, cmd, 2 );
}

/*
 *
 *  Int16 osd9616_multiSend( Uint16 comdat, Uint16 data )
 *
 *      Sends multiple bytes of data to the osd9616
 *
 */
Int16 osd9616_multiSend( Uint16* data, Uint16 len )
{
    Uint16 x;
    Uint16 cmd[193];				// 1 command to tell were writing to screen, 192 commands to fill in screen = 193
    cmd[0] = 0x40; 					// Command for writing to LED segments (haven't figured out yet, this is what example did)
    for(x=1;x<len;x++)               // Command / Data
    {
        cmd[x] = data[x-1];			// cmd[1] = cmdArray[0]
    }
    
    /* Write len bytes to OSD9616 */
    return EZDSP5502_I2C_write( OSD9616_I2C_ADDR, cmd, len );
}

/*
 *
 *  Int16 printLetter(Uint16 l1,Uint16 l2,Uint16 l3,Uint16 l4)
 * 
 *      Send 4 bytes representing a Character
 *
 */
Int16 printLetter(Uint16 c4,Uint16 c3,Uint16 c2,Uint16 c1)
{
    osd9616_send(0x40, c4);    // Column 4
    osd9616_send(0x40, c3);    // Column 3
    osd9616_send(0x40, c2);    // Column 2
    osd9616_send(0x40, c1);    // Column 1
    osd9616_send(0x40, 0x00);
    
    return 0;
}


/*
 *
 *
 *  LCD Task
 *
 *
 */
void TSK_lcd(void){

	// Intro stuff
	int16_t lcdBuffer[128]; // holds the first half of FFT results
	Uint16 cmdArray[192]; // holds 96 columns x 2 pages
	//int16_t maxAmplitude;
	uint16_t j;
	uint16_t count = 0;
	int16_t step = (MAX_VALUE / 16);
	while(1) {
		// Receive FFT output data
//		MBX_pend(&fftMBX, FFT_U.In1, -1); // Blocking
		MBX_pend(&lcdMBX, lcdBuffer, SYS_FOREVER); // wait forever

//		find maximum magnitude value
		//maxAmplitude = findMaximum(lcdBuffer);
//		plot pixels
		// Only have 96 pixel screen so can't plot all 128 pixels
		for (j = 96; j > 0; j--) {
			if ( lcdBuffer[j] >= MAX_VALUE ) {
				cmdArray[count++] = 0xFF; // page 0
				cmdArray[count++] = 0xFF; // page 1
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0xFF);
//				osd9616_send(0x40, 0xFF);
			} else if ( lcdBuffer[j] < MAX_VALUE && lcdBuffer[j] >= (step*15) ) {
				cmdArray[count++] = 0xFE;
				cmdArray[count++] = 0xFF;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0xFE);
//				osd9616_send(0x40, 0xFF);
			} else if ( lcdBuffer[j] < (step*15) && lcdBuffer[j] >= (step*14) ) {
				cmdArray[count++] = 0xFC;
				cmdArray[count++] = 0xFF;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0xFC);
//				osd9616_send(0x40, 0xFF);
			} else if ( lcdBuffer[j] < (step*14) && lcdBuffer[j] >= (step*13) ) {
				cmdArray[count++] = 0xF8;
				cmdArray[count++] = 0xFF;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0xF8);
//				osd9616_send(0x40, 0xFF);
			} else if ( lcdBuffer[j] < (step*13) && lcdBuffer[j] >= (step*12) ) {
				cmdArray[count++] = 0xF0;
				cmdArray[count++] = 0xFF;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0xF0);
//				osd9616_send(0x40, 0xFF);
			} else if ( lcdBuffer[j] < (step*12) && lcdBuffer[j] >= (step*11) ) {
				cmdArray[count++] = 0xE0;
				cmdArray[count++] = 0xFF;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0xE0);
//				osd9616_send(0x40, 0xFF);
			} else if ( lcdBuffer[j] < (step*11) && lcdBuffer[j] >= (step*10) ) {
				cmdArray[count++] = 0xC0;
				cmdArray[count++] = 0xFF;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0xC0);
//				osd9616_send(0x40, 0xFF);
			} else if ( lcdBuffer[j] < (step*10) && lcdBuffer[j] >= (step*9) ) {
				cmdArray[count++] = 0x80;
				cmdArray[count++] = 0xFF;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x80);
//				osd9616_send(0x40, 0xFF);
			} else if ( lcdBuffer[j] < (step*9) && lcdBuffer[j] >= (step*8) ) {
				cmdArray[count++] = 0x00;
				cmdArray[count++] = 0xFF;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x00);
//				osd9616_send(0x40, 0xFF);
			} else if ( lcdBuffer[j] < (step*8) && lcdBuffer[j] >= (step*7) ) {
				cmdArray[count++] = 0x00;
				cmdArray[count++] = 0xFE;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x00);
//				osd9616_send(0x40, 0xFE);
			} else if ( lcdBuffer[j] < (step*7) && lcdBuffer[j] >= (step*6) ) {
				cmdArray[count++] = 0x00;
				cmdArray[count++] = 0xFC;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x00);
//				osd9616_send(0x40, 0xFC);
			} else if ( lcdBuffer[j] < (step*6) && lcdBuffer[j] >= (step*5) ) {
				cmdArray[count++] = 0x00;
				cmdArray[count++] = 0xF8;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x00);
//				osd9616_send(0x40, 0xF8);
			} else if ( lcdBuffer[j] < (step*5) && lcdBuffer[j] >= (step*4) ) {
				cmdArray[count++] = 0x00;
				cmdArray[count++] = 0xF0;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x00);
//				osd9616_send(0x40, 0xF0);
			} else if ( lcdBuffer[j] < (step*4) && lcdBuffer[j] >= (step*3) ) {
				cmdArray[count++] = 0x00;
				cmdArray[count++] = 0xE0;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x00);
//				osd9616_send(0x40, 0xE0);
			} else if ( lcdBuffer[j] < (step*3) && lcdBuffer[j] >= (step*2) ) {
				cmdArray[count++] = 0x00;
				cmdArray[count++] = 0xC0;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x00);
//				osd9616_send(0x40, 0xC0);
			} else if ( lcdBuffer[j] < (step*2) && lcdBuffer[j] >= (step*1) ) { // where it comes it at j = 18, count = 156
				cmdArray[count++] = 0x00;
				cmdArray[count++] = 0x80;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x00);
//				osd9616_send(0x40, 0x80);
			} else if ( lcdBuffer[j] < (step*1) && lcdBuffer[j] >= (step*0) ) {
				cmdArray[count++] = 0x00;
				cmdArray[count++] = 0x80;
				if (count >= 192) {
					count = 0;
				}
//				osd9616_send(0x40, 0x00);
//				osd9616_send(0x40, 0x80);
			}
		} // end FOr loop
		SEM_pend(&SEM_i2c, 1); // Block here for 1 tick until i2c bus is open
		osd9616_multiSend(cmdArray, 193); // cmdArray is 192 -- command is 1 value = 193
		SEM_post(&SEM_i2c); // release the i2c bus
		//TSK_yield();
		TSK_sleep(5); //Blocking,  1 tick = 1 ms
	} // End While loop
} // End TSK_lcd


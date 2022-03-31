#include <std.h>
#include "lcd.h" 				// for the lcd functions

#include <log.h>
#include <clk.h>
#include <tsk.h>
#include <gbl.h>
//#include "clkcfg.h"
#include "hellocfg.h"
#include "ezdsp5502.h"
#include "stdint.h"
#include "stdbool.h" 			// for the boolean variable type
#include "aic3204.h"
#include "ezdsp5502_mcbsp.h"
#include "csl_mcbsp.h"
#include "ezdsp5502_i2cgpio.h"
#include "csl_gpio.h"

extern void audioProcessingInit(void);
extern void ConfigureAic3204(void);

int16_t switchInterrupt = 0;
int counter = 0;
int counterTwo = 0;

// Added for Lab 3 button task
/*
 * 1 when btn is pressed
 * 0 when btn is not pressed
 */
bool btn_press1 = 0;
bool btn_press2 = 0;

void main(void)
  {
	/* Initialize BSL */
    EZDSP5502_init( );

    /* configure the Codec chip */
    ConfigureAic3204();

    /* Initialize I2S */
    EZDSP5502_MCBSP_init();

    /* Initialize LED */
    osd9616_init();

    /* Setup I2C GPIOs for Switches */
    EZDSP5502_I2CGPIO_configLine(  SW0, IN ); // Board is SW1 - toggles between encrypting using RC4 and not-encrypting
    EZDSP5502_I2CGPIO_configLine(  SW1, IN ); // Board is SW2 - toggles between no filter and LPF

    /* Setup I2C GPIOs for LED */
    EZDSP5502_I2CGPIO_configLine(  LED0, OUT );

    /* enable the interrupt with BIOS call */
    C55_enableInt(7); // reference technical manual, I2S2 tx interrupt
    C55_enableInt(6); // reference technical manual, I2S2 rx interrupt

    audioProcessingInit();

    // after main() exits the DSP/BIOS scheduler starts
}

void myIDLThread(void){


}

#if 0
Void taskFxn(Arg value_arg)
{
    LgUns prevHtime, currHtime;
    uint32_t delta;
    float ncycles;

    /* get cpu cycles per htime count */
    ncycles = CLK_cpuCyclesPerHtime();

    while(1)
    {
        TSK_sleep(1);
        LOG_printf(&trace, "task running! Time is: %d ticks", (Int)TSK_time());

        prevHtime = currHtime;
        currHtime = CLK_gethtime();

        delta = (currHtime - prevHtime) * ncycles;
        LOG_printf(&trace, "CPU cycles = 0x%x %x", (uint16_t)(delta >> 16), (uint16_t)(delta));

    }
}
#endif



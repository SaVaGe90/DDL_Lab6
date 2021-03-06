/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC11xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

/*****************************************************************************
 *   Bridge.c:  Communication Code for ARM and DE0
 *   Author: Qi
 *	 Timer interrupt to send out data at fixed interval
 *	 External interrupt to receive and control LED
 *
 *	 Interface Description-
 *   1. Pin 3-2: clk
 *   2. Pin 3-1: serial data out
 *   3. Pin 2-1: data in
 *   4. Pin 2-2: handshake
 *   5. Pin 2-0: reset
 *   6. Pin 0-7: LED
******************************************************************************/

#include "driver_config.h"
#include "target_config.h"

#include "gpio.h"
#include "timer32.h"
//#include "debug_printf.h"


/* Main Program */

/*----Provided Noisy Morse Data for You----*/
const int16_t morse[1600] = {109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100,109,145,145,109,50,-9,5,-45,-9,50,159,145,145,159,100,41,-45,-45,-9,50,109,145,195,109,50,-9,5,-45,-9,100,159,195,145,109,50,-9,-45,-45,41,50,109,145,195,109,50,41,5,5,-9,50,109,145,145,109,100,-9,-45,-45,41,50,109,195,195,159,50,-9,-45,-45,-9,50,159,145,145,109,100,-9,-45,5,41,100
};//ABC
const uint32_t length = 1600;//morse code data length

int16_t results[2000];
int16_t index = 0;


// 1ms timer interval
#define TIMER_INTERVAL (SystemCoreClock/1000 - 1)

#define GPIO2_IS ((int *)0x50028004UL)
#define GPIO2_IBE ((int *)0x50028008UL)
#define GPIO2_IEV ((int *)0x5002800CUL)
#define GPIO2_IE ((int *)0x50028010UL)
#define GPIO2_IC ((int *)0x5002801CUL)

/*	 Interface Description-
*   1. Pin 3-2: clk
*   2. Pin 3-1: serial data out
*   3. Pin 2-1: data in
*   4. Pin 2-2: handshake
*   5. Pin 2-0: reset
*   6. Pin 0-7: LED
*   7. Pin 3-0: data in clock from DE0
*   */
#define CLK_PORT 3
#define CLK 2
#define DOUT_PORT 3
#define DOUT 1
#define DIN_PORT 2
#define DIN 1
#define DINCLK_PORT 2
#define DINCLK 0
#define HNDSHK_PORT 2
#define HNDSHK 2
#define RESET_PORT 3
#define RESET 0
#define LED_PORT 0
#define LED 7

//static const int num_returned_bits = 17; // after filtering, should b
static const int data = 0x33D;
static const int num_bits = 10;
static int done_sending = 0;
static int bits_sent = 0;
static int bits_rcvd = 0;
static int handshake = 0;
static int returned_num = 0;
static long int i = 0;
static int bit_to_send = 0;
static int reset_sig = 0;
static unsigned int samples_sent = 0;

int main (void) {

/*To-do
* 1. System Initialization
*/

/*2. GPIO Initialization
 * GPIO for LED control
 * GPIOs for communication protocol: clock, handshake, reset, data input, data output
 */
	/* Initialize GPIO (sets up clock) */
	// set edge triggering
	*GPIO2_IS &= ~(0x1);
	// both edges
	*GPIO2_IBE |= (0x1);
	// active high
	//*GPIO2_IEV |= (0x1);
	// enable interrupt on pin 0 (data in clock)
	*GPIO2_IE |= (0x1);
	// Set priority of gpio2 interrupt to 0
	NVIC_SetPriority(EINT2_IRQn, 0);

	GPIOInit();

	GPIOSetDir( LED_PORT, LED, 1 );
	GPIOSetDir( CLK_PORT, CLK, 1 );
	GPIOSetDir( HNDSHK_PORT, HNDSHK, 1 );
	GPIOSetDir( RESET_PORT, RESET, 1 );
	GPIOSetDir( DOUT_PORT, DOUT, 1 );
	GPIOSetDir( DIN_PORT, DIN, 0 );
	GPIOSetDir( DINCLK_PORT, DINCLK, 0 );

	GPIOSetValue( LED_PORT, LED, 0 ); // start with LPC board LED off
	GPIOSetValue( CLK_PORT, CLK, 0 ); // start with clock low
	GPIOSetValue( HNDSHK_PORT, HNDSHK, 0 ); // start with reset low
	GPIOSetValue( RESET_PORT, RESET, 0 ); // start with reset low
	GPIOSetValue( DOUT_PORT, DOUT, 0 ); // start with clock low


/*3. Timer Initialization
 * One Timer for generating clock, handshake, data output schedule
 */
	/* init and timer */
	init_timer32( 0, TIME_INTERVAL );


/*4. Reset DE0 FIR filter
* Generate a short pulse to reset DE0
*/

	enable_timer32( 0 ); // reset will go high in timer interrupt



	while (1) {

	};                                /* Loop forever */

	return 1;
}//end of main function


/*-----Interrupt handler for external interrupt 2-----
 * config to both edges to sense the data input clock signal from DE0
 * turn LED on/off
 */

void PIOINT2_IRQHandler(void)
{
	// interrupted by data in clock (pin 0), but need to read from
	// data in (pin 1)
	int data_bit_in = (LPC_GPIO2->DATA >> 1) & 0x1;
	if(data_bit_in == 0x1)
	{
		GPIOSetValue( LED_PORT, LED, 1 );
	}
	else if(data_bit_in == 0x0)
	{
		GPIOSetValue( LED_PORT, LED, 0 );
	}
	returned_num = returned_num | (data_bit_in << bits_rcvd);
	bits_rcvd++;

	// if we've received a whole packet...
	if(bits_rcvd == 32)
	{

		if(samples_sent >= length)
		{
			// all data have been processed... finish
			for(;;);
		}

		// returned_num should have our filtered value
		if(40 <= returned_num && returned_num <= 60)
		{
			// Morse code dot --> turn off the LPC LED
			GPIOSetValue( LED_PORT, LED, 0 );
			//debug_printf("dot: %d\n", returned_num);
		}
		else if(90 <= returned_num && returned_num <= 110 )
		{
			// Morse code dash --> turn on the LPC LED
			GPIOSetValue( LED_PORT, LED, 1 );
			//debug_printf("dash: %d\n", returned_num);
		}
		else
		{
			//debug_printf("nothing: %d\n", returned_num);
		}
		//for(;;);


		// push returned num to an array for storage
		if(index < 2000)
		{
			results[index++] = returned_num;
		}
		else
		{

			for(;;);
		}

		// start sending the next datum for filtering
		bits_rcvd = 0;
		done_sending = 0;
		returned_num = 0;
	}

	// clear the interrupt on pin 0
	*GPIO2_IC |= (0x1);

} //end of external interrupt



/*-----interrupt handler for timer32_0-----
 * generate handshake
 * generate bit output clock
 * send out a sample bit by bit (every interrupt, send out one data sample)
 */

void TIMER32_0_IRQHandler(void)
{

	// only do stuff every 40 ticks
	if(!(timer32_0_counter%3))
	{
		// raise negative reset signal
		if(!reset_sig)
		{
			reset_sig = 1;
			GPIOSetValue( RESET_PORT, RESET, 1 );
		}
		// generate handshake
		if(!handshake && !done_sending)
		{
			// raise the handshake signal
			handshake = 1;
			GPIOSetValue( HNDSHK_PORT, HNDSHK, 1 );
		}
		else if(handshake && done_sending)
		{
			// lower the handshake signal
			handshake = 0;
			GPIOSetValue( HNDSHK_PORT, HNDSHK, 0 );
		}

		// --> Send out one data bit per every two interrupts
		if(bits_sent < num_bits && !(i%2) && handshake)
		{
			// still have data to send
			bit_to_send = ((morse[samples_sent] >> (9-bits_sent)) & 0x1); // get the next bit to send
			GPIOSetValue( DOUT_PORT, DOUT, bit_to_send ); // set the data out value
			bits_sent++;
		}
		if(bits_sent == num_bits)
		{
			// done sending
			done_sending = 1;
			// sent another sample
			samples_sent++;
			bits_sent = 0;
		}

		// generate bit output clock (every interrupt, flip the clock signal)
		if(!(i%2))
		{
			GPIOSetValue( CLK_PORT, CLK, 1 );
		}
		else
		{
			GPIOSetValue( CLK_PORT, CLK, 0 );
		}

		// increment i
		i++;
	}

	// increment the timer counter variable
	if ( LPC_TMR32B0->IR & 0x01 )
	{
		LPC_TMR32B0->IR = 1;				// clear interrupt flag
		timer32_0_counter++;
	}


} //end of timer interrupt


/*
 * Project1_Spring_2025.c
 *
 * Created: 24/02/2025 21:34:00
 * Author : Ciaran.MacNamee
 */ 
/* The ATMega4809 on the Arduino Every uses a CPU clock of 20 MHz  */


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

#define HALF_SEC_COUNT 9766
/* We will set the PER register to 9766, our top value
   This is because 20MHz/1024  gives TCA0 clk period = 51.2us and 51.2us*9766 = 0.5000192s */

/* Use a struct to make the association between PORTs and bits connected to the LED array more explicit */
struct LED_BITS
{
	PORT_t *LED_PORT;
	uint8_t bit_mapping;
};

struct LED_BITS LED_Array[10] = {
	{&PORTC, PIN5_bm}, {&PORTC, PIN4_bm}, {&PORTA, PIN0_bm}, {&PORTF, PIN5_bm}, {&PORTC, PIN6_bm}, {&PORTB, PIN2_bm}, {&PORTF, PIN4_bm}, {&PORTA, PIN1_bm}, {&PORTA, PIN2_bm}, {&PORTA, PIN3_bm}
};

void CLOCK_init (void);
void InitialiseLED_PORT_bits(void);
void Set_Clear_Ports(uint8_t set);
void TCA0_init_bits(void);


void CLOCK_init (void)
{
	/* Disable CLK_PER Prescaler */
	ccp_write_io( (void *) &CLKCTRL.MCLKCTRLB , (0 << CLKCTRL_PEN_bp));
	/* If set from the fuses during device programming, the CPU will now run at 20MHz (default is /6) */
}

void TCA0_init_bits(void)
{
/* This function initialises Timer/Counter TCA0 to interrupt at a period set by the clock division ratio 
	and the PER register setting */

	TCA0.SINGLE.INTCTRL = 0b00000001;		/* Counter overflow interrupt option */
	TCA0.SINGLE.CTRLB = 0b00000000;			/* Normal Mode selected - TOP value in PER register */
	TCA0.SINGLE.EVCTRL = 0b00000000;		/* TCA0 can count events from the EVENT module - disable this option */
	
	/* Now set the PER register to 9766, our top value */
	/* This is because 20MHz/1024  gives TCA0clk period = 51.2us and 51.2us*9766 = 0.5000192s */
	
	TCA0.SINGLE.PER = HALF_SEC_COUNT;		/* The TCA0 counter will overflow when it reaches the PER value */
	TCA0.SINGLE.CTRLA = 0b00001111;			/* Prescale set to /1024 and enable TCA0 (start count) */

}


void InitialiseLED_PORT_bits()
{
	PORTC.DIR = PIN6_bm | PIN5_bm | PIN4_bm;  /*(1<<6) | (1<<5) | (1<<4); 0x70;*/		/* PC4-UNO D1 (TXD1), PC5-UNO D0 (RXD1), PC6 - UNO D4  */
	PORTA.DIR = PIN3_bm | PIN2_bm | PIN1_bm | PIN0_bm; /*(1<<1) | (1<<0);   0x0f; */      /* PA1-UNO D7, PA0 - UNO D2, PA2- LED8, PA3 - LED9  */
	PORTB.DIR = PIN2_bm; /*0x04;*/		/* PB2 - UNO D5 */
	PORTF.DIR = PIN5_bm | PIN4_bm; /*(1<<5) | (1<<4);   0x30; */		/* PF5 - UNO D3, PF4 UNO D6 */
	/* Later use PIN6_bm etc */
}

/* Function to set or clear all LED port bits, 1 - set, 0 - clear */
void Set_Clear_Ports(uint8_t set) {
	
	uint8_t i;
	
	for (i = 0; i <= 9; i += 1)
	{
		if (set)
			LED_Array[i].LED_PORT->OUTSET = LED_Array[i].bit_mapping;
		else
			LED_Array[i].LED_PORT->OUTCLR = LED_Array[i].bit_mapping;
	}
}


int main(void)
{
	CLOCK_init();
	
	/* set UNO D0-D7 to all outputs, also LED8 and LED9  */
	InitialiseLED_PORT_bits();
		
	Set_Clear_Ports(0);
	
	TCA0_init_bits();
	
	/* Later add the ADC0 Initialisation here, following the project specification */
	/* Also any other necessary initialisations */
	
	sei();
	
	while (1)
	{
		/* Later add code here according to the project specification */
	}
}



ISR(TCA0_OVF_vect)
{
	
	TCA0.SINGLE.INTFLAGS = 0b00000001;   /* Writing 1 to the flag bit clears it, necessary to prevent continuous interrupts */

/* Add your Cylon Eyes code here. Note that it cannot use time delays because the time delays are set by the 
	timer overflow interrupt with the time set the PER value
	This means that you cannot use for loops here for left and right movement. You will need to develop an
	alternative technique for moving the active LED. 
*/
	
}


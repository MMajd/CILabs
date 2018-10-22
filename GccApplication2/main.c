
#include <avr/io.h>
#include <avr/interrupt.h>
#include "./UART/uart.h"

#define F_CPU 1000000UL
#define BAUD_RATE 9600

int main(void)
{
	
	static volatile uint8_t cmd_buffer[1];

	 DDRA=0b11111000; // set the first 3 pins of PORTD to be inputs to read from the push buttons
	 DDRB=0b11111111; // ensure that Pin3 in POrt B is output as this is the OC0 pin that wll produce the PWM.
	 PORTD=0b00000000; // Initialize PORTD to zeros
	 TCCR0=0b01110101; //Configure TCCR0 as explained in the article
	 TIMSK=0b00000000;
	 OCR0=255; // Set OCR0 to 255 so that the duty cycle is initially 0 and the motor is not rotating
	 
	 DDRD = 255;
	 PORTD = 0;
	 
	 volatile float duty_cycle = 0;

	/* Init UART driver. */
	UART_cfg my_uart_cfg;
	
	/* Set USART mode. */
	my_uart_cfg.UBRRL_cfg = (BAUD_RATE_VALUE)&0x00FF;
	my_uart_cfg.UBRRH_cfg = (((BAUD_RATE_VALUE)&0xFF00)>>8);
	
	my_uart_cfg.UCSRA_cfg = 0;
	my_uart_cfg.UCSRB_cfg = (1<<RXEN)  | (1<<TXEN) | (1<<TXCIE) | (1<<RXCIE);
	my_uart_cfg.UCSRC_cfg = (1<<URSEL) | (3<<UCSZ0);
	
	UART_Init(&my_uart_cfg);

	sei();

	while(1)
	{
		/* Receive the full buffer command. */
		UART_ReceivePayload(cmd_buffer, 1);
		/* Pull unitl reception is complete. */
		while(0 == UART_IsRxComplete());

		/* Parse command buffer. */
		switch(cmd_buffer[0])
		{
			case '1':
			{
			  
				duty_cycle -= 0.2;

				UART_SendPayload((uint8_t *)"1", 1);
				while (0 == UART_IsTxComplete());

		        OCR0 = (uint8_t)(255 * (1 - duty_cycle));
				DDRC  = 255;
				PORTC = OCR0;
				break;
			}
			case '2':
			{
	            duty_cycle += 0.2;

				UART_SendPayload((uint8_t *)"2", 1);
				while (0 == UART_IsTxComplete());

				OCR0 = (uint8_t)(255 * (1 - duty_cycle));
				DDRC  = 255;
				PORTC = OCR0;
				break;
			}


			default: {/* Do nothing. */}
		}

	}
	return 0;
}

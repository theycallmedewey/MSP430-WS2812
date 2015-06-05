#include <msp430.h>
#include "greenshift.h"
#include "blueshift.h"
#include "checker.h"

#define uint8_t unsigned char

#define nLEDs 70

#define RED 255,0,0
#define GREEN 0,255,0
#define BLUE 0,0,255
#define PURPLE 100,0,255
#define YELLOW 255,255,0
#define WHITE 255,255,255
#define BLACK 0,0,0
#define ORANGE 0xFF,0x66,0x00

typedef struct{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} led_t;

//led_t buffer[nLEDs];

int pattern = 0;
uint8_t bright = 16;
uint8_t hold = 0;
int hold_ctr = 0;
uint8_t pause = 0;
uint8_t unpaused = 0;

uint8_t encodeByte(uint8_t val, uint8_t byte);
void packLED(uint8_t r, uint8_t g, uint8_t b, led_t * out);
void showLEDs( const uint8_t * pattern, uint8_t start );
void whiteOut();
uint8_t brightDiv( uint8_t val, uint8_t bright );

/*
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    BCSCTL1 = CALBC1_16MHZ;
    P1SEL = BIT5 + BIT6 + BIT7;
    P1SEL2 = BIT5 + BIT6 + BIT7;
    UCB0CTL1 = UCSWRST;
    UCB0CTL1 |= UCSSEL_2;							// SMCLK
    UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC;		// 3-pin, 8-bit SPI master
    UCB0BR0 = 6;
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;							// **Initialize USCI state machine**

    P2DIR = 0x00;
    P2REN |= BIT0 + BIT1 + BIT2;
    P2OUT |= BIT0 + BIT1;
    P2OUT &= ~BIT2;
    P2IE |= BIT0;
    P2IES &= ~BIT0;
    P2IFG = 0x00;

    uint8_t i;

    while(1){
    	for( i = 0; i < nLEDs; ++i){
    		if( pause == 0 ){
				if( pattern == 0 ){
					showLEDs( &greenshift[0], i );
				}else if( pattern == 1 ){
					showLEDs( &blueshift[0], i );
				}else{
					showLEDs( &checker[0], i );
				}
				__delay_cycles( 100000 );
    		}

			if( (P2IN & BIT2) == BIT2 ){
				if( hold == 0 ){
					if( pause == 0 ){
						//First push
						hold = 1;
					}else{
						//Push while paused
						pause = 0;
						unpaused = 1;
						hold = 1;
						hold_ctr = 0;
						__disable_interrupt();
					}
					__delay_cycles(1000);
				}else{
					//Hold down
					hold_ctr++;
					if( hold_ctr > 100 ){
						pause = 1;
						__delay_cycles(100);
						whiteOut();
						__enable_interrupt();
					}
				}
			}else{
				//Button released
				if( pause == 0 && hold == 1 && unpaused == 0 ){
					//End of button press, not paused
					pattern++;
					if( pattern > 2 ){
						pattern = 0;
					}
				}else if( unpaused == 1 ){
					unpaused = 0;
				}
				hold = 0;
				hold_ctr = 0;
			}
    	}
    }

	return 0;
}

void packLED(uint8_t r, uint8_t g, uint8_t b, led_t * out){
	out->red = r;
	out->green = g;
	out->blue = b;
}

uint8_t encodeByte( uint8_t val, uint8_t byte ){
	register uint8_t shift = val;
	register uint8_t out = 0;

	if( byte == 0 ){
		shift = shift;
	}else if( byte == 1 ){
		shift = shift << 2;
	}else if( byte == 2 ){
		shift = shift << 4;
	}else{
		shift = shift << 6;
	}

	if( (shift & 0x80) == BIT7 ){
		out |= 0xE0;
	}else{
		out |= 0x80;
	}

	if( (shift & 0x40) == BIT6 ){
		out |= 0x0E;
	}else{
		out |= 0x08;
	}

	return out;
}

void showLEDs( const uint8_t * pattern, uint8_t start ){
	uint8_t i = start;
	uint8_t k,tx,val;

	do{
		val = pattern[ (uint8_t)(3*i) ];
		val = brightDiv( val, bright );
		for( k = 0; k < 4; ++k ){
			tx = encodeByte( val, k );
			while ( !(IFG2 & UCB0TXIFG) );
			UCB0TXBUF = tx;
		}

		val = pattern[ (uint8_t)(3*i + 1) ];
		val = brightDiv( val, bright );
		for( k = 0; k < 4; ++k ){
			tx = encodeByte( val, k );
			while ( !(IFG2 & UCB0TXIFG) );
			UCB0TXBUF = tx;
		}

		val = pattern[ (uint8_t)(3*i + 2) ];
		val = brightDiv( val, bright );
		for( k = 0; k < 4; ++k ){
			tx = encodeByte( val, k );
			while ( !(IFG2 & UCB0TXIFG) );
			UCB0TXBUF = tx;
		}

		++i;

		if( i >= nLEDs ){
			i = 0;
		}
	}while( i != start );
}

void whiteOut( ){
	int i,k;
	uint8_t tx, val;

	for( i = nLEDs; i > 0; --i ){
		val = brightDiv( 0xFF, bright );
		for( k = 0; k < 4; ++k ){
			tx = encodeByte( val, k );
			while ( !(IFG2 & UCB0TXIFG) );
			UCB0TXBUF = tx;
		}

		for( k = 0; k < 4; ++k ){
			tx = encodeByte( val, k );
			while ( !(IFG2 & UCB0TXIFG) );
			UCB0TXBUF = tx;
		}

		for( k = 0; k < 4; ++k ){
			tx = encodeByte( val, k );
			while ( !(IFG2 & UCB0TXIFG) );
			UCB0TXBUF = tx;
		}
	}
}

#pragma vector=PORT2_VECTOR
__interrupt void    Port_2(void)
{
	uint8_t state_prev;
	uint8_t state = 0x00;

	while( state != 0x03 ){
		state_prev = state;
		state = P2IN & 0x03;
	}

	if( state_prev == 0x01 ){
		//CCW
		if( bright != 0 ){
			--bright;
		}
	}else if( state_prev == 0x02 ){
		//CW
		if( bright < 16 ){
			++bright;
		}
	}

	//__disable_interrupt();
	whiteOut();
	//__enable_interrupt();
	__delay_cycles(100);

	P2IFG = 0x00;
}

uint8_t brightDiv( uint8_t val, uint8_t bright ){
	if( bright < 1 ){
		return 0;
	}else if( bright >= 16 ){
		return val;
	}

	switch( bright ){
	case 1:		return (val >> 4);
	case 2: 	return (val >> 3);
	case 3: 	return (val >> 3) + (val >> 4);
	case 4: 	return (val >> 2);
	case 5: 	return (val >> 2) + (val >> 4);
	case 6:		return (val >> 2) + (val >> 3);
	case 7:		return (val >> 1) - (val >> 4);
	case 8:		return (val >> 1);
	case 9:		return (val >> 1) + (val >> 4);
	case 10:	return (val >> 1) + (val >> 3);
	case 11:	return val - (val >> 2) - (val >> 4);
	case 12:	return val - (val >> 2);
	case 13:	return val - (val >> 3) - (val >> 4);
	case 14:	return val - (val >> 3);
	case 15:	return val - (val >> 4);
	default:	return val;
	}
}

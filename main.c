#include <msp430.h>

#define uint8_t unsigned char

#define nLEDs 30

#define ONE 110
#define ZERO 100

#define RED 255,0,0
#define GREEN 0,255,0
#define BLUE 0,0,255
#define PURPLE 255,0,255
#define YELLOW 255,255,0
#define WHITE 255,255,255
#define BLACK 0,0,0

typedef struct{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} led_t;

led_t buffer[nLEDs];

uint8_t encodeByte(uint8_t val);
void packLED(uint8_t r, uint8_t g, uint8_t b, led_t * out);

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

    int i, k;
    int j = 0;

    uint8_t tx;

    while(1){
		for( i = 0; i < j; ++i){
			packLED(BLUE,&buffer[i]);
		}
		packLED(WHITE,&buffer[j]);
		for( i = j+1; i < nLEDs; ++i){
			packLED(GREEN,&buffer[i]);
		}

		j = (++j)%nLEDs;

		for( i = 0; i < nLEDs; ++i ){
			for( k = 0; k < 3; ++k ){
				tx = encodeByte( buffer[i].green );
				while ( !(IFG2 & UCB0TXIFG) );
				UCB0TXBUF = tx;
			}
			for( k = 0; k < 3; ++k ){
				tx = encodeByte( buffer[i].red );
				while ( !(IFG2 & UCB0TXIFG) );
				UCB0TXBUF = tx;
			}
			for( k = 0; k < 3; ++k ){
				tx = encodeByte( buffer[i].blue );
				while ( !(IFG2 & UCB0TXIFG) );
				UCB0TXBUF = tx;
			}
		}
		__delay_cycles(100000);
    }

	return 0;
}

void packLED(uint8_t r, uint8_t g, uint8_t b, led_t * out){
	out->red = r;
	out->green = g;
	out->blue = b;
}

uint8_t encodeByte(uint8_t val){
	static uint8_t i = 0;
	uint8_t out;

	if(i == 0){
		out = 0x92;
		out |= (val >> 1) & BIT6;
		out |= (val >> 3) & BIT3;
		out |= (val >> 5) & BIT0;
		i = 1;
	}else if(i == 1){
		out = 0x49;
		out |= (val >> 1) & BIT2;
		out |= (val << 1) & BIT5;
		i = 2;
	}else{
		out = 0x24;
		out |= (val << 1) & BIT1;
		out |= (val << 3) & BIT4;
		out |= (val << 5) & BIT7;
		i = 0;
	}

	return out;
}

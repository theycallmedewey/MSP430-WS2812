#include "ws2812.h"

void ws2812_init()
{
    BCSCTL1 = CALBC1_16MHZ;
    P1SEL = BIT5 + BIT6 + BIT7;
    P1SEL2 = BIT5 + BIT6 + BIT7;
    UCB0CTL1 = UCSWRST;
    UCB0CTL1 |= UCSSEL_2;							// SMCLK
    UCB0CTL0 = UCCKPH + UCMSB + UCMST + UCSYNC;		// 3-pin, 8-bit SPI master
    UCB0BR0 = 6;
    UCB0BR1 = 0;
    UCB0CTL1 &= ~UCSWRST;							// **Initialize USCI state machine**

	ws2812_global_brightness = 16;
}

void ws2812_set_brightness( uint8_t bright )
{
	ws2812_global_brightness = bright;
}

void ws2812_increase_brightness()
{
	if( ws2812_global_brightness < 16 )
	{
		++ws2812_global_brightness;
	}
}

void ws2812_decrease_brightness()
{
	if( ws2812_global_brightness > 0 )
	{
		--ws2812_global_brightness;
	}
}

void ws2812_pack_LED(  uint8_t r,  uint8_t g,  uint8_t b, led_t* out )
{
	out->red = r;
	out->green = g;
	out->blue = b;
}

uint8_t ws2812_encode_byte(  uint8_t val,  uint8_t byte)
{
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

void ws2812_show_LEDs( const uint8_t* pattern, uint8_t start)
{
	uint8_t i = start;
	uint8_t k,tx,val;

	do{
		val = pattern[ (uint8_t)(3*i) ];
		val = ws2812_bright_div( val, ws2812_global_brightness );
		for( k = 0; k < 4; ++k ){
			tx = ws2812_encode_byte( val, k );
			while ( !(IFG2 & UCB0TXIFG) );
			UCB0TXBUF = tx;
		}

		val = pattern[ (uint8_t)(3*i + 1) ];
		val = ws2812_bright_div( val, ws2812_global_brightness );
		for( k = 0; k < 4; ++k ){
			tx = ws2812_encode_byte( val, k );
			while ( !(IFG2 & UCB0TXIFG) );
			UCB0TXBUF = tx;
		}

		val = pattern[ (uint8_t)(3*i + 2) ];
		val = ws2812_bright_div( val, ws2812_global_brightness );
		for( k = 0; k < 4; ++k ){
			tx = ws2812_encode_byte( val, k );
			while ( !(IFG2 & UCB0TXIFG) );
			UCB0TXBUF = tx;
		}

		++i;

		if( i >= nLEDs ){
			i = 0;
		}
	}while( i != start );
}

uint8_t ws2812_bright_div(  uint8_t val,  uint8_t bright )
{
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

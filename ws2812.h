/*
 * ws2812.h
 *
 *  Created on: Oct 25, 2015
 *      Author: Dewey
 */

#ifndef WS2812_H_
#define WS2812_H_

#include <msp430.h>
#include "stdtypes.h"

#define nLEDs 70

#define RED 255,0,0
#define GREEN 0,255,0
#define BLUE 0,0,255
#define PURPLE 100,0,255
#define YELLOW 255,255,0
#define WHITE 255,255,255
#define BLACK 0,0,0
#define ORANGE 0xFF,0x66,0x00

uint8_t ws2812_global_brightness;

typedef struct{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} led_t;

void ws2812_init();
void ws2812_set_brightness( uint8_t bright );
void ws2812_increase_brightness();
void ws2812_decrease_brightness();
uint8_t ws2812_encode_byte( uint8_t val, uint8_t byte);
void ws2812_show_LEDs( const uint8_t* pattern, uint8_t start);
uint8_t ws2812_bright_div( uint8_t val, uint8_t bright);
void ws2812_pack_LED( uint8_t r, uint8_t g, uint8_t b, led_t* out );

#endif /* WS2812_H_ */

// PRONK Controller
// 
// Trinket DEMO controller for Large LED Letters for PRONK
// 
// Controller DEMOs commands and sends them via I2C to individual letter controller
//
// Kelly Egan 2017
// Ocean State Maker Mill
// http://oceanstatemakermill.org
//

#include <TinyWireM.h>

#define NUM_LETTERS 5

#define P_ADDRESS 1
#define R_ADDRESS 2
#define O_ADDRESS 3
#define N_ADDRESS 4
#define K_ADDRESS 5

#define RED 0xFF0000
#define ORANGE 0xFF8000
#define YELLOW 0xFFFF00
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define PINK 0xFF4040
#define PURPLE 0x7000DD
#define OSMM_TEAL 0x00FF80

unsigned long colors[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PINK, PURPLE, OSMM_TEAL};

void setup() {
    TinyWireM.begin();
}

void loop() {
    int index = random(8);
    unsigned long color = colors[index];
    byte animationCode = (animationCode + 1) % 5;

    animateLetter( P_ADDRESS, color, animationCode );
    
    delay(10000);
}

/**
 *  Send animation command to individual letter controller
 */
void animateLetter( int address, unsigned long color, int animationCode ) {
    TinyWireM.beginTransmission(address);

    byte r = (color >> 16) & 0xFF;  
    byte g = (color >> 8) & 0xFF;   
    byte b = color & 0xFF;   
  
	TinyWireM.send(animationCode);
	TinyWireM.send(r);
	TinyWireM.send(g);
	TinyWireM.send(b);

    TinyWireM.endTransmission();    
}

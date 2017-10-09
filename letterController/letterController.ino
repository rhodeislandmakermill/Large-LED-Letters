// Letter Controller
// 
// Metro mini controller for individual Large LED Letter
// 
// Controller receives commands from Electron via I2C and animates LEDs
//
// Kelly Egan 2017
// Ocean State Maker Mill
// http://oceanstatemakermill.org
//

#include "FastLED.h"
#include <Wire.h>

//LETTER P
#define ADDRESS 10
#define NUM_LEDS 249
//LETTER R
//#define ADDRESS 11
//#define NUM_LEDS 315
//LETTER O
//#define ADDRESS 12
//#define NUM_LEDS 294
//LETTER N
//#define ADDRESS 13
//#define NUM_LEDS 343
//LETTER K
//#define ADDRESS 14
//#define NUM_LEDS 200

#define PIXEL_PIN 3
#define BRIGHTNESS 95

bool waiting;
unsigned long lastCommandTime;  //Last time received a message from the main controller

//int shuffledLEDs[NUM_LEDS];

int animationCode;
CRGB leds[NUM_LEDS];
CRGB currentColor;
CRGB nextColor;
unsigned long duration;

volatile bool startAnimation;

void setup() {  
	//I2C setup	
	pinMode( SDA, INPUT );
	pinMode( SCL, INPUT );
	digitalWrite( SDA, HIGH );
	digitalWrite( SCL, HIGH );
	Wire.begin(ADDRESS);              // join i2c bus with address #4
	Wire.onReceive(receiveCommand);   // register receive event
	Wire.onRequest(requestStatus);    // register request event
	TWAR = (ADDRESS << 1) | 1;     // Enable broadcast messages to be received

	//Fast LED setup
	allBlack();
	FastLED.addLeds<NEOPIXEL, PIXEL_PIN>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
	FastLED.setTemperature(HighNoonSun);
	FastLED.setBrightness(BRIGHTNESS);
	FastLED.show();
		
	//For random change
//	for(int i = 0; i < NUM_LEDS; i++) {
//		shuffledLEDs[i] = i;
//	}
	
	waiting = true;
	lastCommandTime = millis();
	
	//shuffle(shuffledLEDs, NUM_LEDS);
	animationCode = 0;
	
	startAnimation = false;
	animationCode = 0;
	currentColor = CRGB::Black;
	nextColor = CRGB::White;
	duration = 4000;
	
	fadeBetween( currentColor, nextColor, 3000 );
	currentColor = nextColor;
}


void loop() {
	if( startAnimation ) {
		runAnimation( animationCode, currentColor, nextColor, duration );
		currentColor = nextColor;
		startAnimation = false;
	} else {
		fadeToward( currentColor, 64 );
		FastLED.show();
		delay(80);
	}

	// If haven't heard anything from main controller 
	// in 2 minutes set pixels to white.
	if( millis() - lastCommandTime > 60000 * 2 && waiting ) {
		nextColor = CRGB::White;
		runAnimation( random(5), currentColor, nextColor, duration );
		currentColor = nextColor;
		waiting = false;
	}
}


/**
 *  Run an animation and transition to a new color
 */
void runAnimation( byte animation, CRGB current, CRGB next, unsigned long duration ) {
	switch (animation) {
	case 6:
		juggle( duration );
		break;
    case 5: 
    	rainbow( duration );
    	break;
	case 4:
		sparkle( current, next, duration );
		break;
	case 3: 
		dance( current, next, duration );
		break;
	case 2:
//		randomOn( current, next, duration );
//		break;
	case 1:
		slide( current, next, duration );
		break;
	default: 
		fadeBetween( current, next, duration );
	}
}


/**
 *  Receive commands from master controller
 */
void receiveCommand(int howMany) {
	animationCode = Wire.read();    // Animation command
	byte r = Wire.read();           // Red channel
	byte g = Wire.read();           // Green channel
	byte b = Wire.read();           // Blue channel
	duration = 1000 * Wire.read(); // Animation duration in seconds
	
	//Eat up extra bytes
	while (0 < Wire.available()) {
		Wire.read();
	}
	
	nextColor = CRGB(r,g,b);
	waiting = true;
	lastCommandTime  = millis();
	startAnimation = true;
}

/**
 *  Request of status from master controller
 */
void requestStatus(int howMany) {
	if(startAnimation == true) {
		Wire.write(0);
	} else {
		Wire.write(128);
	}
}

/**
 *  Turn everything off
 */
void allBlack() { 
	for(int i = 0; i < NUM_LEDS; i++) { 
		leds[i] = CRGB::Black;
	} 
}

/**
 *  Juggle
 *  From FastLED demo code.
 */
void juggle( unsigned long milliseconds  ) {
	int frameLength = 30;
	int totalFrames = milliseconds / frameLength;
	for( int frame = 0; frame < totalFrames; frame++ ) {
		// eight colored dots, weaving in and out of sync with each other
		fadeToBlackBy( leds, NUM_LEDS, 20);
		byte dothue = 0;
		for( int i = 0; i < 8; i++) {
			leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
			dothue += 32;
		}
		FastLED.show();
		delay(frameLength);
	}
}


/**
 *  Pulsing rainbow
 */
void rainbow( unsigned long milliseconds ) {
	int frameLength = 30;
	int totalFrames = milliseconds / frameLength;
	int hueOffset = 3;
	int hue = 0;
	for( int frame = 0; frame < totalFrames; frame++ ) {
		hue += hueOffset;
		fill_rainbow( leds, NUM_LEDS, hue, 255 / NUM_LEDS );
		FastLED.show();
		delay(frameLength);
	}	
}

/**
 *  Fade to endColor while randomly changing some pixels white.
 */
void sparkle( CRGB startColor, CRGB endColor, unsigned long milliseconds ) {
	int frameLength = 30;
	int totalFrames = milliseconds / frameLength;
	for( int frame = 0; frame < totalFrames; frame++ ) {
		fadeToward( endColor, 32 );
		
		for( int i = 0; i < 3; i++) {
			leds[ random16(NUM_LEDS) ] = CRGB::White;
		}
		
		FastLED.show();
		delay(frameLength);
	}
}


/**
 *  Randomly change from current color to new color until all colors are changed
 *  Random numbers are stored for efficient... necessary?
 */
//void randomOn( CRGB startColor, CRGB endColor, unsigned long milliseconds ) {
//	int totalFrames = NUM_LEDS;
//	int frameLength = milliseconds / totalFrames;
//	for( int frame = 0; frame < totalFrames; frame++ ) {
//		leds[ shuffledLEDs[frame] ] = endColor;
//		FastLED.show();
//		delay(frameLength);
//	}
//	shuffle(shuffledLEDs, NUM_LEDS);
//}


/**
 *  Dance between start and end color
 */
void dance( CRGB startColor, CRGB endColor, unsigned long milliseconds ) {
	int frameLength = 60;
	int totalFrames = milliseconds / frameLength;
	
	for( int frame = 0; frame < totalFrames; frame++ ) {
		for(int i = 0; i < NUM_LEDS; i++) {
			if(((i / 8) % 2 == (frame / 8) % 2) ) {
				leds[i] = startColor;
			} else {
				lerpColor( leds[i], endColor, 64, &leds[i] );
			}
		}
		FastLED.show();
		delay(frameLength);
	}	
	FastLED.show();
}


/**
 *  Slide transition
 */
void slide( CRGB startColor, CRGB endColor, unsigned long milliseconds ) {
  int frameLength = 60;
  int totalFrames = milliseconds / frameLength;
  
  for( int frame = 0; frame < totalFrames; frame++ ) {
    for(int i = 0; i < NUM_LEDS; i++) {
      if((i % 10 == frame % 10) ) {
        leds[i] = startColor;
      } else {
        //leds[i] = endColor; 
        lerpColor( leds[i], endColor, 32, &leds[i] );
      }
    }
    FastLED.show();
    delay(frameLength);
  }

  FastLED.show();
}


/**
 *  Fade between two colors in a given amount of milliseconds.
 */
void fadeBetween( CRGB startColor, CRGB endColor, unsigned long milliseconds ) {
	int totalFrames = 255;
	int frameLength = milliseconds / totalFrames;
	for(int frame = 0; frame < 255; frame++) {
		for(int i = 0; i < NUM_LEDS; i++) {
			leds[i] = lerpColor( startColor, endColor, frame);
		}    
		FastLED.show();
		delay(frameLength);
	} 
}


/**
 *  Fade from the current color towards and end color by a fraction: x / 255
 */
void fadeToward( CRGB endColor, int fraction ) {
	for(int i = 0; i < NUM_LEDS; i++) {
		lerpColor( leds[i], endColor, fraction, &leds[i] );
	}
}


/**
 *  Get a color blend by fraction between a start color and an end color
 *  and return the mixed color
 */
CRGB lerpColor( CRGB startColor, CRGB endColor, byte fraction ) {
	CRGB pixel;
	lerpColor( startColor, endColor, fraction, &pixel );
	return pixel;
}


/**
 *  Get a color blend by fraction between a start color and an end color
 *  and assign the mix to pixel
 */
void lerpColor( CRGB startColor, CRGB endColor, byte fraction, CRGB* pixel ) {
	pixel->r = lerp8by8( startColor.r, endColor.r, fraction );
	pixel->g = lerp8by8( startColor.g, endColor.g, fraction );
	pixel->b = lerp8by8( startColor.b, endColor.b, fraction );
}


/**
 *  Shuffle an array
 */
//void shuffle(int *a, int len) {
//	int m = len;
//	int index, temp;
//	
//	while(m > 0) {
//		index = random( m-- );
//		temp = a[m];
//		a[m] = a[index];
//		a[index] = temp;
//	}  
//}


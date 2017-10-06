// PRONK Controller
//
// Electron controller for Large LED Letters for PRONK
//
// Controller receives commands from web and send them via I2C to individual letter controllers
//
// Kelly Egan 2017
// Ocean State Maker Mill
// http://oceanstatemakermill.org
//

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

#define DEMO_MODE 0
#define TWITTER_MODE 1

unsigned long colors[] = {RED, ORANGE, YELLOW, GREEN, BLUE, PINK, PURPLE, OSMM_TEAL};
int mode;
int animationCode;


void setup() {
    mode = DEMO_MODE;
    Wire.begin();
}

void loop() {
    if( mode == TWITTER_MODE ) {

    } else {
       demoMode(10000);
    }
}

/**
 *  Execute sign animations
 *  colors[] - array gives color for each letter.
 *  animationCode - a single animation for all letters.
 *  pause - sets delay between animating each letter.
 */
void animateSign( unsigned long colors[], int animationCode, int pause ) {
  animateLetter( P_ADDRESS, colors[0], animationCode );
  if( pause > 0 ) { delay(pause); }
  animateLetter( R_ADDRESS, colors[1], animationCode );
  if( pause > 0 ) { delay(pause); }
  animateLetter( O_ADDRESS, colors[2], animationCode );
  if( pause > 0 ) { delay(pause); }
  animateLetter( N_ADDRESS, colors[3], animationCode );
  if( pause > 0 ) { delay(pause); }
  animateLetter( K_ADDRESS, colors[4], animationCode );
}

/**
 *  Demo mode, rotates through colors and animations
 *  Pause sets the time until next animation is run
 */
void demoMode(int pause) {
    int index = random(8);
    unsigned long color = colors[index];
    animationCode = (animationCode + 1) % 5;

    unsigned long colors[] = {color, color, color, color, color};
    animateSign( colors, animationCode, 0);

    delay(pause);
}

/**
 *  Send animation command to individual letter controller
 */
void animateLetter( int address, unsigned long color, int animationCode ) {
    Wire.beginTransmission(address);

    byte r = (color >> 16) & 0xFF;
    byte g = (color >> 8) & 0xFF;
    byte b = color & 0xFF;

    Wire.write(animationCode);
    Wire.write(r);
    Wire.write(g);
    Wire.write(b);

    Wire.endTransmission();
}

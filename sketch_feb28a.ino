#include <FastLED.h>

#define PLAYER_RESET_LED 12
#define PLAYER_ONE_LED 11
#define PLAYER_TWO_LED 10
#define PLAYER_THREE_LED 9
#define PLAYER_FOUR_LED 8

#define PLAYER_RESET_BUTTON 6
#define PLAYER_ONE_BUTTON 5
#define PLAYER_TWO_BUTTON 4
#define PLAYER_THREE_BUTTON 3
#define PLAYER_FOUR_BUTTON 2

#define RESET_BUTTON 13

#define NUM_LEDS 8

// Each buzzer is part of this 2d array
CRGB leds[5][NUM_LEDS];

int player_podium[4] = {0, 0, 0, 0};  // Keeps track of the place of each buzzer
int players_answered = 0;

void setup() {
  FastLED.addLeds<NEOPIXEL, PLAYER_RESET_LED>(leds[0], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, PLAYER_ONE_LED>(leds[1], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, PLAYER_TWO_LED>(leds[2], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, PLAYER_THREE_LED>(leds[3], NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, PLAYER_FOUR_LED>(leds[4], NUM_LEDS);

  
  pinMode(PLAYER_ONE_BUTTON, INPUT_PULLUP);
  pinMode(PLAYER_TWO_BUTTON, INPUT_PULLUP);
  pinMode(PLAYER_THREE_BUTTON, INPUT_PULLUP);
  pinMode(PLAYER_FOUR_BUTTON, INPUT_PULLUP);
  pinMode(PLAYER_RESET_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);

  Serial.begin(9600);
  Serial.println("reset");
}

void loop() {
  Serial.println(players_answered);
  if (digitalRead(RESET_BUTTON) == LOW || digitalRead(PLAYER_RESET_BUTTON) == LOW) {
    players_answered = 0;

    // Reset podium
    for (int i=0; i<=4; i++) {
      player_podium[i] = 0;
    }
    
  } else {
    check_buzzers();
    color_buzzers();
  }

  // Sets last player to 4th place once 3rd place is decided
  if (players_answered == 3) {
    for (int i=0; i<4; i++) {
      if (player_podium[i] == 0) {
        player_podium[i] = 4;
      }
    }
  }
    
  // Set reset buzzer
  if (players_answered == 0){
    fill_solid(leds[0], NUM_LEDS, CRGB(0x440033));
  } else {
    fill_solid(leds[0], NUM_LEDS, CRGB::Red);
  }

  FastLED.show();
}

// Checks to see which button is pressed
void check_buzzers () {
  if (digitalRead(PLAYER_ONE_BUTTON) == LOW  && player_podium[0] == 0) {
    player_podium[0] = players_answered + 1;
    players_answered += 1;
  } else if (digitalRead(PLAYER_TWO_BUTTON) == LOW  && player_podium[1] == 0) {
    player_podium[1] = players_answered + 1;
    players_answered += 1;
  } else if (digitalRead(PLAYER_THREE_BUTTON) == LOW  && player_podium[2] == 0) {
    player_podium[2] = players_answered + 1;
    players_answered += 1;
  } else if (digitalRead(PLAYER_FOUR_BUTTON) == LOW  && player_podium[3] == 0) {
    player_podium[3] = players_answered + 1;
    players_answered += 1;
  }
}

// Sets the winning buzzer to green and losing buzzers to red
void color_buzzers() {
  for (int player_num=1; player_num<=4; player_num++) {
    int place = player_podium[player_num - 1];
    switch (place) {
      case 1:
        strobe_green(player_num);
        break;
      case 2:
        fill_solid(leds[player_num], NUM_LEDS, CRGB::Green);
        break;
      case 3:
        fill_solid(leds[player_num], NUM_LEDS, CRGB(0xAA2200)); // Orange
        break;
      case 4:
        fill_solid(leds[player_num], NUM_LEDS, CRGB::Red);
        break;
      default:
        rainbow(player_num);
    }
  }
  
}

void strobe_green(int player_num) {
  static uint16_t pLastMilis = 0;
  uint16_t ms = millis();
  uint8_t strobe_delay = 150;

  if ((ms - pLastMilis) <= strobe_delay) {
    fill_solid(leds[player_num], NUM_LEDS, CRGB::Green);
  } else if ((ms - pLastMilis) <= strobe_delay*2){
    fill_solid(leds[player_num], NUM_LEDS, CRGB::Black);
  } else {
    pLastMilis = ms;
  }
}

// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void rainbow(int player_num) 
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;
 
  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5,9);
  uint16_t brightnesstheta16 = sPseudotime;
  
  for( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    
    CRGB newcolor = CHSV( hue8, sat8, bri8);
    
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS-1) - pixelnumber;
    
    nblend( leds[player_num][pixelnumber], newcolor, 64);
  }
}

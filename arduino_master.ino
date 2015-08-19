//March 2015 
//OMSI  - Handwashing

//initially developed for the Teensy
//main code runs on Arduino, Wave Audio player runs on teensy


// - 6/8/2015, added pin D17 for output to teensy wave player

#include "HT1632.h"

#define DATA 3
#define WR   4
#define CS   5

HT1632LEDMatrix matrix = HT1632LEDMatrix(DATA, WR, CS);

unsigned char germsAttractor[30];

unsigned char germs[104][4]={
{2,0,2},
{3,1,179,180},
{2,3,4},
{1,5},
{3,6,7,51},
{1,8},
{2,9,10},
{2,11,52},
{3,12,18,53},
{2,13,14},
{3,15,16,17},
{3,19,20,54},
{3,21,22,55},
{1,23},
{1,24},
{2,25,26},
{3,27,28,29},
{1,30},
{1,31},
{1,32},
{1,33},
{3,34,35,36},
{3,37,38,40},
{3,39,41,42},
{2,43,44},
{3,45,46,47},
{1,48},
{1,49},
{1,50},
{2,56,57},
{1,58},
{2,59,60},
{3,61,62,63},
{1,64},
{1,65},
{1,66},
{1,67},
{1,68},
{2,69,70},
{1,71},
{2,72,73},
{1,74},
{2,75,76},
{1,77},
{1,78},
{3,79,80,81},
{1,82},
{2,83,84},
{1,85},
{2,86,87},
{2,88,89},
{1,90},
{1,91},
{2,92,93},
{1,94},
{2,95,96},
{2,97,98},
{2,99,100},
{1,101},
{1,102},
{2,103,104},
{2,105,106},
{1,107},
{1,108},
{1,109},
{3,110,111,112},
{1,113},
{1,114},
{1,115},
{2,116,117},
{2,118,119},
{2,120,121},
{1,122},
{1,123},
{1,124},
{1,125},
{1,126},
{1,127},
{3,128,129,130},
{1,131},
{2,132,133},
{3,134,135,136},
{1,137},
{2,138,139},
{1,140},
{2,141,142},
{3,143,144,145},
{1,146},
{2,147,148},
{1,149},
{1,150},
{3,151,152,153},
{2,154,155},
{1,156},
{2,157,158},
{3,159,160,161},
{2,162,163},
{3,164,165,166},
{3,167,168,169},
{1,170},
{1,171},
{2,172,173},
{3,174,175,176},
{2,177,178},
};

#include <Adafruit_NeoPixel.h>
#define PIN 8
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(31, PIN, NEO_GRB + NEO_KHZ800);
int numberOfGreenLeds = 0;

#include "Timer.h"
Timer t;
int handTime = 21;
 int hand = 3;
 

#include "Adafruit_WS2801.h"
#include "SPI.h" // Comment out this line if using Trinket or Gemma


uint8_t dataPin  = 9;    // Yellow wire on Adafruit Pixels
uint8_t clockPin = 10;    // Green wire on Adafruit Pixels
Adafruit_WS2801 strip = Adafruit_WS2801(181, dataPin, clockPin);

const int faucet = A0;  // Analog input pin that the potentiometer is attached to

int sensorValue = 0;        // value read from the pot

int handPresent = 0;


unsigned long currentMillis = 0;

long chaseTime = 0;  //theater chase attractor lights
int writeOn = 0;  //flag so that theater chase only writes once
int lightCycle = 0;  //color wheel cycle
int q = 0;   //theater chase cycle through 3 lights

long attractorModeDelay = 0;
long attractorModeTimer = 0;
int attractorCycle = 0;

long handPresentTime = 0;  //keeps track of how long a hand has been present... if longer than 2 minutes.. reverts to not present

int greenLedMarker = 0;
int led1 = 0;
int led2 = 0;
int led3 = 0;
int led4 = 0;
int led5 = 0;


void setup() {  //------------------------------------------SETUP

pinMode(17, OUTPUT);  //goes to Teensy Wave Player, LOW starts sound
digitalWrite(17, HIGH);
//delay(6000);

  matrix.begin(HT1632_COMMON_16NMOS);  
      matrix.clearScreen();
      
  strip.begin();
  strip.show();
  strip2.begin();
  strip2.show();
  
  Serial.begin(9600); 


 //int tickEvent4 = t.every(500, printHand);   //trouble shooting
 int tickEvent = t.every(1000, countSeconds);  //counts down from 20 seconds on the Matrix
 int tickEvent2 = t.every(50, removeLeds);     //removes LED germs from the hand
 //int tickEvent3 = t.every(100, greenLeds); 
  
 pinMode(14, OUTPUT); 
 
pinMode(faucet, INPUT);
digitalWrite(faucet, INPUT_PULLUP);
}



void loop() {  //-----------------------------------------------MAIN LOOP
  
currentMillis = millis();

t.update();
    
sensorValue = analogRead(faucet);            

if(sensorValue < 400) {   //--------------------------FAUCET sensor activated
  //Serial.print("Sensor Value = " );                       
  //Serial.println(sensorValue);      
  if(handPresent == 0){
  handPresent = 1;
  handPresentTime = currentMillis;}
  else {
    handPresent = 0;}
    
  delay(200); //so it doesn't count twice
}



if(currentMillis - handPresentTime > 120000 && handPresent == 1){  //if the sensor value thinks the hand is still present after 2 minutes then revert to hand not present
  handPresent = 0;
  hand = 3; //go into attractor mode  
  //Serial.println("reverted to hand not present");
}
  
 //-----------------------------------------------NEW HAND SENSED  
if(hand == 3 && handPresent == 1){ 
  hand = 1;
  
  colorWipe2(strip2.Color(255, 0, 0), 50); // ----- neopixel ring red
  
  greenLedMarker = 30;                   // ------ start with LED # 30
  led1 = 200;
  led2 = 45;
  led5 = 0;

  led3 = 255;
  
  colorWipeWs2();  //-----------------------------  Populate all germ LEDs on hand
  
  Serial.print("hand is washing. hand indicator was: " );                       
  Serial.println(hand);
  handTime = 21;
}

//-----------------------------------HAND IS WASHING
if(hand == 1 && handPresent == 1){ 
  greenLeds();
}

 //--------------------------------------HAND HAS BEEN REMOVED EARLY
if(hand == 1 && handPresent == 0){ 
Serial.print("hands stopped washing. hand indicator was: = " );                       
Serial.println(hand);
 
matrix.clearScreen(); 
matrix.setTextSize(1);    // size 1 == 8 pixels high
matrix.setTextColor(1);   // 'lit' LEDs
matrix.setCursor(1, 5);   // start at top left, with one pixel of spacing
matrix.print("oops");
matrix.writeScreen();  

hand = 3; //go into attractor mode
attractorModeDelay = currentMillis;

digitalWrite(17, HIGH);  //stop playing sound
}
 
 //--------------------------------------HAND successfully washed
 if(hand == 2 && handPresent == 0){
  Serial.print("hand is washed..." );                       
  hand = 3; //atractor mode
  attractorModeDelay = currentMillis;
  }

  

if(handTime < 1) {
hand = 2;
handTime = 20;
//Serial.print("hand is washed = " );                       
//Serial.println(hand);
digitalWrite(17, HIGH);  //stop playing sound
}
 
//----------------------------------------------ATTRACTOR MODES
if(hand == 3){

 //---------------------ring LEDs Attractor Mode for 1 minute then stops
if(currentMillis - attractorModeDelay < 60000){
theaterChaseRainbow1(50);
digitalWrite(17, HIGH);  //stop playing sound
}

 //-----------------------------------clear oops or muybien from screen and neopixel ring
if(currentMillis - attractorModeDelay > 60000 && currentMillis - attractorModeDelay < 60150 ){
matrix.clearScreen();
 colorWipe(strip2.Color(0, 0, 0), 1);
}

  
  
//------------------------------------------------hand attractor Mode
if(currentMillis - attractorModeDelay > 60000){
attractorMode2();
//digitalWrite(16, HIGH);  //stop playing sound
}
}


}
//--------------------------------------------- END LOOP


void printHand(){  //--------------------------PRINT REPORT FUNCTION FOR TROUBLE Shooting
  Serial.print("hand value: ");
  Serial.println(hand);
  Serial.print("hand present: ");
  Serial.println(handPresent);
  Serial.print("attractor mode delay: ");
  Serial.println(attractorModeDelay);
  Serial.print("currentMillis - attractorModeDelay: ");
  Serial.println(currentMillis - attractorModeDelay);
}

void countSeconds()
{
if(hand == 1) {  //hand is washing
   handTime--;
   Serial.print("hand washing seconds: ");
   Serial.println(handTime);
   digitalWrite(17, LOW);  //Play audio file
    
//-------------------------------------------PRINT SECONDS to MATRIX    
    
  matrix.clearScreen(); 
  matrix.setTextSize(2);    // size 1 == 8 pixels high
  matrix.setTextColor(1);   // 'lit' LEDs
  if(handTime > 9) {
    matrix.setCursor(1, 1);   // start at top left, with one pixel of spacing
    matrix.print(handTime);
    matrix.setCursor(24, 0);   // next line, 8 pixels down
    matrix.print("s");
    matrix.writeScreen();
  }
  else {
    matrix.setCursor(7, 1);   // start at top left, with one pixel of spacing
    matrix.print(handTime);
    matrix.setCursor(24, 0);   // next line, 8 pixels down
    matrix.print("ssss");
    matrix.writeScreen();
  }
    
 if(handTime < 1) {
   matrix.clearScreen();
   matrix.setTextSize(1);
   matrix.setTextColor(1);
   matrix.setCursor(3, 0);   // start at top left, with one pixel of spacing
   matrix.print("muy");
   matrix.setCursor(0, 8);   // start at top left, with one pixel of spacing
   matrix.print("bien");
   matrix.writeScreen();
   
   //digitalWrite(16, HIGH);  //turn off sound
}
}
}

//----------------------------------------Populate HAND LEDs
//WS2801 FUNCTION
void colorWipeWs2() {
  int i;
  for (i=0; i < 104; i++) {
    int n = random(0,256);
    for(int j=1; j<germs[i][0]+1; j++){
      strip.setPixelColor(germs[i][j],Wheel(n));
  }}
  strip.show();

}


//----------------------------------------REMOVE LED GERMS FROM HAND SLOWLY
void removeLeds()
{
if(hand == 1) {
int i = random(0,104);
for(int j=1; j<germs[i][0]+1; j++){
strip.setPixelColor(germs[i][j],Color(0,0,0));
}
strip.show();
}
}

//----------------------------------------NEOPiXEL RED/GREEN Timer  
void greenLeds(){

      strip2.setPixelColor(greenLedMarker, strip2.Color(0,led1,0));       //30
      strip2.setPixelColor(greenLedMarker-1, strip2.Color(led3,led2,0));  // 29
      strip2.setPixelColor(greenLedMarker-2, strip2.Color(led4,led5,0));   //28
      strip2.show();
      led1++;
      led2++;
      led5++;
      led3--;
      led4--;
      
      if(led2 == 90){
      led2 = led5;
      
      led5 = 0;
      led1 = 200;
      led3 = led4;
      led4 = 255;
      greenLedMarker--;
      }
      
      if(led1 == 256){
        led1 = 255;}
      if(led3 == 0){
        led3 = 1;}
      if(led4 == 0){
        led4 = 1;}
        delay(7);
}

//-----------------------------------------ATTRACTOR MODE2 for HAND
void attractorMode2(){ 

if(currentMillis - attractorModeTimer > 5000){
attractorModeTimer = currentMillis;

for(int i = 0; i < 181; i++){
strip.setPixelColor(i, Color(0,0,0));}

for(int i = 0; i < 30; i++){
  int k = random(0,104);
  germsAttractor[i] = k;}
}

for(int i=1; i<germs[germsAttractor[attractorCycle]][0]+1; i++){
strip.setPixelColor(germs[germsAttractor[attractorCycle]][i],Wheel(random(0,255)));}
strip.show();

attractorCycle++;
if(attractorCycle == 30){
  attractorCycle = 0;}

}


  





void rainbow(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip2.numPixels(); i++) {
      strip2.setPixelColor(i, c);
  }
strip2.show();
}

void colorWipe2(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip2.numPixels(); i++) {
      strip2.setPixelColor(i, c);}
      strip2.show();


}

/* Helper functions */

// Create a 24 bit color value from R,G,B
uint32_t Color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}


//-------------------------------------- ATRACTROR LIGHTS NeoPixel Ring

void theaterChaseRainbow1(uint8_t wait) {
  if(currentMillis - chaseTime < 10 && writeOn == 1){
  writeOn = 0; 
  
      for (int i=0; i < strip2.numPixels(); i=i+3) {
          strip2.setPixelColor(i+q, Wheel( (i+lightCycle) % 255));    //turn every third pixel on
        }
          strip2.show();
        }

      
      if(currentMillis - chaseTime > 40 && writeOn == 0){
      writeOn = 1;
      chaseTime = currentMillis;
  
        for (int i=0; i < strip2.numPixels(); i=i+3) {
          strip2.setPixelColor(i+q, 0);        //turn every third pixel off
        }

    q++;
    lightCycle++; 
    if(q > 3){
    q = 0;

    if(lightCycle > 255){
    lightCycle = 0;}
    }
   
 }
if(currentMillis - chaseTime > 300 && writeOn == 1){  //this funciton resets the writeON flag if it gets out of phase with the time
writeOn = 1;
chaseTime = currentMillis;}

}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip2.numPixels(); i=i+3) {
        strip2.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip2.show();
     
      delay(wait);
     
      for (int i=0; i < strip2.numPixels(); i=i+3) {
        strip2.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//---------------------------------- WS2801 functions

void rainbowWs(uint8_t wait) {
  int i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycleWs(uint8_t wait) {
  int i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipeWs(uint32_t c, uint8_t wait) {
  int i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}


//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85) {
   return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}




/*
 * NEOPIXEL LED assignment tasked for 10/9/17 Bridging the Hardware/Software Divide Fall 2017
 * 
 * Program: with NEOPIXEL LED strip with 8 NeoPixels and button on a breadboard, takes in button pushes
 * that correspond to four different modes: Neopixel strip off (no button press, default), Neopixel strip cycling through 
 * random colors (one button press), Neopixel strip fading between red and blue (two button presses), Neopixel
 * single light bouncing across/Larson scanner effect (three button presses), and rotation through each of the modes
 * every few seconds (four button presses).
 */
 

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels
#define PIN            14

// How many NeoPixels are attached to the Arduino, in this case 8
#define NUMPIXELS      8


//2D array, color and then rgb values for colors
                     //red,       green,    blue
int colors[3][3] = {{10, 0, 0}, {0,10,0}, {0, 0, 10}};

//used during fading function, starts off with red at 255 and blue 0
int redValue = 255;
int blueValue = 0;

int buttonPin = 13;

//used in colorCycle function and buttonPressHandler
bool colorMode = false;
bool canContinue = true;

bool resetThings = false;

//for button press handling
int buttonPressed = 0; 
unsigned long lastTimeButtonPressed=0;
unsigned long time = millis();
unsigned long timeStartup = millis();
unsigned long timeLastPressed=0; //needs to be global
int lastButtonState=0; //needs to be global


//statecounter of how many buttonpresses we've had
int stateCounter = 0;


// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


void setup() {
    Serial.begin(9600);

    //interrupt handler for the button, makes it respond much better
    attachInterrupt(digitalPinToInterrupt(buttonPin),buttonPressHandler,CHANGE);
    
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.

    //Starup light sequence
      while(millis()-timeStartup < 500)     //Current time minus last time we changed color = interval, checks if the right time passed
      {
            //if appropriate time has passed, we go through each pixel and change it
           for(int i=0;i<NUMPIXELS;i++){
            
            pixels.setPixelColor(i, pixels.Color(50,30,20)); // set current color
            pixels.setPixelColor(i-1, pixels.Color(20,60,10)); // set current color
            pixels.show(); // This sends the updated pixel color to the hardware.
        }
      }
}

void loop() {

  if(resetThings){
    colorMode = false;
    canContinue = true;
    resetThings = false;
  }

burningManEsemble();

}


void burningManEsemble(){


/*switch case for logic depending on buttonpress number (stateCounter)
 *   starts out at default (no press)
 *     1 press, we do randomcolors
 *     2 presses, we do fading colors
 *     3 presses, we do knight rider
 *     4 presses, we do cycle mode, go through each mode automatically
*/

switch(stateCounter){
  case 0:
  Serial.println("At default");
    resetLEDs();
    //nothing here
    break;
    
  case 1:
  Serial.println("Random colors");
    randomColors();
    break;
    
  case 2:
  Serial.println("Fading Colors");
    fadingColors();
    break;
  case 3:
  Serial.println("Knight Rider");
    resetLEDs();
    knightRider();
    break;
  case 4:
  Serial.println("Cycle Mode");
    cycleMode();
    break;
  case 5:
    stateCounter = 0;
    break;
  
}

  
  
}

void buttonPressHandler()
{ 
        int buttonState;
        buttonState = digitalRead(buttonPin);
        
        if (buttonState==LOW && millis()>timeLastPressed+200 && lastButtonState==HIGH)
         {

          //this is specifically for the colorCycle function, if we are in it then colorMode will be true
          //and we have to reset things in order to discontinue cycling through the modes
          if(colorMode == true){
            Serial.println("Pushed button in colormode, stop it");
              canContinue = false;
             colorMode = false;
             resetLEDs();
             yield();
          }

         resetLEDs();
         stateCounter++;
        timeLastPressed = millis();
        Serial.println("BUTTON PRESSED");
        
        }
        lastButtonState=buttonState;
}


void randomColors(){
  int color1 = random(0, 3);
  Serial.println(colors[color1][0]);

      if(millis()-time > 500)     //Current time minus last time we changed color = interval, checks if the right time passed
      {
          //if appropriate time has passed, we go through each pixel and change it
           for(int i=0;i<NUMPIXELS;i++){
            Serial.print(i);
          Serial.println(colors[color1][0]);
          
          pixels.setPixelColor(i, pixels.Color(colors[color1][0],colors[color1][1],colors[color1][2])); // set current color
          pixels.show(); // This sends the updated pixel color to the hardware.
          time = millis();           //and reset time.
      }
     }
}

void fadingColors(){
//transition between two colors, red and blue
//(255, 0, 0) -> (0, 0, 255)
int colorMax = 255;
int fadeSpeed = 1;


//if colors(this slot is 255, then we are HIGH, need to get low, 0, if red is HIGH then this needs to be high);
  if(redValue >= 255){
    if(millis()-time > 5){
      Serial.println("RED HIGH");
          for(int i = 255; i > 0; i--){
          redValue--;
          Serial.println(blueValue);
          blueValue++;
          
          for(int i=0;i<NUMPIXELS;i++){
          pixels.setPixelColor(i, pixels.Color(redValue,0,blueValue));
          }
          pixels.show(); // This sends the updated pixel color to the hardware.
        }
        time = millis();
    }
  }

  //if red is 0, then we need to go reverse direction, the color is blue and we need to transition to red
  if(redValue == 0){
    if(millis()-time > 5){
    Serial.println("BLUE HIGH");
         for(int i = 255; i > 0; i--){
        blueValue--;
        Serial.println(blueValue);
        redValue++;

        for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(redValue,0,blueValue));
        pixels.show(); // This sends the updated pixel color to the hardware.
        }
      }
      time = millis();
    }
   }
}


bool reachedTop = false;

void knightRider(){

int knightRiderColor[3] = {255, 0,0};
unsigned long timeHere = 0;


        if(reachedTop == false){
           for(int i=0;i<NUMPIXELS;i++){
              if(millis()-time > 200)     //Current time minus last time we changed color = interval, Has the right time passed?
                {
                Serial.print(i);
    
              //I had issues with the top LED staying on... this fixed it... I don't know why it was doing that to start with or how this really helps but...          
              pixels.setPixelColor(7, pixels.Color(0,0,0)); // Moderately bright green color.
              //normal color setting up, first put color into current LED, turn off previous
              pixels.setPixelColor(i, pixels.Color(knightRiderColor[0],knightRiderColor[1],knightRiderColor[2]));
              pixels.setPixelColor(i-1, pixels.Color(0,0,0)); // Moderately bright green color.
              pixels.show(); // This sends the updated pixel color to the hardware.

              time = millis();           //and reset time.
              yield();

              } else{
                //My thing kept skipping around, think it is because when this goes through and if my if statement
                //fails, it just continues, by decrementing i I make sure we go back and set the correct pixel
                //when the time comes instead of moving onward.
                i--;  
              }
                  
             }
             reachedTop = true;
        } else if(reachedTop == true){
          
          for(int i=NUMPIXELS;i>-1;i--){
              if(millis()-time > 200)     //Current time minus last time we changed color = interval, Has the right time passed?
                {
                Serial.print(i);
              //normal color setting up, first put color into current LED, turn off previous
              pixels.setPixelColor(i, pixels.Color(knightRiderColor[0],knightRiderColor[1],knightRiderColor[2]));
              pixels.setPixelColor(i+1, pixels.Color(0,0,0)); // Moderately bright green color.
              pixels.show(); // This sends the updated pixel color to the hardware.

              time = millis();           //and reset time.
              yield();

              } else{
                //My thing kept skipping around, think it is because when this goes through and if my if statement
                //fails, it just continues, by decrementing i I make sure we go back and set the correct pixel
                //when the time comes instead of moving onward.
                i++;  
              }   
             }
             reachedTop = false; 
          
        }

}



void cycleMode(){
  int state = 0;

  int interval = 6000;

unsigned long timeHere = millis();

//We are in colormode, will be checked in buttonhandler
colorMode = true;


//while we are allowed to cycle through (the button hasn't been pressed), let's go through them
while(canContinue){
        //if we are cycling through, the first stop is state 0, random colors
        while(state == 0 && canContinue){
          randomColors();
          if(millis()-timeHere > interval){
            //the appropriate time has passed
            state++; //update the state so we can continue onward
          }
          yield();
        }
        timeHere = millis();
         while(state == 1 && canContinue){ 
          resetLEDs(); 
          fadingColors();
          if(millis()-timeHere > interval){
            state++;
          }
          yield();
        }
        timeHere = millis();
        while(state == 2 && canContinue){
          resetLEDs();  
          knightRider();
          if(millis()-timeHere > interval){
            state++;
          }
          yield();
        }
        //if we get here, then we have gone through all the modes, reset state and get out
        if(state >2){
         state = 0;
         yield();
        }
} 

           resetThings = true;
}



//Resets all the LEDs so they turn off
void resetLEDs(){
  for(int i=0;i<NUMPIXELS;i++){
        pixels.setPixelColor(i, pixels.Color(0,0,0));
        pixels.show();
        }
        
}



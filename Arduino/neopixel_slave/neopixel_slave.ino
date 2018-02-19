/* Neopixel Slave for Marlin firmware
 * 
 * Read README at:
 * https://github.com/GarrethX/NeopixelMarlinSlave
 * 
 * Version 0.2
 *  - Breathing effect added
 *  - Smoothed transitions a little bit
 *  
 * Version 0.1
 *  - Initial release
 *  
 */

#include <Wire.h>
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include <math.h>

//#define DEBUG  //Enable to get serial debugging 

const uint16_t PixelCount = 25; // make sure to set this to the number of pixels in your strip
const uint8_t PixelPin = 6;  // make sure to set this to the correct pin

NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(PixelCount, PixelPin);


// Values obtained from master
uint8_t temperatureHotend=0;
uint16_t targetHotend=0;
uint16_t temperatureBed=0;
uint16_t targetBed=0;
uint8_t progress=0;
uint16_t maxBed=150;
uint16_t maxHotend=300;
uint8_t printerState=0;
uint8_t customRed=0;
uint8_t customGreen=0;
uint8_t customBlue=0;
uint8_t customWhite=0;
uint8_t customPixel=0;
uint8_t stripNumber=0;

// Define variables
bool newData=false;
unsigned long rate=2000;
unsigned long last=0;
int lastPrinterState=0;
int fadingValue=0;
bool fadingDir=true;

void setup() {
  #ifdef DEBUG
    Serial.begin(9600);           // start serial for output
    Serial.println();
    Serial.println("Running...");  
  #endif
    
  strip.Begin();
  strip.Show();

  //You can select startup behaviour here, or create your own:
  //stripSet(255, 255, 255);
  theaterChase(RgbColor(127, 127, 127), 50); // White
  //theaterChase(RgbColor(127, 0, 0), 50); // Red
  //theaterChase(RgbColor(0, 0, 127), 50); // Blue

  //Initialize I2C interface
  Wire.setClock(400000);
  Wire.setTimeout(300);
  Wire.begin(8);                // join i2c bus with address 8
  Wire.onReceive(receiveEvent); // register event
  Wire.setTimeout(300);
  } //End setup()


void loop() {
  #ifdef DEBUG
    if(last+rate<millis()){
    last=millis();
     if (isnan(progress) || isnan(temperatureHotend)|| isnan(temperatureBed)|| isnan(targetBed)){
      Serial.println("i2c problem?");
     }
     else{
    Serial.print("Progress: ");
    Serial.print(progress,DEC);
    Serial.println("%");
    Serial.print("Temp_hotend: ");
    Serial.println(temperatureHotend,DEC);
    Serial.print("Temp_target_hotend: ");
    Serial.println(targetHotend,DEC);
    Serial.print("Temp_max_hotend: ");
    Serial.println(maxHotend,DEC);    
    Serial.print("Temp_bed: ");
    Serial.println(temperatureBed,DEC);
    Serial.print("Temp_target_bed: ");
    Serial.println(targetBed,DEC);
    Serial.print("Temp_max_bed: ");
    Serial.println(maxBed,DEC);
    Serial.print("Printer State: ");
    Serial.println(printerState,DEC);
    Serial.println("************************************");
     } 
    }
  #endif

  //Clear strip if a new printer state arrives
  if (printerState != lastPrinterState){ 
    stripSet(0, 0, 0);
    lastPrinterState=printerState;
  }

  switch (printerState){
    case 0: //No state from printer
      if (newData==false){
        stripSet(200, 200, 200);
      }
      break;
    case 1: //Bed heatup
      showProgress(temperatureBed, targetBed, maxBed, 255, 0, 255);
      newData=false;
      break;
    case 2: //Hotend heatup
      showProgress(temperatureHotend, targetHotend, maxHotend, 255, 0, 0);
      newData=false;
      break;
    case 3: //Printing
      showProgress(progress, 0, 100, 0, 255, 0);
      newData=false;
      break;
    case 4: //Printing finished
      stripSet(0, fadingValue, 0);
      break;
    case 5: //Printing paused
      stripSet(0, 0, fadingValue);
      break;
    case 6: //Printing stopped
      stripSet(fadingValue, 0, 0);
      break;
    case 7: //Show custom strip color
      if (newData){
      stripSet(customRed, customGreen, customBlue);
      newData=false;
      }
      break;
    case 8: //Show custom pixel color
      if (newData){
      stripSetPixel(customRed, customGreen, customBlue, customPixel);
      newData=false;
      }
      break;
    case 9: //Idle
      stripSet(fadingValue, fadingValue, fadingValue);
      break;                    
  }
  fadingValue=breathingFunction();
  delay(10);

}

// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  byte buffer[30];
  int i=0;
  bool status=false;
  char c;
  #ifdef DEBUG
    Serial.println("Reading...");
  #endif
  while(Wire.available()) { // loop through all but the last
    Wire.readBytes(buffer,howMany);   
    }
    if (buffer[0]=='p'){ //print
      printerState=buffer[1];
      progress=buffer[2];
      stripNumber=buffer[3];
      newData=true;
    }
    else if (buffer[0]=='b'){ //bed
      printerState=buffer[1];
      temperatureBed=buffer[2]*256+buffer[3];
      targetBed=buffer[4]*256+buffer[5];
      maxBed=buffer[6]*256+buffer[7];
      stripNumber=buffer[8];    
      newData=true;
    }
    else if (buffer[0]=='h'){ //hotend
      printerState=buffer[1];
      temperatureHotend=buffer[2]*256+buffer[3];
      targetHotend=buffer[4]*256+buffer[5];
      maxHotend=buffer[6]*256+buffer[7];
      stripNumber=buffer[8];
      newData=true;
    }
    else if (buffer[0]=='c'){ //custom
      printerState=buffer[1];
      customWhite=buffer[2];
      customRed=buffer[3];
      customGreen=buffer[4];
      customBlue=buffer[5];
      customPixel=buffer[6];
      stripNumber=buffer[7];
      newData=true;
    }
    else if (buffer[0]=='i'){ //init
      printerState=buffer[1];
      maxBed=buffer[2]*256+buffer[3];
      maxHotend=buffer[4]*256+buffer[5];
      stripNumber=buffer[6];
      newData=true;
    }  
    else if (buffer[0]=='s'){ //state
      printerState=buffer[1];
      stripNumber=buffer[2];
      newData=true;
    }
  int x = Wire.read();    // make sure we get all data
} //receiveEvent

int lastFadeG, lastFadeB, lastFadeR;

void showProgress(uint8_t currentValue, int targetValue, int maxValue, int rMax, int gMax, int bMax) {
 
  int tmpFadeG=0, tmpFadeB=0, tmpFadeR=0;
  if (newData){  
    int resolution=maxValue/PixelCount;
    
    if (targetValue>0){ //Set target pixel color
      int targetPixel = map(constrain(targetValue,0,maxValue),0,maxValue,0,PixelCount);
      strip.SetPixelColor(targetPixel, RgbColor(rMax,gMax,bMax)); 
    }
    
    for (uint16_t pixel = 0; pixel <= PixelCount-1; pixel++) { //Show progress

      if (currentValue>=(pixel*resolution)) {   // Set pixes to full intensity when they should be fully lit
        strip.SetPixelColor(pixel, RgbColor(rMax,gMax,bMax)); 
      }
      else if (currentValue<(pixel*resolution)){ // Smoothly set pixel to intermediate value    
        int fadeG=map((currentValue%resolution),0,(resolution-1),0,gMax);
        int fadeR=map((currentValue%resolution),0,(resolution-1),0,rMax);
        int fadeB=map((currentValue%resolution),0,(resolution-1),0,bMax);
        if (lastFadeG>fadeG) { lastFadeG=0;}
        if (lastFadeB>fadeB) { lastFadeB=0;}
        if (lastFadeR>fadeR) { lastFadeR=0;}
        for (int i=0;i<=50;i++){
          tmpFadeG=((fadeG-lastFadeG)*i)/50+lastFadeG;
          tmpFadeB=((fadeB-lastFadeB)*i)/50+lastFadeB;
          tmpFadeR=((fadeR-lastFadeR)*i)/50+lastFadeR;
        strip.SetPixelColor(pixel-1, RgbColor(tmpFadeR,tmpFadeG,tmpFadeB));
        delay(5);  //Smoothing time is (delay + time to set pixel)*number of loop iterations, don't make this time too long, or there will be problems with receiving data
        }
        lastFadeG=fadeG;
        lastFadeB=fadeB;
        lastFadeR=fadeR;
        break; 
      }  
    } 
    strip.Show();
  }
} //showProgress

void stripSet(int r, int g, int b){
  for (uint16_t pixel = 0; pixel <= PixelCount-1; pixel++) { 
    strip.SetPixelColor(pixel, RgbColor(r,g,b));
  }
  strip.Show(); 
} //stripSet

void stripSetPixel(int r, int g, int b,int pixel){
  strip.SetPixelColor(pixel, RgbColor(r,g,b));
  strip.Show(); 
}  //stripSetPixel

void theaterChase(RgbColor c, uint8_t wait) { //Base on example from Adafruit
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < PixelCount; i=i+3) {
        strip.SetPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.Show();

      delay(wait);

      for (uint16_t i=0; i < PixelCount; i=i+3) {
        strip.SetPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
    if (newData){
      break;
    }
  }  
} //theaterChase

int breathingFunction() {
  float value = (exp(sin(millis()/2000.0*PI)) - 0.36787944)*108.0;
  return (int)value;
} //breathingFunction


/**********************************************************************
* Was NanoAnalog8Sensor 15/09/2019 *** 
* migrated to nano
* 
* Removed dispay info from main body and created displayInfo() funtion
* added second sensor
* 
* 
* V3
* adding running average...
* Add second (multiple) sensors
* display sensor averages
*
**ALL WORKING 03/10/2019
*
*N.B. Each sensor requires about 5V at 200mA for its heater
 *********************************************************************/
 //
 //
 /***************************
 *V6.0 23/12/2020
 *
 *Add third sensor and add to display.
 *
 *
 ***************************/
//
//THE CODE!//
//
#include <Wire.h>  //I2C Library
#include <LiquidCrystal_I2C.h> //LCD Libray

#include "RunningAverage.h"

RunningAverage ra1Minute(60); //creates 60 samples i.e. one per second
RunningAverage ra1Hour(60);
RunningAverage ra2Minute(60); //creates 60 samples i.e. one per second
RunningAverage ra2Hour(60);
RunningAverage ra3Minute(60); //creates 60 samples i.e. one per second
RunningAverage ra3Hour(60);
int samples = 0;

/*Declare Constants*/

const int analogInPin1 = A0;  // Analog input pin that Sensor1 is attached to
const int analogInPin2 = A1;  // Analog input pin that Sensor2
const int analogInPin3 = A2;  // Analog input pin that Sensor3

const unsigned long refreshFreq = 3000; //refresh display every 'n' millis

//const int analogOutPin1 = 9; // Analog output pin that the LED or whatever is attached to


//Declare objects

//My 2004, 20 chars by 04 lines, LCD display addr is 0x3f
//Use iic_scanner to find it if not sure of addr
// Set the pins on the I2C chip used for LCD connections:
//                    addr, en,rw,rs,d4,d5,d6,d7,bl,blpol
//LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);  // Sets the LCD I2C address etc
//LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3);  // Sets the LCD I2C address etc

LiquidCrystal_I2C lcd(0x3f,20,4);//new version of LCD library
int sensorValue1 = 0;        // value read from the sensor, initialise to 0
int sensorValue2 = 0;
int sensorValue3 = 0;

//int outputValue1 = 0;       // value output to the PWM (analog out) were we to use it

int lastSV1;
int lastSV2;


unsigned long LastAvUpdate; //average should be updated every second to get minute and hour average
unsigned long LastDisp;
unsigned long ThisTime;

//Setup
void setup() {

  pinMode(LED_BUILTIN, OUTPUT);// in case we want it later, built_in LED is on PIN 13 on just about all boards inc UNO/NANO/LEONARDO/PRO etc etc
  
  // initialize serial communications at 9600 (well faster ) bps:
  Serial.begin(115200);

  //init the running average
  Serial.println("Demo RunningAverage lib - average per minute & hour");
  Serial.print("Version: ");
  Serial.println(RUNNINGAVERAGE_LIB_VERSION);
  
  ra1Hour.clear();
  ra1Minute.clear();

  ra2Hour.clear();
  ra2Minute.clear();

  ra3Hour.clear();
  ra3Minute.clear();
  
  //init LCD for 20x4
  lcd.init(); 

  // ------- Quick blinks of backlight as a start-up indicator-------------
  for(int i = 0; i< 10; i++)
  {
    lcd.backlight();
    delay(50);
    lcd.noBacklight();
    delay(50);
  }
  lcd.backlight(); // finish with backlight on  

  //initialise time counters used for calcs
  ThisTime = millis();
  LastDisp = ThisTime;
  LastAvUpdate = ThisTime;
  
}
//Main Loop
void loop() {
  
  // read the current analog in value:
  sensorValue1 = analogRead(analogInPin1);
  sensorValue2 = analogRead(analogInPin2);
  sensorValue3 = analogRead(analogInPin3);

 
  ThisTime=millis();
  
  //if sufficient time elapsed since last refresh then refresh
  if (ThisTime-LastDisp > refreshFreq)
  {
    displayInfoOnLCD(); //refresh display
    DisplayToSerial(); //also to serial
    
    LastDisp=ThisTime; //reset display timer
  }
 
  if (ThisTime-LastAvUpdate >= 1000) //update every second
  {
    updateRunningAvs(); //update avs 
    LastAvUpdate=ThisTime; //reset av calc timer
  }
}

//***************
//Update Averages
//***************
void updateRunningAvs()
{
//to get minutes and hour av should be done once per second
  
 //Running Averags for minutes
  ra1Minute.addValue(sensorValue1);    
  ra2Minute.addValue(sensorValue2);
  ra3Minute.addValue(sensorValue3);
  
  //inc sample count
  samples++;
  
  if (samples % 60 == 0) {
    //update running hour av
    ra1Hour.addValue(ra1Minute.getAverage());
    ra2Hour.addValue(ra2Minute.getAverage());
    ra3Hour.addValue(ra3Minute.getAverage());
  } 
}
//***************
//Dislpay Refresh
//***************
void displayInfoOnLCD()
{
//
//-------- Write characters on the display ------------------
// NOTE: Cursor Position: Lines and Characters are zero based.

  //Title line 0 
  lcd.setCursor(0,0); //Start at character 0 on line 0
  lcd.print("AQM V6.1
  
  #
  243018");
  lcd.setCursor(10,0);
  lcd.print("Min");
  lcd.setCursor(14,0);
  lcd.print("Hour");
  
  //display Sensor 1 info
  lcd.setCursor(0,1); //line 2 of display
  lcd.print("S01 ");  //MQ135 Air Quality Sensor
  lcd.print(sensorValue1);
  lcd.print("  ");
  lcd.setCursor(10,1);
  lcd.print(ra1Minute.getAverage(), 0);
  lcd.print("  ");
  lcd.setCursor(14,1);
  lcd.print(ra1Hour.getAverage(), 1);
  
  //display Sensor 2 info
  lcd.setCursor(0,2); //line 3 of display
  lcd.print("S02 ");  //Sensor 2
  lcd.print(sensorValue2);
  lcd.print("  ");
  lcd.setCursor(10,2);
  lcd.print(ra2Minute.getAverage(), 0);
  lcd.print("  ");
  lcd.setCursor(14,2);
  lcd.print(ra2Hour.getAverage(), 1);

  //display Sensor 3 info
  lcd.setCursor(0,3); //line 4 of display
  lcd.print("S03 ");  //Sensor 3
  lcd.print(sensorValue3);
  lcd.print("  ");
  lcd.setCursor(10,3);
  lcd.print(ra3Minute.getAverage(), 0);
  lcd.print("  ");
  lcd.setCursor(14,3);
  lcd.print(ra3Hour.getAverage(), 1); 
}
//******************
//Dump the info to the serial monitor
//******************
void DisplayToSerial(){
  //display to serial
  Serial.print(sensorValue1);
  Serial.print("\t");
  //Serial.print("  raMinute: ");
  Serial.print(ra1Minute.getAverage(), 4);
  Serial.print("\t");
  Serial.println(ra1Hour.getAverage(), 4);
  //Serial.println(sensorValue2);
 
}
/*
void setup(void) 
{
  
}

void loop(void) 
{
  long rn = random(0, 100);
  raMinute.addValue(rn);
  samples++;
  
  if (samples % 60 == 0) raHour.addValue(raMinute.getAverage());
  
  Serial.print("  raMinute: ");
  Serial.print(raMinute.getAverage(), 4);
  Serial.print("  raHour: ");
  Serial.println(raHour.getAverage(), 4);
}


/**DROSS
 * 
 * 
 * // Get the LCD I2C Library here: 
// https://bitbucket.org/fmalpartida/new-liquidcrystal/downloads
// Move any other LCD libraries to another folder or delete them
// See Library "Docs" folder for possible commands etc.



  // map it to the range of the analog out:
  // outputValue1 = map(sensorValue1, 0, 1023, 0, 255);
  // change the analog out value:
  // analogWrite(analogOutPin1, outputValue1);

  //calc dy/dt to monitor rate of change
  //**** do away with this, we need a rolling average
  //diff1 = (float) (sensorValue1-lastSV1)/float(ThisTime-LastTime);
  //diff1 = diff1 * 1000.0;
  //if (diff1 >0) Serial.print("+");
  //Serial.print(diff1,4);

// Dim an LED using a PWM pin
int PWMpin = 10;  // LED in series with 470 ohm resistor on pin 10

void setup() {
  // no setup needed
}

void loop() {
  for (int i = 0; i <= 255; i++) {
    analogWrite(PWMpin, i);
    delay(10);
  }
}


for (byte i = 0; i < 5; i = i + 1) {
  Serial.println(myPins[i]);
}

  int myInts[6];
  int myPins[] = {2, 4, 8, 3, 6};
  int mySensVals[6] = {2, 4, -8, 3, 2};
  char message[6] = "hello";
**/

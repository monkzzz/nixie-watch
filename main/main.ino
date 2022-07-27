//Libs
//lib for hv5530 control
#include <ShiftRegister74HC595.h>

//lib for retrieving time from wifi
#include <WiFi.h>
#include "time.h"
//---------------------------------------

//Pins

//Pins HV5530

#define POL      16  //        // Polarity
#define BL       19 //        //  Blank

#define LE       17 //        // Latch
#define DATA     23 // mosi   // Data in
#define CLOCK    18 // sck    // Clk
//---------------------------------------

//Pins HVSMD

#define VEN      21 // enable hv    
#define VADJ     22 // regulate voltage
//---------------------------------------

//botao
struct Button {
  const uint8_t PIN;
  bool pressed;
};

Button button1 = {0, false};

void IRAM_ATTR isr() {
  button1.pressed = true;
}
//--------------------------------------

//Defines

// PWM properties
const int freq = 16000;
const int PWMChannel = 0;
const int resolution = 8;
const int dutyCycle = 210; //175 v 1 nixie in-2 1.85 mA
//---------------------------------------

//wifi
const char* ssid       = "******";
const char* password   = "******";
//---------------------------------------

//time server
const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 3600*0;
const int   daylightOffset_sec = 3600;
//----------------------------------------

//Shift Register

// parameters: (number of shift registers, data pin, clock pin, latch pin)
ShiftRegister74HC595 sr (4, DATA, CLOCK, LE);

// shift register positions for each digit in each tube

//       Digit     0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   red,   green,   blue,
int tube1[] = { 0,    1,    2,    3,    5,    7,    9,    8,    6,    4,     11,     10,    12};

//		Digit      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   red,   green,   blue,			,
int tube2[] = {31,   30,   29,   27,   25,   23,   22,   24,   26,   28,     19,     20,     18};
//-----------------------------------------------------------------

void setup() {
 
 //HVSMD
  pinMode(VEN, OUTPUT);
  digitalWrite(VEN, HIGH);

  //PWM
  ledcSetup(PWMChannel, freq, resolution);
  ledcAttachPin(VADJ, PWMChannel);
 //-----------------------------------------------------------------

  //HV5530
  pinMode(POL, OUTPUT); // Polarity
  pinMode(BL, OUTPUT); // Blank
  
  pinMode(LE, OUTPUT); // Latch
  pinMode(DATA, OUTPUT); // Data
  pinMode(CLOCK, OUTPUT); // Clk

  digitalWrite(POL, HIGH); //Disable Outputs to prevent flicker
  digitalWrite(BL, LOW); //Disable Outputs to prevent flicker

  digitalWrite(LE, LOW);
  digitalWrite(DATA, LOW);
  digitalWrite(CLOCK, LOW);
//---------------------------------------------------------

  //botao
  pinMode(button1.PIN, INPUT_PULLUP); //pullup_button
  attachInterrupt(button1.PIN, isr, FALLING);
//---------------------------------------------------------

  //Blank Display
  BlankDisplay();
  delay(500);

  Serial.begin(9600);
  
	//First Boot
	Serial.println("All in One first boot");
	
	//Blank Display
	BlankDisplay();
	delay(500);
	
	//High voltage adjust
	Serial.println("PWM fixed");
    ledcWrite(PWMChannel, dutyCycle);
	delay(500);
	
	//Start HV
	Serial.println("HV Enabled");
	digitalWrite(VEN, LOW);
	delay(500);
	
	//conect to the internet and retrieve time
	//wifi connection
	ConnectWifi();
	delay(500);
	
	//Show time
	//printLocalTime();
	//delay(500);
	
	//shut off HV
	Serial.println("HV Disabled");
	digitalWrite(VEN, HIGH);
	delay(500);
	
	//Done
	Serial.println("Done");	
	delay(500);	
}
//------------------------------------------------------------------------

void loop() {
  // put your main code here, to run repeatedly:
  
  if (button1.pressed) {
	Serial.println("Button Press");
	
	//Blank Display
	BlankDisplay();
	delay(500);
	
	//Adjust HV
	Serial.println("PWM fixed");
    ledcWrite(PWMChannel, dutyCycle);
	delay(500);
	
	//Start HV
	Serial.println("HV Enabled");
	digitalWrite(VEN, LOW);
	delay(500);
	
	//Show time
	printLocalTime();
	delay(500);
	
	//shutdown HV
	Serial.println("HV Disabled");
	digitalWrite(VEN, HIGH);
	delay(500);
	
	//done
	Serial.println("Done");	
	delay(500);
  button1.pressed = false;
  }
  delay(500);
}
//------------------------------------------------------------------------

//Functions

//Blank Display
void BlankDisplay(){
  digitalWrite(BL, HIGH); //Disable Outputs to prevent flicker
  digitalWrite(POL, HIGH); //Disable Outputs to prevent flicker
  
  // Disconnect shift register from data latches:
  Serial.println( "blank");

   sr.setAllLow(); // set all pins LOW
   delay(500);
}
//------------------------------------------------------------------------

//connect to WiFi
void ConnectWifi(){
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}
//------------------------------------------------------------------------

//retrieves time and shows it
void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");

//split the hours
  //Serial.println("hour");
  uint8_t H =  timeinfo.tm_hour;
  uint8_t H1 = H/10;
  uint8_t H2 = H%10;
  //Serial.println(H);
  //Serial.println(H1);
  //Serial.println(H2);

  //Serial.println("min");
  uint8_t M =  timeinfo.tm_min;
  uint8_t M1 = M/10;
  uint8_t M2 = M%10;
  //Serial.println(M);
  //Serial.println(M1);
  //Serial.println(M2);  

  NixieDisplayTime(H1, H2, M1, M2);
  delay(500);
}
//------------------------------------------------------------------------

//show digits
void NixieDisplayTime(uint8_t H1,uint8_t H2, uint8_t M1,uint8_t M2)
{
  //hour
  // set pins without immediate update
  Serial.println("hour");
  Serial.println(H1);
  sr.setNoUpdate(tube2[H1], HIGH);
  Serial.println(H2);
  sr.setNoUpdate(tube1[H2], HIGH);
  // at this point of time, pin 0 and 1 did not change yet
  sr.updateRegisters(); // update the pins to the set values
  delay(3000);

  BlankDisplay();
  
  delay(100);
  
  //min
  Serial.println("min");
  Serial.println(M1);
  sr.setNoUpdate(tube2[M1], HIGH);
  Serial.println(M2);
  sr.setNoUpdate(tube1[M2], HIGH);
  sr.updateRegisters();
  delay(3000);

  BlankDisplay();
  delay(100);
}
//------------------------------------------------------------------------

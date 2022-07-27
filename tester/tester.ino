// 45 simulates the power on
// 46 simulates a button press

//Libs
//lib for hv5630 control
#include <ShiftRegister74HC595.h>

//lib to retrieve the time from the internet
#include <WiFi.h>
#include "time.h"

//Pins

//Pins HV5530

#define POL      16 //        // polarity                // pin 20
#define BL       19 //        // socket Blank            // pin 25

#define LE       17 //        // socket Latch            // pin 23
#define DATA     23 // mosi   // socket data in          // pin 24
#define CLOCK    18 // sck    // socket clk              // roxo baixo

//gnd - 21 gnd
//+5v - 22 vdd
//---------------------------------------

//Pins HVSMD

#define VEN      21 // enable hv    
#define VADJ     22 // regulate voltage
//---------------------------------------

// PWM properties
const int freq = 16000;
const int PWMChannel = 0;
const int resolution = 8;
const int dutyCycle = 210; //175 v 1 nixie in-2 1.85 mA
//---------------------------------------

//Shift Register

// parameters: (number of shift registers, data pin, clock pin, latch pin)
ShiftRegister74HC595 sr (4, DATA, CLOCK, LE);

// shift register positions for each digit in each tube

//       Digit     0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   red,   green,   blue,
int tube1[] = { 0,    1,    2,    3,    5,    7,    9,    8,    6,    4,     11,     10,    12};

//		Digit      0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   red,   green,   blue,			,
int tube2[] = {31,   30,   29,   27,   25,   23,   22,   24,   26,   28,     19,     20,     18};
//-----------------------------------------------------------------

//wifi
const char* ssid       = "******";
const char* password   = "******";
//---------------------------------------

//time server
const char* ntpServer = "europe.pool.ntp.org";
const long  gmtOffset_sec = 3600*0;
const int   daylightOffset_sec = 3600;
//----------------------------------------

void setup(){
	
	//HVSMD
	pinMode(VEN, OUTPUT);
	digitalWrite(VEN, HIGH);

	//PWM
	ledcSetup(PWMChannel, freq, resolution);
	ledcAttachPin(VADJ, PWMChannel);
//---------------------------------------------------------

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

  BlankDisplay();
 
Serial.begin(9600);

while (!Serial);

Serial.println("Input 1 to LL on and 2 to LH and 39 to HH");

Serial.println("Input 3 to BLANK on and 4 to light 1");
Serial.println("Input 5 to shift 0 to and 6 to go 1 transp latch");

Serial.println("Input 7 to shift 00 and 8 to shift 11");
Serial.println("Input 9 to shift 22 and 10 to shift 33");
Serial.println("Input 11 to shift 44 and 12 to shift 55");
Serial.println("Input 13 to shift 66 and 14 to shift 77");
Serial.println("Input 15 to shift 88 and 16 to shift 99");
Serial.println("Input 17 to shift 2red and 18 to shift 2green");

Serial.println("Input 19 to 2blue and 20 connect wifi");
Serial.println("Input 21 to to show time and 22 to ");
Serial.println("Input 23 to  and 24 to ");
Serial.println("Input 25 to  and 26 to ");
Serial.println("Input 27 to  and 28 to ");
Serial.println("Input 29 to  and 30 to ");
Serial.println("Input 31 to  and 32 to ");
Serial.println("Input 33 to  and 34 to ");

Serial.println("Input 35 to enable voltage and 36 to disable");

Serial.println("Input 37  and 38 ");

Serial.println("Input 39 to HH and 40 ");

Serial.println("Input 41 to and 42 to ");

Serial.println("Input 43 to PWM fixed and 44 ");

Serial.println("Input 45 to first boot and 46 to button press ");
}

void loop() {

if (Serial.available())

{

int state = Serial.parseInt();

if (state == 1)

{

  digitalWrite(BL, LOW); //Disable Outputs to prevent flicker
  digitalWrite(POL, LOW); //Disable Outputs to prevent flicker
  
Serial.println("Command completed LED turned ON");

}

if (state == 2)

{

digitalWrite(BL, LOW); //Disable Outputs to prevent flicker
digitalWrite(POL, HIGH); //Disable Outputs to prevent flicker
 
Serial.println("Command completed LED turned OFF");

}
if (state == 3)

{
  Serial.println("Blank");
  BlankDisplay();

}
if (state == 4)

{
  BlankDisplay();
  Serial.println("Light one");
  light1();
}

if (state == 5)

{
Serial.println("Shift zero");
	
digitalWrite(BL, HIGH);
digitalWrite(POL, HIGH);


digitalWrite(DATA, LOW);
digitalWrite(CLOCK, HIGH);
digitalWrite(CLOCK, LOW);
  
//latch
//LE low to high 
digitalWrite(LE, LOW);
digitalWrite(LE, HIGH);
}

if (state == 6)

{
BlankDisplay();
sequence();
}

if (state == 7)

{
uint8_t H1 = 0; //libtube 2
uint8_t H2 = 0; //libtube 1

Serial.println("Lib shift 00");

BlankDisplay();
    
NixieDisplay(H1,H2);
}
if (state == 8)

{
uint8_t H1 = 1; //libtube 2
uint8_t H2 = 1; //libtube 1

Serial.println("Lib shift 11");

BlankDisplay();
    
NixieDisplay(H1,H2);
}
if (state == 9)

{
uint8_t H1 = 2; //libtube 2
uint8_t H2 = 2; //libtube 1

Serial.println("Lib shift 11");

BlankDisplay();
    
NixieDisplay(H1,H2);

}
if (state == 10)

{
uint8_t H1 = 3; //libtube 2
uint8_t H2 = 3; //libtube 1

Serial.println("Lib shift 33");

BlankDisplay();
    
NixieDisplay(H1,H2);
}
if (state == 11)

{
uint8_t H1 = 4; //libtube 2
uint8_t H2 = 4; //libtube 1

Serial.println("Lib shift 44");

BlankDisplay();
    
NixieDisplay(H1,H2);
}
if (state == 12)

{
uint8_t H1 = 5; //libtube 2
uint8_t H2 = 5; //libtube 1

Serial.println("Lib shift 55");

BlankDisplay();
    
NixieDisplay(H1,H2);
}

if (state == 13)

{
uint8_t H1 = 6; //libtube 2
uint8_t H2 = 6; //libtube 1

Serial.println("Lib shift 66");

BlankDisplay();
    
NixieDisplay(H1,H2);
}

if (state == 14)

{
uint8_t H1 = 7; //libtube 2
uint8_t H2 = 7; //libtube 1

Serial.println("Lib shift 77");

BlankDisplay();
    
NixieDisplay(H1,H2);
}


if (state == 15)

{
uint8_t H1 = 8; //libtube 2
uint8_t H2 = 8; //libtube 1

Serial.println("Lib shift 88");

BlankDisplay();
    
NixieDisplay(H1,H2);

}

if (state == 16)

{
uint8_t H1 = 9; //libtube 2
uint8_t H2 = 9; //libtube 1

Serial.println("Lib shift 99");

BlankDisplay();
    
NixieDisplay(H1,H2);
}

if (state == 17)

{
uint8_t H1 = 10; //libtube 2
uint8_t H2 = 10; //libtube 1

Serial.println("Lib shift red red");

BlankDisplay();
    
NixieDisplay(H1,H2);
}

if (state == 18)

{
uint8_t H1 = 11; //libtube 2
uint8_t H2 = 11; //libtube 1

Serial.println("Lib shift green green");

BlankDisplay();
    
NixieDisplay(H1,H2);
}

if (state == 19)

{
uint8_t H1 = 12; //libtube 2
uint8_t H2 = 12; //libtube 1

Serial.println("Lib shift blue blue");

BlankDisplay();
    
NixieDisplay(H1,H2);
}

if (state == 20)

{
//wifi connection
  ConnectWifi();
  delay(500);
}

if (state == 21)

{
BlankDisplay();
printLocalTime();
}

if (state == 22)

{

}

if (state == 23)

{

}

if (state == 24)

{
	
}

if (state == 25)

{

}

if (state == 26)

{

}

if (state == 35)

{
Serial.println("HV Enabled");
digitalWrite(VEN, LOW);
}

if (state == 36)

{
Serial.println("HV Disabled");
digitalWrite(VEN, HIGH);
}

if (state == 37)

{
  
}

if (state == 38)

{

}

if (state == 39)

{

digitalWrite(BL, HIGH); //Disable Outputs to prevent flicker
digitalWrite(POL, HIGH); //Disable Outputs to prevent flicker
 
Serial.println("H H");

}

if (state == 40)

{



}

if (state == 41)

{

}

if (state == 42)

{

}

if (state == 43)

{
	Serial.println("PWM fixed");
    
	Serial.println(dutyCycle);
	
	//Changing the PWM
    ledcWrite(PWMChannel, dutyCycle);	
}

if (state == 44)

{

}

if (state == 45)

{
	Serial.println("All in One first boot");
	
	//Blank Display
	BlankDisplay();
	delay(500);
	
	//Adjust voltage of the HV
	Serial.println("PWM fixed");
    ledcWrite(PWMChannel, dutyCycle);
	delay(500);
	
	//Turn on HV
	Serial.println("HV Enabled");
	digitalWrite(VEN, LOW);
	delay(500);
	
	//conect to the wifi and retrieve the time
	//wifi connection
	ConnectWifi();
	delay(500);
	
	//show time
	//printLocalTime();
	//delay(500);
	
	//shut HV off
	Serial.println("HV Disabled");
	digitalWrite(VEN, HIGH);
	delay(500);
	
	//stop
	Serial.println("Done");	
	delay(500);
}

if (state == 46)

{
	Serial.println("Button Press");
	
	//Blank Display
	BlankDisplay();
	delay(500);
	
	//Adjust voltage of the HV
	Serial.println("PWM fixed");
    ledcWrite(PWMChannel, dutyCycle);
	delay(500);
	
	//turn HV on
	Serial.println("HV Enabled");
	digitalWrite(VEN, LOW);
	delay(500);
	
	//show time
	printLocalTime();
	delay(500);
	
	//shut HV
	Serial.println("HV Disabled");
	digitalWrite(VEN, HIGH);
	delay(500);
	
	//done
	Serial.println("Done");	
	delay(500);
}

//------------------------------------------------------------------------
}
}
//------------------------------------------------------------------------

//Functions

void light1(){
//to light 1
  Serial.println(1);

  digitalWrite(BL, HIGH); //Disable Outputs to prevent flicker
  digitalWrite(POL, HIGH); //Disable Outputs to prevent flicker

  digitalWrite(LE, LOW);

//------------------------
//acenda a luz na posição 1
  digitalWrite(DATA, HIGH);
  digitalWrite(CLOCK, HIGH);
  digitalWrite(CLOCK, LOW);
  digitalWrite(DATA, LOW);
  
//------------------------
//envia os dados para os pins
  digitalWrite(LE, HIGH);
  digitalWrite(LE, LOW);
}
//------------------------------------------------------------------------


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


//show digits
void NixieDisplay(uint8_t H1,uint8_t H2){
  // set pins without immediate update
  Serial.println(H1);
  Serial.println(tube2[H1]);
  sr.setNoUpdate(tube2[H1], HIGH);
  Serial.println(H2);
  Serial.println(tube1[H2]);
  sr.setNoUpdate(tube1[H2], HIGH);
  // at this point of time, pin 0 and 1 did not change yet
  sr.updateRegisters(); // update the pins to the set values
}
//------------------------------------------------------------------------


//1 by 1 transparent latch
void sequence(){
Serial.println("1 by 1 transparent latch");
  
 //to everyone
//BL high POL high
//LE high 

digitalWrite(BL, HIGH);
digitalWrite(POL, HIGH);
digitalWrite(LE, HIGH);

//send data 1
//DATA high 
//CLOCK high to low
digitalWrite(DATA, HIGH);
digitalWrite(CLOCK, HIGH);
digitalWrite(CLOCK, LOW);

delay(1000);

  //  Clock in the LOW 32  times
  for (uint8_t i = 0; i < 32 ; i++)
  { 
//Serial.println( i); 
//show data 0
//DATA low 
//CLOCK high to low
digitalWrite(DATA, LOW);
digitalWrite(CLOCK, HIGH);
digitalWrite(CLOCK, LOW);
delay(1000); // Slow this step down by 20ms so we can see each output turning on
  }
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

//retrieve time and show time
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

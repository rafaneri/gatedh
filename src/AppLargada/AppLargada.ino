/* AppLargada: Start the chronometer and send data using nRF24L01
 - WHAT IT DOES: Reads Analog values on A0 and transmits
   them over a nRF24L01 Radio Link to another transceiver when the read value is smaller than trash holder value.
 - SEE the comments after "//" on each line below
 - CONNECTIONS: nRF24L01 Modules See:
 http://arduino-info.wikispaces.com/Nrf24L01-2.4GHz-HowTo
   1 - GND
   2 - VCC 3.3V !!! NOT 5V
   3 - CE to Arduino pin 9
   4 - CSN to Arduino pin 10
   5 - SCK to Arduino pin 13
   6 - MOSI to Arduino pin 11
   7 - MISO to Arduino pin 12
   8 - UNUSED
   - 
   Analog LDR:
   GND to Arduino GND
   VCC to Arduino +5V
   Signal to Arduino A0
   - 
   Digital Laser Led:
   GND to Arduino GND
   VCC to Arduino +5V
   Signal to Arduino pin 7
   
 - V1.00 11/26/13
   Based on examples at http://www.bajdi.com/
   Questions: terry@yourduino.com */

/*-----( Import needed libraries )-----*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10
#define LDR A0
#define LED 7

// NOTE: the "LL" at the end of the constant is "LongLong" type
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL };


/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
bool listenerStart = false;
bool waitToReceive = false;
int photoLimit;
int trashhold = -50;
// serial instruction codes
// 1 - need configurate laser
// 2 - finish configuration
// 3 - wait for start ride 
// 4 - cancel ride
int serialInputInstruction; 

void setup()
{
  Serial.begin(57600);
  initRadio();
  initLaser();
}

void loop()
{
  if(Serial.available()) {
    
    Serial.print("Serial Available: ");    
    serialInputInstruction = Serial.read();    
    Serial.println(serialInputInstruction);
    
    switch (serialInputInstruction) {
      case 1:
        configureLaserPoint();
        break;
      case 2:
        finishLaserConfiguration();
        break;
      case 3:
        startWaitRide();
        break;
      case 4:
        stopWaitRide();
        break;
    }
  }
  
  if(listenerStart)
  {
    if(analogRead(LDR) < photoLimit)
    {
      Serial.print("PILOTO LARGOU: ");
      unsigned long time = millis();
      Serial.println(time);
      
      bool ok = radio.write( &time, sizeof(unsigned long));
      
      if (ok)
      {
        Serial.println("ok...");
        configureRadioToReceive();
      }
      else
      {
        Serial.println("failed.\n\r");
      }
      
      stopWaitRide();
    }
  }
  
  if(waitToReceive)
  {
    if ( radio.available() )
    {
      // Dump the payloads until we've gotten everything
      unsigned long got_time;
      bool done = false;
      while (!done)
      {
        // Fetch the payload, and see if this was the last one.
        done = radio.read( &got_time, sizeof(unsigned long) );

        // Spew it
        printf("Got payload %lu...",got_time);

	// Delay just a little bit to let the other unit
	// make the transition to receiver
	delay(20);
      }
      radio.stopListening();
    }
  }
}

void initLaser()
{
  pinMode(LED, OUTPUT);
  turnLedOff();
}

void initRadio()
{
  radio.begin();
}

void startWaitRide()
{
  configurePhotoLimit();
  delay(500);
  turnLedOn();
  delay(500);
  configureRadioToTransmit();
  listenerStart = true;
}

void stopWaitRide()
{
  listenerStart = false;
  radio.stopListening();
  turnLedOff();
}

void configureRadioToTransmit()
{
  radio.stopListening();
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  waitToReceive = false;
}

void configureRadioToReceive()
{
  radio.startListening();
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
  waitToReceive = true;
}

void configurePhotoLimit()
{
  turnLedOn(); // turn the led on
  delay(500); //wait for the laser ligth focus the LDR surface
  photoLimit = analogRead(LDR) + trashhold; //read the ldr value
  turnLedOff(); // turn the led off
}

void configureLaserPoint()
{
  turnLedOn();
}

void finishLaserConfiguration()
{
  turnLedOff();
}

void turnLedOn()
{
  chageLedState(HIGH);
}

void turnLedOff()
{
  chageLedState(LOW);
}

void chageLedState(int state)
{
  digitalWrite(LED, state);
}

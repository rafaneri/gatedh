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
   
 - V1.00 14/02/15

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
const uint64_t pipes[2] = { 0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL }; // Define the transmit pipe


/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
bool listenerStart = false; // Define the wait start ride
bool listenerFinish = false; // Define the wait for the finish ride message
int photoLimit;
int trashhold = -30;
unsigned long startTime;
unsigned long finishTime;
unsigned long realTime;
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
    serialInputInstruction = Serial.read();    
    
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
    checkIfStartRide();
  }
  
  if(listenerFinish)
  {
    checkIfFinishRide();
  }
}

void checkIfStartRide()
{
    if(analogRead(LDR) < photoLimit)
    {
      startTime = millis();

      stopWaitRide();
      
      bool ok = radio.write( &startTime, sizeof(unsigned long));

      if(!ok)
        Serial.println("retry...");
      
      startWaitFinishRide();      
    }
}

void checkIfFinishRide()
{
  if ( radio.available() )
  {
    finishTime = millis();
    // if has data, read the data then send message to get the rtt
    // Read the data payload until we've received everything
    bool done = false;
    unsigned long time = 0;
    while (!done)
    {
      // Fetch the data payload
      done = radio.read( &time, sizeof(unsigned long) );
    }
    
    radio.stopListening();
    
    radio.write( &time, sizeof(unsigned long) );
 
    radio.startListening();
    
    // Wait here until we get a response, or timeout (250ms)
    unsigned long started_waiting_at = millis();
    bool timeout = false;
    while ( ! radio.available() && ! timeout )
      if (millis() - started_waiting_at > 200 )
        timeout = true;

    // Describe the results
    if ( timeout )
    {
      Serial.println("Failed, response timed out.\n\r");
      realTime = (finishTime - startTime);
    }
    else
    {
      // Grab the response, compare, and send to debugging spew
      unsigned long rtt;
      radio.read( &rtt, sizeof(unsigned long) );
      realTime = (finishTime - startTime) - rtt;
    }
    
    finishRide();
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
  radio.setRetries(15,15);
  radio.setPayloadSize(8);
  radio.openWritingPipe(pipes[0]);
  radio.openReadingPipe(1,pipes[1]);
}

void startWaitRide()
{
  configurePhotoLimit();
  delay(500);
  turnLedOn();
  delay(500);
  listenerStart = true;
}

void stopWaitRide()
{
  listenerStart = false;
  turnLedOff();
}

void startWaitFinishRide()
{
  listenerFinish = true;
  radio.startListening();
}

void finishRide()
{
  listenerFinish = false;
  radio.stopListening();
  Serial.print("FIM DE DESCIDA: ");
  int segundos = realTime/1000;
  int milisegundos = realTime - (segundos * 1000);
  int minutos = segundos/60;
  segundos = segundos - (minutos*60);
  if(minutos < 10)
  Serial.print("0");
  Serial.print(minutos);
  Serial.print(":");
  if(segundos < 10)
  Serial.print("0");
  Serial.print(segundos);
  Serial.print(":");
  Serial.println(milisegundos);
  for(int i =0; i<3; i++)
  {
      turnLedOn();
      delay(10);
      turnLedOff();
      delay(10);    
  }
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

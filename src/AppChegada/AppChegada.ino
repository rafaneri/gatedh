/* AppChegada: Start the chronometer when receive data using nRF24L01
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
const uint64_t pipe = 0xE8E8F0F0E1LL; // Define the transmit pipe


/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
bool listenerStart = false;
unsigned long initTime;
int photoLimit;
int trashhold = -50;
// serial instruction codes
// 1 - need configurate laser
// 2 - finish configuration
// 3 - wait for start ride - don't work here
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
      case 4:
        stopWaitRide();
        break;
    }
  }
  
  if(listenerStart)
  {
    checkIfFinishRide();
  }
  
  if ( radio.available() )
  {
    Serial.println("RADIO");
    checkIfStartRide(); 
  }
}

void checkIfStartRide()
{
  // Read the data payload until we've received everything
    bool done = false;
    unsigned long time = 0;
    while (!done)
    {
      // Fetch the data payload
      done = radio.read( &time, sizeof(unsigned long) );
      initTime = millis();
    }
    
    startWaitRide();
}

void checkIfFinishRide()
{
  if(analogRead(LDR) < photoLimit)
  {
    Serial.print("PILOTO CHEGOU: ");
    unsigned long time = millis();
    unsigned long elapsedTime = time - initTime;
    int segundos = elapsedTime/1000;
    int milisegundos = elapsedTime - (segundos * 1000);
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
    Serial.println(elapsedTime);
    stopWaitRide();
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
  radio.openReadingPipe(1,pipe);
  radio.startListening();
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

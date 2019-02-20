/*
  This firmware using Arduino Nano Board
  water level
  0 - Almost Empty
  1 - over 30%
  2 - over 60%
  3 - over 90%

*/

#include <SPI.h>
#include <NRFLite.h>

/*radio Connection*/
#define RADIO_ID              1 // sensor radio's ID.
#define DESTINATION_RADIO_ID  0 // Monitor radio ID.
#define PIN_RADIO_CE          9
#define PIN_RADIO_CSN         10

/*sensor Connection*/
#define Level1 A0
#define Level2 A1
#define Level3 A2
#define rdL1() digitalRead(A0)
#define rdL2() digitalRead(A1)
#define rdL3() digitalRead(A2)

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId;
  uint8_t waterLevel;
};

NRFLite _radio;
RadioPacket _radioData;

void setup()
{
  Serial.begin(115200);

  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 99))
  {
    Serial.println(F("Cannot communicate with radio"));
    while (1); // Wait here forever.
  }
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  _radioData.FromRadioId = RADIO_ID;
  Serial.println(F("Smart Water Tank V.1"));
}

void loop()
{
  
  _radioData.waterLevel++;
  if (_radioData.waterLevel > 3)
    _radioData.waterLevel = 0;
  
//  if(rdL1() == 0)
//    _radioData.waterLevel = 0;
//  if(rdL1() == 1)
//    _radioData.waterLevel = 1;
//  if(rdL1() == 1 && rdL2() == 1)
//    _radioData.waterLevel = 2;
//  if(rdL1() == 1 && rdL2() == 1 && rdL3() == 1)
//    _radioData.waterLevel = 3;
  if (_radio.send(DESTINATION_RADIO_ID, &_radioData, sizeof(_radioData))) // Note how '&' must be placed in front of the variable name.
    Serial.println(F("...Success"));
  else
    Serial.println(F("...Failed"));

  delay(10000); 
}

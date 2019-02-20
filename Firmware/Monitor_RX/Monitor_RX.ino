/*
  This firmware using NodeMCU ESP8266
  water level
  0 - Almost Empty
  1 - over 30%
  2 - over 60%
  3 - over 90%

*/

#include <SPI.h>
#include <NRFLite.h>

#define USE_TGBOT 1

#if USE_TGBOT
#include "CTBot.h"
#endif

/*radio Connection*/
#define RADIO_ID              0
#define DESTINATION_RADIO_ID  1
#define PIN_RADIO_CE          9
#define PIN_RADIO_CSN         15

/*LED LVL Connection*/
#define ledLevel3   16
#define ledLevel2   5
#define ledLevel1   4
#define ledLevel0   0

/*water PUMP connection*/
#define waterPump   2

#if USE_TGBOT
CTBot myBot;
TBMessage msg;
String ssid  = "YOUR WIFI"    ; // REPLACE mySSID WITH YOUR WIFI SSID
String pass  = "YOUR WIFI PASSWORD"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "TELEGRAM BOT TOKEN"   ; // REPLACE myToken WITH YOUR TELEGRAM BOT TOKEN
#endif

struct RadioPacket // Any packet up to 32 bytes can be sent.
{
  uint8_t FromRadioId;
  uint8_t waterLevel;
};


NRFLite _radio;
RadioPacket _radioData;

unsigned long timeLapsed, poolRadio, poolTgBot;
String level = "Empty";

void setup()
{
  pinMode(ledLevel0, OUTPUT);
  pinMode(ledLevel1, OUTPUT);
  pinMode(ledLevel2, OUTPUT);
  pinMode(ledLevel3, OUTPUT);
  pinMode(waterPump, OUTPUT);
  Serial.begin(115200);

  if (!_radio.init(RADIO_ID, PIN_RADIO_CE, PIN_RADIO_CSN, NRFLite::BITRATE250KBPS, 99))
  {
    Serial.println(F("Cannot communicate with radio"));
    while (1); // Wait here forever.
  }
  Serial.println("Starting TelegramBot...");

#if USE_TGBOT
  // connect the ESP8266 to the desired access point
  myBot.wifiConnect(ssid, pass);

  // set the telegram bot token
  myBot.setTelegramToken(token);

  // check if all things are ok
  if (myBot.testConnection())
    Serial.println("\ntestConnection OK");
  else
    Serial.println("\ntestConnection NOK");
#endif

  Serial.println(F("Smart Water Tank V.1"));
}

void loop() {
  timeLapsed = millis();
  if (timeLapsed - poolRadio >= 5000) {
    poolRadio = timeLapsed;
    while (_radio.hasData()) {
      _radio.readData(&_radioData); // Note how '&' must be placed in front of the variable name.

      if (_radioData.waterLevel == 0) {
        level = "Empty";
        digitalWrite(ledLevel3, LOW);
        digitalWrite(ledLevel2, LOW);
        digitalWrite(ledLevel1, LOW);
        digitalWrite(ledLevel0, HIGH);
        digitalWrite(waterPump, LOW);
      }
      else if (_radioData.waterLevel == 1) {
        level = "Almost Empty";
        digitalWrite(ledLevel3, LOW);
        digitalWrite(ledLevel2, LOW);
        digitalWrite(ledLevel1, HIGH);
        digitalWrite(ledLevel0, HIGH);
        digitalWrite(waterPump, LOW);
      }
      else if (_radioData.waterLevel == 2) {
        level = "Almost Full";
        digitalWrite(ledLevel3, LOW);
        digitalWrite(ledLevel2, HIGH);
        digitalWrite(ledLevel1, HIGH);
        digitalWrite(ledLevel0, HIGH);
        digitalWrite(waterPump, LOW);
      }
      else if (_radioData.waterLevel == 3) {
        level = "Full";
        digitalWrite(ledLevel3, HIGH);
        digitalWrite(ledLevel2, HIGH);
        digitalWrite(ledLevel1, HIGH);
        digitalWrite(ledLevel0, HIGH);
        digitalWrite(waterPump, HIGH);
      }

      String msg = "Radio ";
      msg += _radioData.FromRadioId;
      msg += ", waterLevel = ";
      msg += _radioData.waterLevel;

      Serial.println(msg);
    }
  }

#if USE_TGBOT
  if (timeLapsed - poolTgBot >= 500) {
    poolTgBot = timeLapsed;
    if (myBot.getNewMessage(msg)) {
      Serial.println("Telegram cmd");
      if (msg.text.equalsIgnoreCase("Level")) {
        String reply;
        reply = (String)"Hi " + msg.sender.username + (String)", water level is : " +  level;
        myBot.sendMessage(msg.sender.id, reply);
      }
      else {      // otherwise...
        myBot.sendMessage(msg.sender.id, (String)"Unknown Command");             // and send it
      }
    }
  }
#endif
}

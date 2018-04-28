// Feather9x_TX
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messaging client (transmitter)
// with the RH_RF95 class. RH_RF95 class does not provide for addressing or
// reliability, so you should only use RH_RF95 if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example Feather9x_RX

// Send voltage
// https://learn.adafruit.com/adafruit-feather-m0-adalogger/power-management
 
#include <SPI.h>
#include <RH_RF95.h>
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson

#define DEVID 6

#define RESOLUTION 4096

  // for feather m0  
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3


// Change to 434.0 or other frequency, must match RX's freq!
//#define RF95_FREQ 915.0
#define RF95_FREQ 868.0
 

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

// Voltage PIN
#define VBATPIN A7


void setup() 
{


    analogReadResolution(12); // on M0 only .. 


  pinMode(LED, OUTPUT);
  //pinMode(RFM95_RST, OUTPUT);
  //digitalWrite(RFM95_RST, HIGH);
 
//  while (!Serial);
  Serial.begin(9600);
  delay(100);
 
  Serial.println(" LoRa TX Test!");


 // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);


  while (!rf95.init()) {
    Serial.println("LoRa radio init failed");
    while (1);
  }
  Serial.println("LoRa radio init OK!");
 
  // Defaults after init are 434.0MHz, modulation GFSK_Rb250Fd250, +13dbM
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
    while (1);
  }
  Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);
  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
 
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 5 to 23 dBm:
  rf95.setTxPower(23, false);
}
 
int16_t packetnum = 0;  // packet counter, we increment per xmission
 
void loop()
{ 

  float h = 23.;
  // Read temperature as Celsius (the default)
  float t = 31.6;

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  float a0= analogRead(A0);
  float a1= analogRead(A1);
  float a2 = analogRead(A2);
  float a3 = analogRead(A3);
  float a4 = analogRead(A4);
  float a5 = analogRead(A5);



  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["id"] = DEVID;
  root["packetnum"] = packetnum;
  JsonObject& data = root.createNestedObject("data");
  data["temp"] = double_with_n_digits(t,2);
  data["humidity"] = double_with_n_digits(h,2);
  data["A0"] = analogRead(A0);
  data["A1"] = analogRead(A1);
  data["A2"] = analogRead(A2);
  data["A3"] = analogRead(A3);
  data["A4"] = analogRead(A4);
  data["A5"] = analogRead(A5);

  char buf[251];
  root.printTo(buf, sizeof(buf));
  buf[sizeof(buf)-1] = 0;
  
  Serial.print("Sending "); Serial.println(buf);


  rf95.send((uint8_t *)buf, sizeof(buf));

  
  Serial.println("Waiting for packet to complete..."); delay(10);
    
  rf95.waitPacketSent();

  packetnum++; // update the packet number
  
  
     digitalWrite(LED,HIGH);
    delay(50);
    digitalWrite(LED, LOW);

  delay(8000);
  
}

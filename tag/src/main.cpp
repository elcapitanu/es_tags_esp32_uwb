#include <Arduino.h>

#include "SPI.h"
#include "DW1000Ranging.h"

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

#define TAG_ADD "7D:00:22:EA:82:60:3B:9C"

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

uint8_t new13 = 0, new14 = 0;
float dist13 = 0, dist14 = 0;

void newRange()
{
  int device = DW1000Ranging.getDistantDevice()->getShortAddress();
  float dist = DW1000Ranging.getDistantDevice()->getRange();
  DW1000Ranging.getDistantDevice()->getRXPower();

  if (dist >= 0 && dist <= 25)
  {
    if (device == 0x1313)
    {
      dist13 = dist;
      new13 = 1;
    }
    else if (device == 0x1314)
    {
      dist14 = dist;
      new14 = 1;
    }
  }
}

void newDevice(DW1000Device *device)
{
  device->getShortAddress();
}

void inactiveDevice(DW1000Device *device)
{
  device->getShortAddress();
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  // init the configuration
  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); // Reset, CS, IRQ pin
  // define the sketch as anchor. It will be great to dynamically change the type of module
  DW1000Ranging.attachNewRange(newRange);
  DW1000Ranging.attachNewDevice(newDevice);
  DW1000Ranging.attachInactiveDevice(inactiveDevice);
  // Enable the filter to smooth the distance
  DW1000Ranging.useRangeFilter(true);

  // we start the module as a tag
  DW1000Ranging.startAsTag(TAG_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER);
}

void loop()
{
  DW1000Ranging.loop();

  if (new13 && new14)
  {
    Serial.print("$DIST,");
    Serial.print(dist13, 3);
    Serial.print(",");
    Serial.print(dist14, 3);
    Serial.println(",*");

    new13 = new14 = 0;
  }
}
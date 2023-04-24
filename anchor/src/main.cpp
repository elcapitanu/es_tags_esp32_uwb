#include <SPI.h>
#include "DW1000Ranging.h" //https://github.com/thotro/arduino-dw1000

#define ANCHOR_ADD "13:13:5B:D5:A9:9A:E2:90" //13
//#define ANCHOR_ADD "14:13:5B:D5:A9:9A:E2:90" // 14

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define DW_CS 4

// connection pins
const uint8_t PIN_RST = 27; // reset pin
const uint8_t PIN_IRQ = 34; // irq pin
const uint8_t PIN_SS = 4;   // spi select pin

// calibrated Antenna Delay setting for this anchor
uint16_t Adelay = 16539;

// previously determined calibration results for antenna delay
//  #13:13      #14:13
// #1 16529     16530
// #2 16538     16533
// #3 16533     16529
// #4 16539     16536

// calibration distance
float dist_m =1; //meters

void newRange()
{
    Serial.print("from: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getShortAddress(), HEX);
    Serial.print("\t Range: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRange());
    Serial.print(" m");
    Serial.print("\t RX power: ");
    Serial.print(DW1000Ranging.getDistantDevice()->getRXPower());
    Serial.println(" dBm");
}

void newBlink(DW1000Device *device)
{
    Serial.print("blink; 1 device added ! -> ");
    Serial.print(" short:");
    Serial.println(device->getShortAddress(), HEX);
}

void inactiveDevice(DW1000Device *device)
{
    Serial.print("delete inactive device: ");
    Serial.println(device->getShortAddress(), HEX);
}

void setup()
{
    Serial.begin(115200);
    delay(1000);
    // init the configuration
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    DW1000Ranging.initCommunication(PIN_RST, PIN_SS, PIN_IRQ); // Reset, CS, IRQ pin

    // set antenna delay for anchors only. Tag is default (16384)
    DW1000.setAntennaDelay(Adelay);

    // define the sketch as anchor. It will be great to dynamically change the type of module
    DW1000Ranging.attachNewRange(newRange);
    DW1000Ranging.attachBlinkDevice(newBlink);
    DW1000Ranging.attachInactiveDevice(inactiveDevice);
    // Enable the filter to smooth the distance
    DW1000Ranging.useRangeFilter(true);

    DW1000Ranging.startAsAnchor(ANCHOR_ADD, DW1000.MODE_LONGDATA_RANGE_LOWPOWER, false);
}

void loop()
{
    DW1000Ranging.loop();
}
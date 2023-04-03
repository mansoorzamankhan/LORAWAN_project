#include <SPI.h>
#include <LoRa.h>
#define ss 5
#define rst 4
#define dio0 2
int counter = 0;

void setup() {
  Serial.begin(9600);
  LoRa.setPins(ss, rst, dio0);
  while (!Serial)
    ;

  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
}

void loop() {


  if (counter % 2 == 2) {
    LoRa.setSyncWord(0xF3);
    Serial.print("sent packet node 1:");
    Serial.println(counter);
    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(counter);
    LoRa.endPacket();
  } else {
    LoRa.setSyncWord(0x00);
    Serial.print("sent packet node 2: ");
    Serial.println(counter);

    LoRa.beginPacket();
    LoRa.print("hello ");
    LoRa.print(counter);
    LoRa.endPacket();
  }
  counter++;

  delay(500);
}

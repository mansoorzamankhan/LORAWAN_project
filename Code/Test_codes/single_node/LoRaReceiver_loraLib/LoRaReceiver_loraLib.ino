#include <SPI.h>
#include <LoRa.h>
#define ss   5
#define rst  4
#define dio0 2
void setup() {
  LoRa.setPins(ss, rst, dio0);
  Serial.begin(9600);
  while (!Serial);

  Serial.println("LoRa Receiver");
  LoRa.setSyncWord(0xF3);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet from server'");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

  }
}

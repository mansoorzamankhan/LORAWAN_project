/*Multiple LoRa Communication
  example code by miliohm.com */

#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;

void setup()
{
  Serial.begin(9600);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  //rf95.setFrequency(433.0);
}

void loop()
{
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (rf95.waitAvailableTimeout(3000))
  {
    // Should be a reply message for us now
    if (rf95.recv(buf, &len))
    {
      int dataLength;
      String Request = (char*)buf;
      Serial.println(Request);
      if (Request == "C1") {
        Serial.print("Client 1 Got Request, Answering Server...");
        String data = "Hello";
        uint8_t total[dataLength]; //variable for data to send
        data.toCharArray(total, dataLength); //change type data from string ke uint8_t
        Serial.println(data);
        rf95.send(total, dataLength); //send data
        rf95.waitPacketSent();
      }
    }
  }
  else
  {
    Serial.println("No reply, is rf95_server running?");
  }
  delay(400);
}
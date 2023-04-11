/*Multiple LoRa Communication
  example code by miliohm.com */
#include <SPI.h>
#include <RH_RF95.h>
RH_RF95 rf95;
int led = 13;
unsigned long int millisBefore;
int turn = 1;
boolean statusRepeater = 0;
void setup()
{
  pinMode(led, OUTPUT);
  Serial.begin(9600);
  RH_RF95 driver(5, 2);
  while (!Serial) ; // Wait for serial port to be available
  if (!rf95.init())
    Serial.println("init failed");
  Serial.println("Starting...");
}
void loop()
{
  if (statusRepeater == 0) {
    if (millis() - millisBefore < 1000) { 
      if (turn == 1) {
        Serial.println("Send Request 1");
        SendRequest("C1"); 
        waitForAnswer();
        turn = 2;
      }
      waitForAnswer();
    } else if ((millis() - millisBefore > 1000) && (millis() - millisBefore < 2000)) { //jika waktu kurang dari 4 detik
      if (turn == 2) {
        Serial.println("Send Request 2");
        SendRequest("G2");
        waitForAnswer();
        turn = 3;
      }
    } else if (millis() - millisBefore > 6000) {
      if (turn == 3) {
        Serial.println("Send Request 3");
        SendRequest("G3");
        waitForAnswer();
        SendRequest("G1");
        waitForAnswer();
        millisBefore = millis();
        turn = 1;
      }
    }
  }
}
void waitForAnswer() { //fungsi tunggu jawaban dari node
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);
  if (rf95.waitAvailableTimeout(500))
  {
    if (rf95.recv(buf, &len))
    {
      Serial.print("Received at Server: ");
      Serial.println((char*)buf);
      String dataTotal = (char*)buf;
      Serial.println(dataTotal);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}
void SendRequest(String request) { //fungsi untuk Send request dengan variable parameter request
  String dataTotal = request;
  int dataLength = dataTotal.length(); dataLength ++;
  uint8_t total[dataLength];
  dataTotal.toCharArray(total, dataLength);
  rf95.send(total, dataLength);
  rf95.waitPacketSent();
}
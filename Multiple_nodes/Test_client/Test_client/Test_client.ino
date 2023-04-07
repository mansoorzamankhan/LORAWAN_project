/*
 Lora Node1
https://www.electroniclinic.com/

*/
#include <SPI.h>  // include libraries
#include <LoRa.h>
#define ss 5
#define rst 4
#define dio0 2

String outgoing;  // outgoing message
String SenderNode = "";
byte msgCount = 0;  // count of outgoing messages
byte MasterNode = 0xFF; // master/server node adress
byte Node1 = 0xBB;// client/this deveice  adress 


void setup() {

  LoRa.setPins(ss, rst, dio0);
  Serial.begin(9600);
  while (!Serial)
    ;

  Serial.println("LoRa client(1)");
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  } else { // on power on send indication message to server
    String message = "power_on(Node1)";
    sendMessage(message, MasterNode, Node1);
    delay(100);
  }
}

void loop() {


  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());// receive data from server
}

void sendMessage(String outgoing, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();             // start packet
  LoRa.write(MasterNode);         // add destination address
  LoRa.write(Node1);              // add sender address
  LoRa.write(msgCount);           // add message ID
  LoRa.write(outgoing.length());  // add payload length
  LoRa.print(outgoing);           // add payload
  LoRa.endPacket();               // finish packet and send it
  msgCount++;                     // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;  // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();  // recipient address
  byte sender = LoRa.read();    // sender address
  if (sender == 0XFF)
    SenderNode = "Master";
  byte incomingMsgId = LoRa.read();   // incoming msg ID
  byte incomingLength = LoRa.read();  // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {  // check length for error
    Serial.println("error: message length does not match length");
    ;
    return;  // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != Node1 && recipient != MasterNode) {
    //Serial.println("This message is not for me.");
    ;
    return;  // skip rest of function
  }
  if (sender == 0xFF) {
    Serial.print(incoming);// message from serer 
    // print RSSI of packet
    Serial.print("' Signal Power ");
    Serial.println(LoRa.packetRssi());// find signal power 

    String message = "received(Node1)";//acknowledgemet 
    sendMessage(message, MasterNode, Node1);//send acknowledgemet message to server
    delay(100);
  }
}
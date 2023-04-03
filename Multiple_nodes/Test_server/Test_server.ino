/*
  Master Lora Node
 

*/
#include <SPI.h>  // include libraries
#include <LoRa.h>




byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;
String Message= "";
String SenderNode = "";
String outgoing;  // outgoing message

byte msgCount = 0;  // count of outgoing messages

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
int interval = 5;  // updated every 1 second
int Secs = 0;
#define ss 5
#define rst 4
#define dio0 2

void setup() {
  //LoRa.setPins(ss, rst, dio0);
  Serial.begin(9600);
  LoRa.setPins(ss, rst, dio0);
  while (!Serial)
    ;

  Serial.println("LoRa server");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1)
      ;
  }
}

void loop() {


  
    Message = " message from server to node 1 ";
    sendMessage(Message, MasterNode, Node1);
    //Serial.println("message sent to node 1 ");
    Serial.println("Sending " + Message);
  Message = "";     

    delay (5000);
  

    Message = "  message from server to node 2";
    sendMessage(Message, MasterNode, Node2);
    //Serial.println("message sent to node 2 ");
    Serial.println("Sending " + Message);
    Message ="";

    delay (5000);


    // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());
}


void sendMessage(String outgoing, byte MasterNode, byte otherNode) {
  LoRa.beginPacket();             // start packet
  LoRa.write(otherNode);          // add destination address
  LoRa.write(MasterNode);         // add sender address
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
  if (sender == 0XBB)
    SenderNode = "Node1:";
  if (sender == 0XCC)
    SenderNode = "Node2:";
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
    Serial.println("This message is not for me.");
    ;
    return;  // skip rest of function
  }




  if (sender == 0XBB) {

    Serial.print(incoming);
    Serial.println("from node 1");
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }

  if (sender == 0XCC) {
    Serial.print(incoming);
    Serial.println("from node 2");
    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
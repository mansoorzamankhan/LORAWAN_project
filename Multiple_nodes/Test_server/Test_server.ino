/*
  Master Lora Node
 

*/
#include <SPI.h>  // include libraries
#include <LoRa.h>
bool ack_node1 = false;
bool ack_node2 = false;
bool timeout_flag = false;
bool start_time_flag = true;
int timeout_counter = 0;

long long int attempts = 0;
int timeout = 20000;
byte MasterNode = 0xFF;
byte Node1 = 0xBB;
byte Node2 = 0xCC;
String Message = "";
String SenderNode = "";
String outgoing;  // outgoing message

byte msgCount = 0;  // count of outgoing messages

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
unsigned long int start_time = 0;
int interval = 5;  // updated every 1 second
int Secs = 0;
#define ss 5
#define rst 4
#define dio0 2
int counter1 = 0;
int counter2 = 0;
void setup() {
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
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    Secs = Secs + 1;
    //Serial.println(Secs);
    if (Secs >= 11) {
      Secs = 0;
    }
    if ((Secs >= 1) && (Secs <= 5)) {
      String message1 = "00000000";
      if (ack_node1 == false ) {
        sendMessage(message1, MasterNode, Node1);
        Serial.println("message send to node 1 ");

      }
    }

    if ((Secs >= 6) && (Secs <= 10)) {

      String message2 = "00000000";
      if (ack_node2 == false) {
        sendMessage(message2, MasterNode, Node2);
        //Serial.println("message send to node 2");
      }
    }

    previoussecs = currentsecs;
  }

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
  if (packetSize == 0) {

    return;  // if there's no packet, return
  }

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



  //from node 1
  if (sender == 0XBB) {


    if (incoming == "received(1)") {
      Serial.print(incoming);
      // print RSSI of packet
      Serial.print("' Signal power  ");
      Serial.println(LoRa.packetRssi());
      ack_node1 = true;
    }
  }
  //from node 2
  if (sender == 0XCC) {
    if (incoming == "received(2)") {
      Serial.print(incoming);
      // print RSSI of packet
      Serial.print("' Signal power  ");
      Serial.println(LoRa.packetRssi());
      ack_node2 = true;
    }
  }
}
/*
  Master Lora Node
 

*/




#include <SPI.h>  // include libraries
#include <LoRa.h>
bool ack_node1 = false;                                                                       // acknowledgement of received message from node 1
bool ack_node2 = false;                                                                       // acknowledgement of received message from node 2
int valve_input_f1[16] = { 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42, 44, 46, 48, 50, 52 };  //field 1 valves input signal pins
int valve_input_f2[16] = { 23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 47, 49, 51, 53 };  //field 2 valves input signal pins
int valve_data_f1[16] = {};                                                                   // save state of vavles for field 1
int valve_data_f2[16] = {};                                                                   // save state if vlaves for field 2
int j = 0;                                                                                    // iteratr for node 1 data
int k = 0;                                                                                    //iterator for node 2 data
bool node1_data[8] = { false };                                                               // bolian array for valves
bool node2_data[8] = { false };                                                               // bolian array for valves
String message1 = "";
String message2 = "";
byte MasterNode = 0xFF;  // address for master/server node
byte Node1 = 0xBB;       // client node 1
byte Node2 = 0xCC;       //client node 2
String Message = "";
String SenderNode = "";
String outgoing;    // outgoing message
byte msgCount = 0;  // count of outgoing messages

// Tracks the time since last event fired
unsigned long previousMillis = 0;
unsigned long int previoussecs = 0;
unsigned long int currentsecs = 0;
unsigned long currentMillis = 0;
unsigned long int start_time = 0;
int interval = 5;  // updated every 1 second
int Secs = 0;
// //for arduino 
// #define ss 10
// #define rst 9
// #define dio0 2

//for esp32 
#define ss 5
#define rst 4
#define dio0 2

int counter1 = 0;
int counter2 = 0;
void setup() {
  Serial.begin(115200);
  // define int pins
  for (int i = 0; i < 16; i++) {
    pinMode(valve_input_f1[i], INPUT_PULLUP);
    pinMode(valve_input_f2[i], INPUT_PULLUP);
  }
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
void read_valves_status(void) {
  for (int i = 0; i < 16; i++) {
    valve_data_f1[i] = digitalRead(valve_input_f1[i]);
    valve_data_f2[i] = digitalRead(valve_input_f2[i]);
  }
}
void encode_message(void) {

  for (int i = 0; i < 16; i + 2) {

    if (valve_data_f1[i] == 1 && valve_data_f1[i + 1] == 0) {
      node1_data[j] = true;
      j++;
    } else if (valve_data_f1[i] == 0 && valve_data_f1[i + 1] == 1) {
      node1_data[j] = false;
      j++;
    }

    if (valve_data_f2[i] == 1 && valve_data_f2[i + 1] == 0) {
      node2_data[k] = true;
      k++;
    } else if (valve_data_f2[i] == 0 && valve_data_f2[i + 1] == 1) {
      node2_data[k] = false;
      k++;
    }
  }
  for (int i = 0; i < sizeof(node2_data) / sizeof(node2_data[0]); i++) {
    message1 += String(node1_data[i]);
    message2 += String(node2_data[i]);
    Serial.print("Valves Status for node 1 :");
    Serial.println(message1);
    Serial.print("Valves Status for node 2 :");
    Serial.println(message2);
    
  }

}
void add_parity(String msg1 , String msg2)
{

}
void loop() {
  read_valves_status();
  encode_message();
  currentMillis = millis();
  currentsecs = currentMillis / 1000;
  // send paket time (11 second )
  if ((unsigned long)(currentsecs - previoussecs) >= interval) {
    Secs = Secs + 1;
    //Serial.println(Secs);
    if (Secs >= 11) {  // reset time after 11 seconds
      Secs = 0;
    }
    if ((Secs >= 1) && (Secs <= 5)) {  // in first 5 second send data to node 1
      // message1 = "00000000";
      if (ack_node1 == false) {                    // if message is not received by the receiving node1
        sendMessage(message1, MasterNode, Node1);  // send message to node1
        Serial.println("message send to node 1 ");
        message1 = ""; // clear message string after sending message
      }
    }

    if ((Secs >= 6) && (Secs <= 10)) {  // in next 5 seconds send data to node 2

      //message2 = "00000000";
      if (ack_node2 == false) {                    // if message is not received by the receiving node2
        sendMessage(message2, MasterNode, Node2);  // send message to node2
        Serial.println("message send to node 2");
        message2 = ""; // clear message string after sending message
      }
    }

    previoussecs = currentsecs;
  }

  // parse for a packet, and call onReceive with the result:
  onReceive(LoRa.parsePacket());  // receive data from client nodes
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
    if (incoming == "received(Node1)") {
      Serial.print(incoming);
      // print RSSI of packet
      Serial.print("' Signal power  ");
      Serial.println(LoRa.packetRssi());
      ack_node1 = true;
    }
    if (incoming == "power_on(Node1)") {
      Serial.println("Node 1 :powered on");
      // print RSSI of packet
      ack_node1 = false;
    }
  }
  //from node 2
  if (sender == 0XCC) {
    if (incoming == "received(Node2)") {
      Serial.print(incoming);
      // print RSSI of packet
      Serial.print("' Signal power  ");
      Serial.println(LoRa.packetRssi());
      ack_node2 = true;
    }
    if (incoming == "power_on(Node2)") {
      Serial.println("Node 2 :powered on");
      // print RSSI of packet
      ack_node2 = false;
    }
  }
}

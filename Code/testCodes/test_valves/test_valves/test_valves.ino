


void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
pinMode(22, INPUT_PULLUP);
pinMode(24, INPUT_PULLUP);
pinMode(26, INPUT_PULLUP);
pinMode(28, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
Serial.print("valve 1 ");
Serial.print(digitalRead(22));
Serial.println(digitalRead(24));

Serial.print("valve 2 ");
Serial.print(digitalRead(26));
Serial.println(digitalRead(28));
delay(1000);

}

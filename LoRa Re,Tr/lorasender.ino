
HardwareSerial &ss = Serial2;


void setup() {
  Serial.begin(115200);
  ss.begin(9600);
  // Wire.begin();
}

void loop() {
  ss.println("LOLOL");
  
  delay(200);
}
  
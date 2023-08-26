// lora

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SparkFun_u-blox_GNSS_v3.h>
#include <SD.h> 

#define BME280_I2C_ADDR1 0x76
#define BME280_I2C_ADDR2 0x77

Adafruit_BME280 bme1;
Adafruit_BME280 bme2;
const int lm35Pin = A0;
HardwareSerial &ss = Serial1;

String p_com = "";
SFE_UBLOX_GNSS myGNSS; // Move the declaration of myGNSS to the global scope

const int pumpPin = 9;
unsigned long startTimePump = 0;
bool pumpRunning = false;
const int fanPin = 7; // Changed fanPin to 7 to avoid conflicts
unsigned long startTimeFan = 0;
bool fanRunning = false;

const int chipSelect = 10; 
File dataFile; 

float getLM35Temperature() {
  float voltage = analogRead(lm35Pin) * 5.0 / 1024.0;
  return voltage;
}

void turnOnPump() {
  digitalWrite(pumpPin, HIGH);
}

void turnOffPump() {
  digitalWrite(pumpPin, LOW);
}

void turnOnFan() {
  digitalWrite(fanPin, HIGH);
}

void turnOffFan() {
  digitalWrite(fanPin, LOW);
}

void setup() {
  pinMode(pumpPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  Wire.begin();
  Serial.begin(115200);
  ss.begin(9600);

  if (!bme1.begin(BME280_I2C_ADDR1)) {
    Serial.println("Could not find a valid BME280 sensor. Check wiring!");
    while (1);
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized successfully!");

  dataFile = SD.open("data.txt", FILE_WRITE);

  if (dataFile) {
    Serial.println("Data file opened successfully!");
    dataFile.println("Hello, this is a sample text."); 
    dataFile.close();
    Serial.println("Text data written to the SD card.");
  } else {
    Serial.println("Error opening data file.");
  }
}

void loop() {
  float temp1 = bme1.readTemperature();
  float temp2 = bme2.readTemperature();
  float temp3 = getLM35Temperature();
  float press1 = bme1.readPressure() / 100.0;
  float press2 = bme2.readPressure() / 100.0;
  float hum1 = bme1.readHumidity();
  float hum2 = bme2.readHumidity();
  float latitude = myGNSS.getLatitude();
  float longitude = myGNSS.getLongitude();
  float altitude = myGNSS.getAltitudeMSL();
  

  p_com += String(temp1) + ',';
  p_com += String(temp2) + ',';
  p_com += String(temp3) + ',';
  p_com += String(press1) + ',';
  p_com += String(press2) + ',';
  p_com += String(hum1) + ',';
  p_com += String(hum2) + ',';
  p_com += String(latitude) + ',';
  p_com += String(longitude) + ',';
  p_com += String(altitude) ;

  ss.println(p_com);
  Serial.println(p_com);

  if (altitude >= 25 || temp2 <= -50) {
    if (!pumpRunning) {
      turnOnPump();
      startTimePump = millis();
      pumpRunning = true;
    }
  } else {
    if (pumpRunning && millis() - startTimePump >= 300000) {
      turnOffPump();
      pumpRunning = false;
    }
  }

  if (temp1 >= 60) {
    if (!fanRunning) {
      turnOnFan();
      fanRunning = true;
    }
  } else {
    if (fanRunning) {
      turnOffFan();
      fanRunning = false;
    }
  }

  delay(1000);
}

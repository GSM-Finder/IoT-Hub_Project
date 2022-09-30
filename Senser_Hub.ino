#include <SoftwareSerial.h>
#include "DHT.h"

#define DHTPIN 7
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

SoftwareSerial BT(2, 3);

void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  dht.begin();
}

void loop() {
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();

  String humi_string = String(humi);
  String temp_string = String(temp);

  Serial.println("h" + humi_string);
  BT.print("h" + humi_string);
  delay(1100);
  Serial.println("t" + temp_string);
  BT.print("t" + temp_string);
  delay(1100);
}
  

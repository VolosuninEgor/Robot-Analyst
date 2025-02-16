#include "DHT.h"
#include <BH1750.h>                          
#include <Wire.h> 

#define DHT22_PIN 3
#define PIN_TRIG1 12
#define PIN_ECHO1 11
#define PIN_TRIG2 10
#define PIN_ECHO2 9
#define PIN_TRIG3 7
#define PIN_ECHO3 8
#define PIN_TRIG4 5
#define PIN_ECHO4 6

int duration1, cm1, duration2, cm2, duration3, cm3, duration4, cm4;
float lux, humi, tempC;

DHT dht22(DHT22_PIN, DHT22);
BH1750 lightMeter;                           

void setup() {
  Serial.begin(9600);
  dht22.begin();

  Wire.begin();                              
  lightMeter.begin();
  pinMode(PIN_TRIG1, OUTPUT);
  pinMode(PIN_ECHO1, INPUT);
  pinMode(PIN_TRIG2, OUTPUT);
  pinMode(PIN_ECHO2, INPUT);
  pinMode(PIN_TRIG3, OUTPUT);
  pinMode(PIN_ECHO3, INPUT);
  pinMode(PIN_TRIG4, OUTPUT);
  pinMode(PIN_ECHO4, INPUT);
}
void loop() {
  humi = dht22.readHumidity();
  tempC = dht22.readTemperature();
  lux = lightMeter.readLightLevel();

  // Датчик 1
  digitalWrite(PIN_TRIG1, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG1, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG1, LOW);
  duration1 = pulseIn(PIN_ECHO1, HIGH, 30000);
  cm1 = (duration1 == 0) ? -1 : (duration1 / 2) / 29.1;

  // Датчик 2
  digitalWrite(PIN_TRIG2, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG2, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG2, LOW);
  duration2 = pulseIn(PIN_ECHO2, HIGH, 30000);
  cm2 = (duration2 == 0) ? -1 : (duration2 / 2) / 29.1;

  // Датчик 3
  digitalWrite(PIN_TRIG3, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG3, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG3, LOW);
  duration3 = pulseIn(PIN_ECHO3, HIGH, 30000);
  cm3 = (duration3 == 0) ? -1 : (duration3 / 2) / 29.1;

  // Датчик 4
  digitalWrite(PIN_TRIG4, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG4, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG4, LOW);
  duration4 = pulseIn(PIN_ECHO4, HIGH, 30000);
  cm4 = (duration4 == 0) ? -1 : (duration4 / 2) / 29.1;

  // Вывод данных
  Serial.print(cm1);
  Serial.print(",");
  Serial.print(cm2);
  Serial.print(",");
  Serial.print(cm3);
  Serial.print(",");
  Serial.print(cm4);
  Serial.print(",");
  Serial.print(humi);
  Serial.print(",");
  Serial.print(tempC);
  Serial.print(",");
  Serial.print(lux);
  Serial.println("");
  delay(1000); // Задержка между циклами
}
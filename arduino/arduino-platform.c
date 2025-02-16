#include <GyverMotor.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN 150  // Минимальное значение для серво
#define SERVOMAX 600  // Максимальное значение для серво

#define MOTOR1_A 2
#define MOTOR1_B 3
#define MOTOR3_A 4
#define MOTOR3_B 5
#define MOTOR2_A 7
#define MOTOR2_B 6
#define MOTOR4_A 9
#define MOTOR4_B 8

#define BT_TX 13
#define BT_RX 12

#define JOY_MAX 128
#define MAX_SPEED 255
float serv1 = 0.0, serv2 = 0.0, serv3 = 0.0; 
boolean light = false;
boolean y = false, u = false, h = false, j = false, b = false, n = false;

GMotor motorFR(DRIVER2WIRE, MOTOR1_A, MOTOR1_B, HIGH);
GMotor motorFL(DRIVER2WIRE, MOTOR2_A, MOTOR2_B, HIGH);
GMotor motorBR(DRIVER2WIRE, MOTOR3_A, MOTOR3_B, HIGH);
GMotor motorBL(DRIVER2WIRE, MOTOR4_A, MOTOR4_B, HIGH);

SoftwareSerial BTSerial(BT_RX, BT_TX);

void setServoAngle(uint8_t channel, int angle) {
    int pulse = map(angle, 0, 180, SERVOMIN, SERVOMAX);
    pwm.setPWM(channel, 0, pulse);
}

void setupMotors() {  
  motorFR.setMinDuty(250);
  motorFL.setMinDuty(250);
  motorBR.setMinDuty(250);
  motorBL.setMinDuty(250);

  motorFR.setMode(AUTO);
  motorFL.setMode(AUTO);
  motorBR.setMode(AUTO);
  motorBL.setMode(AUTO);

  motorFR.setSmoothSpeed(255);
  motorFL.setSmoothSpeed(255);
  motorBR.setSmoothSpeed(255);
  motorBL.setSmoothSpeed(255);
}

void setup() {
  pwm.begin();
  pwm.setPWMFreq(50);
  
  pinMode(10, OUTPUT);
  
  Serial.begin(9600);
  setupMotors();
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    switch (command) {
      case 'w': // Вперёд
        moveMotors(255, 255, 255, 255);
        break;
      case 's': // Назад
        moveMotors(-255, -255, -255, -255);
        break;
      case 'a': // Влево
        moveMotors(255, -255, -255, 255);
        break;
      case 'd': // Вправо
        moveMotors(-255, 255, 255, -255);
        break;
      case 'e': // Поворот вправо
        moveMotors(-255, -255, 255, 255);
        break;
      case 'q': // Поворот влево
        moveMotors(255, 255, -255, -255);
        break;
      case 'l':
        light = !light;
        digitalWrite(10, light ? HIGH : LOW);
        break;
      case 'm': // Остановка
        moveMotors(0, 0, 0, 0);
        stopAllServos();
        break;
      default:
        handleArmControl(command);
        break;
    }
  }

  updateServos();
}

void moveMotors(int fr, int fl, int br, int bl) {
  motorFR.smoothTick(fr);
  motorFL.smoothTick(fl);
  motorBR.smoothTick(br);
  motorBL.smoothTick(bl);
}

void stopAllServos() {
  y = u = h = j = b = n = false;
}

void handleArmControl(char command) {
  switch (command) {
    case 'y':
      y = true;
      break;
    case 'u':
      u = true;
      break;
    case 'h':
      h = true;
      break;
    case 'j':
      j = true;
      break;
    case 'b':
      b = true;
      break;
    case 'n':
      n = true;
      break;
    case 'M': // Команда остановки движений руки
      stopAllServos();
      break;
  }
}

void updateServos() {
  if (y && serv1 < 74) {
    serv1 += 0.5;
    setServoAngle(0, serv1);
    Serial.print("1 (y): ");
    Serial.println(serv1);
  }

  if (u && serv1 > -30) {
    serv1 -= 0.5;
    setServoAngle(0, serv1);
    Serial.print("1 (u): ");
    Serial.println(serv1);
  }

  if (h && serv2 < 150) {
    serv2 += 0.5;
    setServoAngle(1, serv2);
    Serial.print("2 (h): ");
    Serial.println(serv2);
  }

  if (j && serv2 > -30) {
    serv2 -= 0.5;
    setServoAngle(1, serv2);
    Serial.print("2 (j): ");
    Serial.println(serv2);
  }

  if (b && serv3 < 75) {
    serv3 += 0.5;
    setServoAngle(2, serv3);
    Serial.print("3 (b): ");
    Serial.println(serv3);
  }

  if (n && serv3 > -21) {
    serv3 -= 0.5;
    setServoAngle(2, serv3);
    Serial.print("3 (n): ");
    Serial.println(serv3);
  }
}
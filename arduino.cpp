#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

LiquidCrystal_I2C lcd(0x27, 16, 2); 

SoftwareSerial mySoftwareSerial(10, 11);
DFRobotDFPlayerMini myDFPlayer;

int flexSensor1 = A0;
int flexSensor2 = A1;
int flexSensor3 = A2;
int flexSensor4 = A3;

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  
  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    lcd.print("DFPlayer Fail");
    while (true);
  }
  myDFPlayer.volume(25);

  lcd.setCursor(0, 0);
  lcd.print("Health Monitoring");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
  lcd.clear();
}

void loop() {
  checkFlexSensor();
}

void checkFlexSensor() {
  if (analogRead(flexSensor1) > 850) {
    displayMessage("Need Help", 2);
  } else if (analogRead(flexSensor2) > 850) {
    displayMessage("Need Water", 3);
  } else if (analogRead(flexSensor3) > 850) {
    displayMessage("Need Food", 4);
  } else if (analogRead(flexSensor4) > 850) {
    displayMessage("Need Washroom", 5);
  }
}

void displayMessage(String message, int audioFile) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Alert!");
  lcd.setCursor(0, 1);
  lcd.print(message);
  myDFPlayer.play(audioFile);
  delay(5000);
}
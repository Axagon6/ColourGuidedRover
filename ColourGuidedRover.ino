#include <Adafruit_SSD1306.h>
#include <splash.h>

#include <Adafruit_GFX.h>
#include <Adafruit_GrayOLED.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>

#include <SpheroRVR.h>
#include <DriveControl.h>
#include <LEDControl.h>
#include "structs.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

DriveControl dc;
LedControl lc;

#define OBSTACLE_FRONT 13
#define OBSTACLE_RIGHT 8
#define OBSTACLE_LEFT 12
#define OBSTACLE_E_LEFT A1
#define OBSTACLE_E_RIGHT A0
#define PIR_START 2


bool shouldMoveForward = false;
bool isStarted = false;
unsigned long lastRollUpdate = 0;
const unsigned long rollInterval = 250; // in ms


void rotate180() {
  dc.setHeading(180);       // folosim DriveControl, nu rvr
  dc.rollStart(180, 64);     // miscare cu viteza 0 doar pentru aplicarea heading-ului
  delay(1350);                         // mic delay pentru aplicare
}

void rotate180_fast() {
  //dc.setHeading(180);       // folosim DriveControl, nu rvr
  //dc.rollStart(128, 128);     // miscare cu viteza 0 doar pentru aplicarea heading-ului
  //delay(675); 
  dc.setRawMotors(rawMotorModes::forward, 255, rawMotorModes::reverse, 255);
  delay(515);
  rvr.resetYaw();
  dc.resetHeading();
}

void rotate180_ultrafast() {
  dc.setHeading(180);       // folosim DriveControl, nu rvr
  dc.rollStart(180, 192);     // miscare cu viteza 0 doar pentru aplicarea heading-ului
  delay(450);                         // mic delay pentru aplicare
}

void rotate180_extreme() {
  dc.setHeading(180);       // folosim DriveControl, nu rvr
  dc.rollStart(180, 255);     // miscare cu viteza 0 doar pentru aplicarea heading-ului
  delay(337);                         // mic delay pentru aplicare
}

void setup() {
  rvr.configUART(&Serial);
  delay(200);

  pinMode(OBSTACLE_FRONT, INPUT);
  pinMode(OBSTACLE_LEFT, INPUT);
  pinMode(OBSTACLE_RIGHT, INPUT);
  pinMode(OBSTACLE_E_LEFT, INPUT);
  pinMode(OBSTACLE_E_RIGHT, INPUT);
  pinMode(PIR_START, INPUT);

  dc = rvr.getDriveControl();
  lc = rvr.getLedControl();

  rvr.resetYaw(); // setam orientarea aici

  lcd.init();
  lcd.backlight();
  //lcd.setCursor(0, 0);
  //lcd.print("Salut RVR!");
  //lcd.setCursor(0, 1);
  //lcd.print("Debounce PIR");
  //delay(5100);
  lcd.clear();
}

unsigned long frontCooldown = 0;
unsigned long leftCooldown = 0;
unsigned long rightCooldown = 0;
unsigned long e_leftCooldown = 0;
unsigned long e_rightCooldown = 0;
const unsigned long cooldownTime = 1200;

void loop() {
  rvr.poll();
  rvr.enableColorDetection(true);
  rvr.enableColorDetectionNotify(true, 1000, 20, colorCallback);
  rvr.getCurrentDetectedColorReading();

  unsigned long now = millis();

  int obstacle_front = digitalRead(OBSTACLE_FRONT);
  int obstacle_left = digitalRead(OBSTACLE_LEFT);
  int obstacle_right = digitalRead(OBSTACLE_RIGHT);
  int obstacle_e_left = digitalRead(OBSTACLE_E_LEFT);
  int obstacle_e_right = digitalRead(OBSTACLE_E_RIGHT);
  int start = digitalRead(PIR_START);

   lcd.setCursor(14, 1);
   lcd.print(start);
  
  if (!isStarted && start == HIGH) {
    isStarted = true;
    shouldMoveForward = true;
    lcd.setCursor(0, 1);
    lcd.print("START ");
    dc.setHeading(0);
    delay(1500);
  }
  // if(start==HIGH){
  //   lcd.setCursor(0, 1);
  //   lcd.print("PIR           ");
  //   delay(1500);
  // }

  // Mers continuu daca e activat
  if (shouldMoveForward && now - lastRollUpdate > rollInterval) {
    dc.rollStart(0, 16);
    lcd.setCursor(0, 1);
    lcd.print("FORWARD    ");
    lastRollUpdate = now;
  }

  if (obstacle_front == LOW || (obstacle_left == LOW && obstacle_right == LOW)) {
    lcd.setCursor(0, 1);
    lcd.print("OBSTACLE F ");
    shouldMoveForward = false;
    dc.rollStop(0);
    dc.setRawMotors(rawMotorModes::reverse, 64, rawMotorModes::reverse, 64);
    delay(450);
    rotate180_fast();
    //frontCooldown = millis() + cooldownTime;
    rvr.resetYaw();
    dc.resetHeading();
    shouldMoveForward = true;
    dc.rollStart(0, 16);
    lcd.setCursor(0, 1);
    lcd.print("FORWARD    ");
    lastRollUpdate = now;
  }
  else if (obstacle_left == LOW ) {
    lcd.setCursor(0, 1);
    lcd.print("OBSTACLE L ");
    shouldMoveForward = false;
    dc.rollStop(0);
    dc.setRawMotors(rawMotorModes::reverse, 192, rawMotorModes::reverse, 64);
    delay(500);
    dc.rollStop(0);
    //leftCooldown = millis() + cooldownTime;
    //rvr.resetYaw();
    shouldMoveForward = true;
    dc.rollStart(0, 16);
    lcd.setCursor(0, 1);
    lcd.print("FORWARD          ");
    lastRollUpdate = now;
  }
  else if (obstacle_right == LOW ) {
    lcd.setCursor(0, 1);
    lcd.print("OBSTACLE R ");
    shouldMoveForward = false;
    dc.rollStop(0);
    dc.setRawMotors(rawMotorModes::reverse, 64, rawMotorModes::reverse, 192);
    delay(500);
    dc.rollStop(0);
    //leftCooldown = millis() + cooldownTime;
    //rvr.resetYaw();
    shouldMoveForward = true;
    dc.rollStart(0, 16);
    lcd.setCursor(0, 1);
    lcd.print("FORWARD    ");
    lastRollUpdate = now;
  }
  // else if (obstacle_e_right == LOW) {
  //   lcd.setCursor(0, 1);
  //   lcd.print("OBSTACLE REG   ");
  //   shouldMoveForward = false;
  //   while (digitalRead(OBSTACLE_E_RIGHT) == LOW) {
  //     rotate180Short(90); 
  //   }
  //   e_rightCooldown = millis() + cooldownTime;
  //   rvr.resetYaw();
  //   shouldMoveForward = true;
  // }
  // else if (obstacle_e_left == LOW) {
  //   lcd.setCursor(0, 1);
  //   lcd.print("OBSTACLE LE    ");
  //   shouldMoveForward = false;
  //   while (digitalRead(OBSTACLE_E_LEFT) == LOW) {
  //     rotate180Short(270);
  //   }
  //   e_leftCooldown = millis() + cooldownTime;
  //   rvr.resetYaw();
  //   shouldMoveForward = true;
  // }
  else if (isStarted) {
    lcd.setCursor(0, 1);
    lcd.print("FORWARD    ");
  }
}

void colorCallback(ColorDetectionNotifyReturn_t *detection) {
  uint8_t id = detection->colorClassificationId;
  uint8_t conf = detection->confidence;

  if (detection->isSuccessful) {
    uint8_t r = detection->red;
    uint8_t g = detection->green;
    uint8_t b = detection->blue;

    const uint8_t threshold = 230;
    int sensorValue = r*100000+g*1000+b;

    lcd.setCursor(0, 0);
    
    if (r >= 228 && g <= 53 && b <= 64) {
        lcd.print("RED");
        lcd.setCursor(9, 0);  // Position after "RED" + 4 spaces
        lcd.print(sensorValue);
        dc.rollStop(0);
    } 
    else if (r >= 251 && g <= 171 && b <= 25) {
        lcd.print("ORANGE");
        lcd.setCursor(9, 0);  // Position after "ORANGE" + 4 spaces
        lcd.print(sensorValue);
    }
    else if (r >= 252 && g >= 238 && b <= 33) {
        lcd.print("YELLOW");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r <= 116 && g >= 192 && b <= 68) {
        lcd.print("GREEN");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r <= 10 && g >= 161 && b >= 130) {
        lcd.print("TEAL");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r <= 10 && g <= 123 && b >= 194) {
        lcd.print("BLUE");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r >= 126 && g <= 79 && b >= 154) {
        lcd.print("PURPLE");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r >= 202 && g <= 87 && b >= 160) {
        lcd.print("PINK");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r >= 246 && g >= 146 && b <= 120) {
        lcd.print("CORAL");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r >= 254 && g >= 198 && b <= 121) {
        lcd.print("YELLOW_O");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r <= 177 && g >= 216 && b <= 156) {
        lcd.print("LT GREEN");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r <= 135 && g <= 158 && b >= 207) {
        lcd.print("CLD BLUE");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r >= 230 && g >= 230 && b >= 230) {
        lcd.print("WHITE");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else if (r <= 30 && g <= 30 && b <= 30) {
        lcd.print("BLACK");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
    else {
        lcd.print("UNKNOWN");
        lcd.setCursor(9, 0);
        lcd.print(sensorValue);
    }
  
  } 
 }

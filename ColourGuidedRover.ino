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
#define PIR_START 4

bool stopFlag = false;

struct RGBColor {
  uint8_t r, g, b;
};

static uint32_t ledGroup;

RGBColor getColorFromId(uint8_t id) {
  switch (id) {
    case 1: return {255, 0, 0};     // Red
    case 2: return {0, 255, 0};     // Green
    case 3: return {0, 0, 255};     // Blue
    case 4: return {255, 255, 0};   // Yellow
    case 5: return {0, 255, 255};   // Cyan
    case 6: return {255, 0, 255};   // Magenta
    case 7: return {255, 255, 255}; // White
    case 8: return {255, 165, 0};   // Orange
    case 9: return {255, 192, 203}; // Pink
    default: return {0, 0, 0};      // Black or unknown
  }
}

void moveForward() {
  dc.rollStart(0, 64); // 0° = inainte
}

void moveBackward() {
  dc.rollStart(180, 64); // 180° = inapoi
}

void turnRight() {
  dc.rollStart(90, 64); // 90° = dreapta
}

void turnLeft() {
  dc.rollStart(270, 64); // 270° = stanga
}

void stopMotion() {
  dc.rollStop(0); // opreste complet miscarea
}

void rotate180() {
  dc.setHeading(180);
  dc.rollStart(180, 0);
  delay(1000);
  dc.rollStop(180);
  rvr.resetYaw(); // important!
}

void rotateLeft90() {
  dc.setHeading(270);           // 270° corespunde unei rotiri la stanga
  dc.rollStart(270, 0);         // viteza 0 -> doar setarea headingului
  delay(1000);                  // asteptam sa aplice heading-ul
  dc.rollStop(270);            
  rvr.resetYaw();               // reseteaza heading-ul la 0
}

void rotateRight90() {
  dc.setHeading(90);            // 90° corespunde unei rotiri la dreapta
  dc.rollStart(90, 0);          // viteza 0 -> doar setarea headingului
  delay(1000);                  
  dc.rollStop(90);
  rvr.resetYaw();               // reseteaza heading-ul la 0
}

void setup() {
  rvr.configUART(&Serial);
  delay(2000);

  pinMode(OBSTACLE_FRONT, INPUT);
  pinMode(OBSTACLE_LEFT, INPUT);
  pinMode(OBSTACLE_RIGHT, INPUT);
  pinMode(PIR_START, INPUT);

  dc = rvr.getDriveControl();
  lc = rvr.getLedControl();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Salut RVR!");
  lcd.setCursor(0, 1);
  lcd.print("Test I2C OK!");
  delay(2000);
  lcd.clear();

  ledGroup = 0;
}

void loop() {
  rvr.poll();
  rvr.enableColorDetection(true);
  rvr.enableColorDetectionNotify(true, 1000, 20, colorCallback);
  rvr.getCurrentDetectedColorReading();

  int obstacle_front = digitalRead(OBSTACLE_FRONT);
  int obstacle_left = digitalRead(OBSTACLE_LEFT);
  int obstacle_right = digitalRead(OBSTACLE_RIGHT);
  int start = digitalRead(PIR_START);
  
  if(start == HIGH){
    lcd.setCursor(0, 0);
    lcd.print("START          ");
    dc.setHeading(0);
    dc.rollStart(0, 16);
    lcd.print("               ");
  }

  if (obstacle_front == LOW) {
    lcd.setCursor(0, 1);
    lcd.print("OBSTACLE F     ");
    rotate180();
    delay(1000);
    dc.rollStart(0, 16);

  }
  if (obstacle_left == LOW) {
    lcd.setCursor(0, 1);
    lcd.print("OBSTACLE L     ");
    rotateLeft90();
    delay(1000);
    dc.rollStart(0, 16);
  }
  if (obstacle_right == LOW) {
    lcd.setCursor(0, 1);
    lcd.print("OBSTACLE R     ");
    rotateRight90();
    delay(1000);
    dc.rollStart(0, 16);
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("Liber          ");
  }
}

void colorCallback(ColorDetectionNotifyReturn_t *detection) {
  uint8_t id = detection->colorClassificationId;
  uint8_t conf = detection->confidence;

  if (detection->isSuccessful) {
    uint8_t r = detection->red;
    uint8_t g = detection->green;
    uint8_t b = detection->blue;

    const uint8_t threshold = 200;

    lcd.setCursor(0, 0);
    if (r > threshold) {
      lcd.print("ROSU           ");
      stopMotion();
    } else if (g > threshold) {
      lcd.print("VERDE          ");
    } else if (b > threshold) {
      lcd.print("ALBASTRU       ");
    } else {
      lcd.print("Necunoscut     ");
    }
  }
}

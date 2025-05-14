#include <SpheroRVR.h>
#include <DriveControl.h>
#include <LEDControl.h>
#include "structs.h"
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

DriveControl dc;
LedControl lc;

#define OBSTACLE 13

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

void setup() {
  rvr.configUART(&Serial);
  delay(2000);

  pinMode(OBSTACLE, INPUT);

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

  int obstacle = digitalRead(OBSTACLE);
  lcd.setCursor(0, 1);
  if (obstacle == LOW) {
    lcd.print("OBSTACOL       ");
    rotate180();
  } else {
    lcd.print("Liber          ");
    moveForward();
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
      moveForward();
    } else if (b > threshold) {
      lcd.print("ALBASTRU       ");
    } else {
      lcd.print("Necunoscut     ");
    }
  }
}

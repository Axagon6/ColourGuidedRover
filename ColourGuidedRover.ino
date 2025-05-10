#include <SpheroRVR.h>
#include <DriveControl.h>
#define PIRPIN 2

#define ALL_LEDS 0x3FFFFFFF
const uint8_t RED[] = {255, 0, 0};
const uint8_t ORANGE[] = {255, 100, 0};
const uint8_t YELLOW[] = {255, 255, 0};
const uint8_t GREEN[] = {0, 255, 0};
const uint8_t BLUE[] = {0, 0, 255};
const uint8_t PURPLE[] = {100, 0, 255};
const uint8_t WHITE[] = {255, 255, 255};
const uint8_t OFF[] = {0, 0, 0};
// this is the index of the current color
byte colorIndex = 0;
DriveControl dc;
LedControl lc;
// this is the RGB array for the current color
uint8_t currentColor[3] = {0};
static uint32_t ledGroup;


void setup() {
  // configure the Arduino pin to read from the PIR sensor
  pinMode(PIRPIN, INPUT);
  // put your setup code here, to run once:
  rvr.configUART(&Serial);
 // setting up the led group for the right headlight
    ledGroup = 0;
    ledGroup |= (1 << static_cast<uint8_t>(LEDs::rightHeadlightRed));
    ledGroup |= (1 << static_cast<uint8_t>(LEDs::rightHeadlightGreen));
    ledGroup |= (1 << static_cast<uint8_t>(LEDs::rightHeadlightBlue));
 
  delay(2000);
}

void loop() {
  rvr.poll();
  rvr.enableColorDetection(true);
  rvr.enableColorDetectionNotify(
    true,     // Enable notifications
    1000,     // Check every 1 second
    20,       // 50% minimum confidence
    [](ColorDetectionNotifyReturn_t *detection) {
    // Lambda function as the callback
      if (detection->isSuccessful) {
      // set right headlight to red and wait 1 second
      uint8_t redArray[] = {0xFF, 0x00, 0x00};
      uint8_t blueArray[] = {0x00, 0x00, 0xFF};
      uint8_t greenArray[] = {0x00, 0xFF, 0x00};
      if (detection->blue > 200 ){
      rvr.setAllLeds(ledGroup, blueArray, sizeof(blueArray) / sizeof(blueArray[0]));
      delay(1000);}
      else if (detection->red > 200){
        rvr.setAllLeds(ledGroup, redArray, sizeof(redArray) / sizeof(redArray[0]));
      delay(1000);
      } else {
        rvr.setAllLeds(ledGroup, greenArray, sizeof(greenArray) / sizeof(greenArray[0]));
        delay(1000);
      }
    } else {
      }});

      rvr.getCurrentDetectedColorReading();
  // put your main code here, to run repeatedly:
   
   //sp.enableColorDetectionNotify(bool isEnabled, uint16_t interval, uint8_t minimumConfidenceThreshold, ColorDetectionNotifyCallback_t callback)
   if (digitalRead(PIRPIN) == HIGH){
    //dc.aimStart();
    //dc.rollStart(10, 10);
   }
   if (digitalRead(PIRPIN) == LOW){
    //dc.aimStop();
    //delay(100);
   }

  

  // tell RVR to update the LED colors
  //rvr.setAllLeds(ALL_LEDS, ledValues, 30);

  // loop every 1 second
  delay(100);

}
 




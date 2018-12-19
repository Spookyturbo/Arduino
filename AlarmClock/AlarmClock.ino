#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);

  //setting up the clock
  if(!rtc.begin()) {
    Serial.println("Clock could not properly initialise");
  }
  
  if(rtc.lostpower()) {
    Serial.println("Clock lost power, defaulting to compile time.");
    //Set time to compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  DateTime currentTime = rtc.now();
}

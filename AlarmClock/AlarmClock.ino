#include <avr/pgmspace.h>
#include "ShiftRegIn.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
RTC_DS3231 rtc;

struct Time {
  unsigned int hour: 5;
  unsigned int minute: 6;
};

//Store weekdays in flash memory
const char weekDays[7][4] PROGMEM =
{
  { "Sun" },
  { "Mon" },
  { "Tue" },
  { "Wed" },
  { "Thu" },
  { "Fri" },
  { "Sat" }
};

//Store months in flash memory
const char months[12][4] PROGMEM =
{
  { "Jan" },
  { "Feb" },
  { "Mar" },
  { "Apr" },
  { "May" },
  { "Jun" },
  { "Jul" },
  { "Aug" },
  { "Sep" },
  { "Oct" },
  { "Nov" },
  { "Dec" }
};

//load, clock, data
ShiftRegIn buttons(3, 4, 5);

//Button numbers
const uint8_t alarmSwitch = 0;
const uint8_t sleepButton = 1;

//Sleep Variables
const uint8_t sleepLightPin = 13;
//120 seconds
const uint8_t sleepLength = 120;
bool sleepEnabled = false;

//Alarm Variables
const uint8_t alarmBuzzPin = 2;
const uint8_t alarmFrequency = 4500;
const uint8_t alarmLength = 30;
bool alarmBuzzing = false;
struct Time alarmTime;

uint8_t buttonStates = 0;

void setup() {
  Serial.begin(9600);

  //Initialise alarm
  alarmTime.hour = 6;
  alarmTime.minute = 20;
  pinMode(alarmBuzzPin, OUTPUT);

  //Setup sleep button
  pinMode(sleepLightPin, OUTPUT);
  digitalWrite(sleepLightPin, LOW);

  //setting up the display
  lcd.begin(16, 2);
  //setting up the clock
  if (!rtc.begin()) {
    Serial.println(F("Clock could not properly initialise"));
  }

  if (rtc.lostPower()) {
    Serial.println(F("Clock lost power, defaulting to compile time."));
    //Set time to compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  printDate();
  printTime();
  buttons.updateButtonState();
  checkAlarm();

  //Managing the sleep states
  bool sleepButtonPressed = buttons.getButtonDown(sleepButton);
  if (sleepButtonPressed && !sleepEnabled) {
    enableSleep();
  }
  else if (sleepButtonPressed) {
    disableSleep();
  }
}

void checkAlarm() {
  static unsigned long alarmStartTime = 0;
  static unsigned long sleepStartTime = 0;
  DateTime currentTime = rtc.now();

  bool isAlarmEnabled = buttons.getButton(alarmSwitch);

  if (alarmBuzzing && isAlarmEnabled) {
    if (sleepEnabled) {
      stopAlarm();
      //Exit sleep mode
      if (currentTime.unixtime() - sleepStartTime >= sleepLength) {
        disableSleep();
      }
      //Will stop updating when alarm starts back up because sleep disables
      alarmStartTime = currentTime.unixtime();
    }
    else {
      soundAlarm();
      //Exit alarm
      if (currentTime.unixtime() - alarmStartTime >= alarmLength) {
        alarmBuzzing = false;
        stopAlarm();
      }
      //Sleep start time will stop updating when sleep is enabled
      sleepStartTime = currentTime.unixtime();
    }
  }
  else { //Happens when the alarm finishes or when the alarm is turned off
    alarmBuzzing = false;
    disableSleep();
    stopAlarm();
  }

  if (currentTime.hour() == alarmTime.hour && currentTime.minute() == alarmTime.minute && currentTime.second() == 0 && !sleepEnabled && isAlarmEnabled) {
    soundAlarm();
    alarmBuzzing = true;
    alarmStartTime = currentTime.unixtime();
  }
}

//turns on the light and only works while alarm is buzzing
void enableSleep() {
  if (alarmBuzzing) {
    sleepEnabled = true;
    digitalWrite(sleepLightPin, HIGH);
  }
}

//Turns off the light and sets value
void disableSleep() {
  sleepEnabled = false;
  digitalWrite(sleepLightPin, LOW);
}

void soundAlarm() {
  tone(alarmBuzzPin, alarmFrequency);
}

//Stops the alarm buzzing
void stopAlarm() {
  noTone(alarmBuzzPin);
}

/*
   Prints the date on the first line of the lcd
   in the format: WWW, MMM DD
   Example: Tue, Oct 27
*/
void printDate() {
  DateTime currentTime = rtc.now();

  //Retrieve month from flash memory
  char month[4];
  memcpy_P(&month, &months[currentTime.month() - 1], 4);
  uint8_t day = currentTime.day();

  //Retrieve weekDay from flash memory
  char weekDay[4];
  memcpy_P(&weekDay, &weekDays[currentTime.dayOfTheWeek()], 4);

  lcd.setCursor(0, 0);
  lcd.print(weekDay);
  lcd.print(", ");
  lcd.print(month);
  lcd.print(" ");
  lcd.print(day);

}

/*
   Prints out the time in format HH:MM:SS P/A
   On the second line of the LCD
*/
void printTime() {
  DateTime currentTime = rtc.now();

  uint8_t hour = currentTime.hour();
  uint8_t minute = currentTime.minute();
  uint8_t second = currentTime.second();

  bool AM = true;
  if (hour >= 12) {
    AM = false;
    if (hour >= 13)
      hour -= 12;
  }
  else if (hour == 0) {
    hour = 12;
  }

  lcd.setCursor(0, 1);

  if (hour < 10)
    lcd.print(0);
  lcd.print(hour);
  lcd.print(F(":"));

  if (minute < 10)
    lcd.print(0);
  lcd.print(minute);
  lcd.print(F(":"));

  if (second < 10)
    lcd.print(0);
  lcd.print(second);
  lcd.print((AM) ? F(" A") : F(" P"));
}

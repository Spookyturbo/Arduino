#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>
#include <Piezo.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
RTC_DS3231 rtc;

struct Time {
  unsigned int hour: 5;
  unsigned int minute: 6;
};

int marioMelody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7,
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0,
 
  NOTE_C7, 0, 0, NOTE_G6,
  0, 0, NOTE_E6, 0,
  0, NOTE_A6, 0, NOTE_B6,
  0, NOTE_AS6, NOTE_A6, 0,
 
  NOTE_G6, NOTE_E7, NOTE_G7,
  NOTE_A7, 0, NOTE_F7, NOTE_G7,
  0, NOTE_E7, 0, NOTE_C7,
  NOTE_D7, NOTE_B6, 0, 0
};

int marioTempo[] = {
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
 
  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
};

int marioLength = sizeof(marioMelody) / sizeof(marioMelody[0]);
const float marioMultiplier = 0.5f;

int  marioUnderworldMelody[] = {
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
  NOTE_AS3, NOTE_AS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0,
  NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
  NOTE_DS3, NOTE_DS4, 0,
  0, NOTE_DS4, NOTE_CS4, NOTE_D4,
  NOTE_CS4, NOTE_DS4,
  NOTE_DS4, NOTE_GS3,
  NOTE_G3, NOTE_CS4,
  NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
  NOTE_GS4, NOTE_DS4, NOTE_B3,
  NOTE_AS3, NOTE_A3, NOTE_GS3,
  0, 0, 0
};

int marioUnderworldTempo[] = {
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  3,
  12, 12, 12, 12,
  12, 12, 6,
  6, 18, 18, 18,
  6, 6,
  6, 6,
  6, 6,
  18, 18, 18, 18, 18, 18,
  10, 10, 10,
  10, 10, 10,
  3, 3, 3
};

int marioUnderworldLength = sizeof(marioUnderworldMelody) / sizeof(marioUnderworldMelody[0]);
float marioUnderworldMultiplier = 0.5f;

int  zeldaThemeMelody[] = {
  NOTE_AS4, 0, 0, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, 0, NOTE_GS4, NOTE_AS4, 0, 0, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, 0, NOTE_GS4, NOTE_AS4, 0, 0, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_AS4, NOTE_F3, NOTE_F3, 0, NOTE_AS4, NOTE_AS4, NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_F5, 0, NOTE_F5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_AS5, 0, NOTE_AS5, NOTE_AS5, NOTE_AS5, NOTE_GS5, NOTE_FS5, NOTE_GS5, 0, NOTE_FS5, NOTE_F5, NOTE_F5, NOTE_DS5, NOTE_DS5, NOTE_F5, NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_CS5, NOTE_DS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_G5, NOTE_F5, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_AS4, NOTE_F3, NOTE_F3, 0, NOTE_AS4, NOTE_AS4, NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_F5, 0, NOTE_F5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_AS5, 0, NOTE_CS6, NOTE_C6, NOTE_A5, 0, NOTE_F5, NOTE_FS5, 0, NOTE_AS5, NOTE_A5, NOTE_F5, 0, NOTE_F5, NOTE_FS5, 0, NOTE_AS5, NOTE_A5, NOTE_F5, 0, NOTE_D5, NOTE_DS5, 0, NOTE_FS5, NOTE_F5, NOTE_CS5, 0, NOTE_AS4, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, 0, NOTE_G5, NOTE_F5, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_AS4, NOTE_F3, NOTE_F3, 0, NOTE_AS4, NOTE_AS4, NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_F5, 0, NOTE_F5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_AS5, 0, NOTE_AS5, NOTE_AS5, NOTE_AS5, NOTE_GS5, NOTE_FS5, NOTE_GS5, 0, NOTE_FS5, NOTE_F5, NOTE_F5, NOTE_DS5, NOTE_DS5, NOTE_F5, NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_CS5, NOTE_DS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_G5, NOTE_F5, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_AS4, NOTE_F3, NOTE_F3, 0, NOTE_AS4, NOTE_AS4, NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_F5, 0, NOTE_F5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_AS5, 0, NOTE_CS6, NOTE_C6, NOTE_A5, 0, NOTE_F5, NOTE_FS5, 0, NOTE_AS5, NOTE_A5, NOTE_F5, 0, NOTE_F5, NOTE_FS5, 0, NOTE_AS5, NOTE_A5, NOTE_F5, 0, NOTE_D5, NOTE_DS5, 0, NOTE_FS5, NOTE_F5, NOTE_CS5, 0, NOTE_AS4, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, 0, NOTE_G5, NOTE_F5, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3
};

int zeldaThemeTempo[] = {
  2, 8, 8, 8, 8, 8, 8, 6, 16, 16, 4, 8, 8, 8, 8, 8, 8, 6, 16, 16, 4, 8, 8, 8, 8, 8, 8, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8 ,8 ,8, 2, 8, 8, 8 ,8 ,8, 8, 6, 16, 16, 2, 4, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8 ,8 ,8, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 8, 16, 16, 4, 4, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8 ,8 ,8, 2, 8, 8, 8 ,8 ,8, 8, 6, 16, 16, 2, 4, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8 ,8 ,8, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 8, 16, 16, 4, 4, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8
};

int zeldaThemeLength = sizeof(songOfStormsMelody) / sizeof(songOfStormsMelody[0]);
float zeldaThemeMultiplier = 0.75f;

//Storing weekday info in the flash memory
const char weekDay_0[] PROGMEM = "Sun";
const char weekDay_1[] PROGMEM = "Mon";
const char weekDay_2[] PROGMEM = "Tue";
const char weekDay_3[] PROGMEM = "Wed";
const char weekDay_4[] PROGMEM = "Thu";
const char weekDay_5[] PROGMEM = "Fri";
const char weekDay_6[] PROGMEM = "Sat";

const char * const weekDays[] PROGMEM =
{
  weekDay_0,
  weekDay_1,
  weekDay_2,
  weekDay_3,
  weekDay_4,
  weekDay_5,
  weekDay_6
};

//Storing month info in the flash memory
const char month_0[] PROGMEM = "Jan";
const char month_1[] PROGMEM = "Feb";
const char month_2[] PROGMEM = "Mar";
const char month_3[] PROGMEM = "Apr";
const char month_4[] PROGMEM = "May";
const char month_5[] PROGMEM = "Jun";
const char month_6[] PROGMEM = "Jul";
const char month_7[] PROGMEM = "Aug";
const char month_8[] PROGMEM = "Sep";
const char month_9[] PROGMEM = "Oct";
const char month_10[] PROGMEM = "Nov";
const char month_11[] PROGMEM = "Dec";

const char * const months[] PROGMEM =
{
  month_0,
  month_1,
  month_2,
  month_3,
  month_4,
  month_5,
  month_6,
  month_7,
  month_8,
  month_9,
  month_10,
  month_11
};

const uint8_t sleepButtonPin = 3;
const uint8_t sleepLightPin = 13;
const uint8_t alarmOnPin = 4;
const uint8_t alarmBuzzPin = 2;

//120 seconds
const uint8_t sleepLength = 120;
bool sleepEnabled = false;
bool previousSleepButtonState = false;

const uint8_t alarmFrequency = 4500;
const uint8_t alarmLength = 30;
bool alarmBuzzing = false;
struct Time alarmTime;

Piezo buzzer(alarmBuzzPin);

void setup() {
  Serial.begin(9600);
  
  //Initialise alarm
  alarmTime.hour = 6;
  alarmTime.minute = 20;
  pinMode(alarmBuzzPin, OUTPUT);
  pinMode(alarmOnPin, INPUT_PULLUP);

  //Setup sleep button
  pinMode(sleepButtonPin, INPUT_PULLUP);
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
  //checkAlarm();

  //Managing the sleep states
  bool sleepButtonPressed = getSleepDown();
  if (sleepButtonPressed && !sleepEnabled) {
    enableSleep();
  }
  else if (sleepButtonPressed) {
    disableSleep();
  }

  buzzer.playSong(zeldaThemeMelody, zeldaThemeTempo, zeldaThemeLength, zeldaThemeMultiplier);
}

void checkAlarm() {
  static unsigned long alarmStartTime = 0;
  static unsigned long sleepStartTime = 0;
  DateTime currentTime = rtc.now();

  if (alarmBuzzing && isAlarmEnabled()) {
    if (sleepEnabled) {
      stopAlarm();
      //Light up the sleep button
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
      //Unlight the sleep button
    }
  }
  else { //Happens when the alarm finishes or when the alarm is turned off
    alarmBuzzing = false;
    disableSleep();
    stopAlarm();
  }

  if (currentTime.hour() == alarmTime.hour && currentTime.minute() == alarmTime.minute && currentTime.second() == 0 && !sleepEnabled && isAlarmEnabled()) {
    soundAlarm();
    alarmBuzzing = true;
    alarmStartTime = currentTime.unixtime();
  }
}

void soundAlarm() {
  tone(alarmBuzzPin, alarmFrequency);
}

void musicAlarm() {

}

//Stops the alarm buzzing
void stopAlarm() {
  noTone(alarmBuzzPin);
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

//Returns the status of the alarm switch
bool isAlarmEnabled() {
  return !digitalRead(alarmOnPin);
}

/*
   Returns true when button is first pressed
   Button must be let go once to be pressed again
*/
bool getSleepDown() {
  static bool previousState;
  bool justPressed = false;
  bool buttonPressed = !digitalRead(sleepButtonPin);

  if (!previousState && buttonPressed) {
    justPressed = true;
  }
  previousState = buttonPressed;
  return justPressed;
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
  strcpy_P(month, (char*)pgm_read_word(&(months[currentTime.month() - 1])));

  uint8_t day = currentTime.day();

  //Retrieve weekDay from flash memory
  char weekDay[4];
  strcpy_P(weekDay, (char*)pgm_read_word(&(weekDays[currentTime.dayOfTheWeek()])));

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

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

struct Song {
  uint16_t *melody;
  uint16_t *tempo;
  uint16_t length;
  float speedMultiplier;
};

const uint16_t marioThemeMelody[] PROGMEM = {NOTE_E7, NOTE_E7, 0, NOTE_E7, 0, NOTE_C7, NOTE_E7, 0, NOTE_G7, 0, 0,  0, NOTE_G6, 0, 0, 0, NOTE_C7, 0, 0, NOTE_G6, 0, 0, NOTE_E6, 0, 0, NOTE_A6, 0, NOTE_B6, 0, NOTE_AS6, NOTE_A6, 0, NOTE_G6, NOTE_E7, NOTE_G7, NOTE_A7, 0, NOTE_F7, NOTE_G7, 0, NOTE_E7, 0, NOTE_C7, NOTE_D7, NOTE_B6, 0, 0, NOTE_C7, 0, 0, NOTE_G6, 0, 0, NOTE_E6, 0, 0, NOTE_A6, 0, NOTE_B6, 0, NOTE_AS6, NOTE_A6, 0, NOTE_G6, NOTE_E7, NOTE_G7, NOTE_A7, 0, NOTE_F7, NOTE_G7, 0, NOTE_E7, 0, NOTE_C7, NOTE_D7, NOTE_B6, 0, 0};
const uint16_t marioThemeTempo[] PROGMEM = {12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 9, 9, 9, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 9, 9, 9, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12};

const uint16_t marioUnderworldMelody[] PROGMEM = {NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4, NOTE_AS3, NOTE_AS4, 0, 0, NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4, NOTE_AS3, NOTE_AS4, 0, 0, NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4, NOTE_DS3, NOTE_DS4, 0, 0, NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4, NOTE_DS3, NOTE_DS4, 0, 0, NOTE_DS4, NOTE_CS4, NOTE_D4, NOTE_CS4, NOTE_DS4, NOTE_DS4, NOTE_GS3, NOTE_G3, NOTE_CS4, NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4, NOTE_GS4, NOTE_DS4, NOTE_B3, NOTE_AS3, NOTE_A3, NOTE_GS3, 0, 0, 0};
const uint16_t marioUnderworldTempo[] PROGMEM = {12, 12, 12, 12, 12, 12, 6, 3, 12, 12, 12, 12, 12, 12, 6, 3, 12, 12, 12, 12, 12, 12, 6, 3, 12, 12, 12, 12, 12, 12, 6, 6, 18, 18, 18, 6, 6, 6, 6, 6, 6, 18, 18, 18, 18, 18, 18, 10, 10, 10, 10, 10, 10, 3, 3, 3};

const uint16_t zeldaThemeMelody[] PROGMEM = {NOTE_AS4, 0, 0, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, 0, NOTE_GS4, NOTE_AS4, 0, 0, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, 0, NOTE_GS4, NOTE_AS4, 0, 0, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_AS4, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_AS4, NOTE_F3, NOTE_F3, 0, NOTE_AS4, NOTE_AS4, NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_F5, 0, NOTE_F5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_AS5, 0, NOTE_AS5, NOTE_AS5, NOTE_AS5, NOTE_GS5, NOTE_FS5, NOTE_GS5, 0, NOTE_FS5, NOTE_F5, NOTE_F5, NOTE_DS5, NOTE_DS5, NOTE_F5, NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_CS5, NOTE_DS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_G5, NOTE_F5, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_AS4, NOTE_F3, NOTE_F3, 0, NOTE_AS4, NOTE_AS4, NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_F5, 0, NOTE_F5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_AS5, 0, NOTE_CS6, NOTE_C6, NOTE_A5, 0, NOTE_F5, NOTE_FS5, 0, NOTE_AS5, NOTE_A5, NOTE_F5, 0, NOTE_F5, NOTE_FS5, 0, NOTE_AS5, NOTE_A5, NOTE_F5, 0, NOTE_D5, NOTE_DS5, 0, NOTE_FS5, NOTE_F5, NOTE_CS5, 0, NOTE_AS4, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, 0, NOTE_G5, NOTE_F5, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_AS4, NOTE_F3, NOTE_F3, 0, NOTE_AS4, NOTE_AS4, NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_F5, 0, NOTE_F5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_AS5, 0, NOTE_AS5, NOTE_AS5, NOTE_AS5, NOTE_GS5, NOTE_FS5, NOTE_GS5, 0, NOTE_FS5, NOTE_F5, NOTE_F5, NOTE_DS5, NOTE_DS5, NOTE_F5, NOTE_FS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_CS5, NOTE_DS5, NOTE_F5, NOTE_DS5, NOTE_CS5, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_G5, NOTE_F5, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_AS4, NOTE_F3, NOTE_F3, 0, NOTE_AS4, NOTE_AS4, NOTE_C5, NOTE_D5, NOTE_DS5, NOTE_F5, 0, NOTE_F5, NOTE_F5, NOTE_FS5, NOTE_GS5, NOTE_AS5, 0, NOTE_CS6, NOTE_C6, NOTE_A5, 0, NOTE_F5, NOTE_FS5, 0, NOTE_AS5, NOTE_A5, NOTE_F5, 0, NOTE_F5, NOTE_FS5, 0, NOTE_AS5, NOTE_A5, NOTE_F5, 0, NOTE_D5, NOTE_DS5, 0, NOTE_FS5, NOTE_F5, NOTE_CS5, 0, NOTE_AS4, NOTE_C5, NOTE_C5, NOTE_D5, NOTE_E5, 0, NOTE_G5, NOTE_F5, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3, NOTE_F3};
const uint16_t zeldaThemeTempo[] PROGMEM = {2, 8, 8, 8, 8, 8, 8, 6, 16, 16, 4, 8, 8, 8, 8, 8, 8, 6, 16, 16, 4, 8, 8, 8, 8, 8, 8, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8 , 8 , 8, 2, 8, 8, 8 , 8 , 8, 8, 6, 16, 16, 2, 4, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8 , 8 , 8, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 8, 16, 16, 4, 4, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8 , 8 , 8, 2, 8, 8, 8 , 8 , 8, 8, 6, 16, 16, 2, 4, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 8, 8, 8, 16, 16, 2, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8, 4, 4, 6, 16, 16, 16, 16, 16, 16, 2, 8, 8, 8 , 8 , 8, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 8, 16, 16, 4, 4, 4, 8, 16, 16, 8, 16, 16, 8, 16, 16, 8, 8};

const Song alarmSongs[] PROGMEM =
{
  { //Mario Theme
    marioThemeMelody,
    marioThemeTempo,
    78,
    0.5f
  },
  { //Mario Underworld
    marioUnderworldMelody,
    marioUnderworldTempo,
    56,
    0.5f
  },
  { //Zelda Theme
    zeldaThemeMelody,
    zeldaThemeTempo,
    268,
    0.75f
  }
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

//Input variables
const uint8_t loadPin = 3;
const uint8_t clkPin = 4;
const uint8_t dataPin = 5;

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
Time alarmTime;

Piezo buzzer(alarmBuzzPin);

uint8_t buttonStates = 0;

void setup() {
  Serial.begin(9600);

  //Initialise Shift register
  pinMode(loadPin, OUTPUT);
  pinMode(clkPin, OUTPUT);
  pinMode(dataPin, INPUT);
  digitalWrite(loadPin, HIGH);
  digitalWrite(clkPin, HIGH);

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
  readInput();
  //checkAlarm();

  playSongAtIndex(2);

  //Managing the sleep states
  bool sleepButtonPressed = getSleepDown();
  if (sleepButtonPressed && !sleepEnabled) {
    enableSleep();
  }
  else if (sleepButtonPressed) {
    disableSleep();
  }
}

void playSongAtIndex(int index) {
  Song song;
  memcpy_P(&song, &alarmSongs[index], sizeof(song));

  uint16_t melody[song.length];
  //Multiply song length by 2 because melody elements are each 2 bytes
  memcpy_P(melody, song.melody, song.length * 2);

  uint16_t tempo[song.length];
  memcpy_P(tempo, song.tempo, song.length * 2);
  
  buzzer.playSong(melody, tempo, song.length, song.speedMultiplier);
}

void checkAlarm() {
  static unsigned long alarmStartTime = 0;
  static unsigned long sleepStartTime = 0;
  DateTime currentTime = rtc.now();

  if (alarmBuzzing && isAlarmEnabled()) {
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

  if (currentTime.hour() == alarmTime.hour && currentTime.minute() == alarmTime.minute && currentTime.second() == 0 && !sleepEnabled && isAlarmEnabled()) {
    soundAlarm();
    alarmBuzzing = true;
    alarmStartTime = currentTime.unixtime();
  }
}

void readInput() {
  digitalWrite(clkPin, HIGH);
  digitalWrite(loadPin, LOW);
  delayMicroseconds(5);
  digitalWrite(loadPin, HIGH);

  buttonStates = shiftIn(dataPin, clkPin, MSBFIRST);
}

/*
   Returns true when button is first pressed
   Button must be let go once to be pressed again
*/
bool getSleepDown() {
  static bool previousState;
  bool justPressed = false;
  bool buttonPressed = buttonStates & B00000010; //!digitalRead(sleepButtonPin);

  if (!previousState && buttonPressed) {
    justPressed = true;
  }
  previousState = buttonPressed;
  return justPressed;
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
  return buttonStates & B00000001;
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

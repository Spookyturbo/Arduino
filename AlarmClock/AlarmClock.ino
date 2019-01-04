#include <avr/pgmspace.h>
#include "ShiftRegIn.h"
#include <LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>
#include <Piezo.h>
#include <EEPROM.h>

struct Time {
  unsigned int hour: 5;
  unsigned int minute: 6;
  unsigned int second: 6;
};

struct Song {
  char *name;
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

const uint16_t jingleBellsMelody[] PROGMEM = {NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_G5, NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_F5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_E5, NOTE_D5, NOTE_D5, NOTE_E5, NOTE_D5, NOTE_G5};
const uint16_t jingleBellsTempo[] PROGMEM = {8, 8, 4, 8, 8, 4, 8, 8, 8, 8, 2, 8, 8, 8, 8, 8, 8, 8, 16, 16, 8, 8, 8, 8, 4, 4};

//We wish you a merry christmas
const uint16_t wishMelody[] PROGMEM = {NOTE_B3, NOTE_F4, NOTE_F4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_D4, NOTE_D4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_A4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_B3, NOTE_B3, NOTE_D4, NOTE_G4, NOTE_E4, NOTE_F4};
const uint16_t wishTempo[] PROGMEM = {4, 4, 8, 8, 8, 8, 4, 4, 4, 4, 8, 8, 8, 8, 4, 4, 4, 4, 8, 8, 8, 8, 4, 4, 8, 8, 4, 4, 4, 2};

const uint16_t santaMelody[] PROGMEM = {NOTE_G4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_G4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_F4, NOTE_F4, NOTE_E4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_D4, NOTE_F4, NOTE_B3, NOTE_C4};
const uint16_t santaTempo[] PROGMEM = {8, 8, 8, 4, 4, 4, 8, 8, 4, 4, 4, 8, 8, 4, 4, 4, 8, 8, 4, 2, 4, 4, 4, 4, 4, 2, 4, 1};

const Song alarmSongs[] PROGMEM =
{
  { //Mario Theme
    "Mario Theme",
    marioThemeMelody,
    marioThemeTempo,
    78,
    0.5f
  },
  { //Mario Underworld
    "Mario Underworld",
    marioUnderworldMelody,
    marioUnderworldTempo,
    56,
    0.5f
  },
  { //Zelda Theme
    "Zelda Theme",
    zeldaThemeMelody,
    zeldaThemeTempo,
    268,
    0.75f
  },
  {
    "Jingle Bells",
    jingleBellsMelody,
    jingleBellsTempo,
    26,
    0.5f
  },
  {
    "Wish Christmas",
    wishMelody,
    wishTempo,
    30,
    1.0f
  },
  {
    "Santa Town",
    santaMelody,
    santaTempo,
    28,
    1.0f
  }
};
const uint8_t songListLength = sizeof(alarmSongs) / sizeof(alarmSongs[0]);

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

//Set up the display
LiquidCrystal lcd(7, 8, A3, A2, A1, 12);
//Set up the backLight
int brightness = 255;
const uint8_t redLightPin = 9;
const uint8_t greenLightPin = 10;
const uint8_t blueLightPin = 11;

RTC_DS3231 rtc;

//load, clock, data
ShiftRegIn buttons(3, 4, 5);

//Button numbers
const uint8_t alarmSwitch = 0;
const uint8_t sleepButton = 1;
const uint8_t modeSwitchButton = 2;
const uint8_t backButton = 3;
const uint8_t downButton = 4;
const uint8_t upButton = 5;

//LCD Modes
const uint8_t displayMode = 0;
const uint8_t setTimeMode = 1;
const uint8_t setAlarmMode = 2;
const uint8_t setMusicMode = 3;
const uint8_t setRGBMode = 4;

//LCD Control Variables
uint8_t currentMode = 0;
//For resetting currentMode, not resetting the clock
const uint8_t maxMode = 4;
int8_t cursorPosition = 0;

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
//EEPROM address for the alarm storage (Keeping alarm in case of power loss)
const uint8_t eeAlarmAddress = 0;

//0 = Normal Buzzer, 1 = Random, 2-X = specificAlarmSong at index X - 2;
uint8_t usersAlarmChoice = 0;
uint8_t chosenAlarmSong = 2;

Time alarmTime;

Piezo buzzer(alarmBuzzPin);

uint8_t buttonStates = 0;

void setup() {
  Serial.begin(9600);

  //Initalise RGB backlight
  pinMode(redLightPin, OUTPUT);
  pinMode(greenLightPin, OUTPUT);
  pinMode(blueLightPin, OUTPUT);
  setBacklight(255, 0, 255);

  //Initialise alarm
  //Read the alarm time from the saved memory
  EEPROM.get(eeAlarmAddress, alarmTime);
  
  pinMode(alarmBuzzPin, OUTPUT);
  randomSeed(millis());

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
  LCDControl();
  checkControls();
  checkAlarm();
}

void checkControls() {
  buttons.updateButtonState();
  static unsigned long lastButtonPressTime = 0;

  //Check for time since last action
  if(currentMode != displayMode) {
    //If over 1 minute go back to displayMode
    if(abs(millis() - lastButtonPressTime) > 60000) {
      setMode(displayMode);
    }
  }
  
  //Allows the mode to override the advance function
  //Useful for cleaning up variables before moving screen
  if (currentMode != setMusicMode) {
    //Manages mode
    if (buttons.getButtonDown(modeSwitchButton)) {
      lastButtonPressTime = millis();
      setMode(currentMode + 1);
    }
  }
  //Managing the sleep states and enter button
  bool sleepButtonPressed = buttons.getButtonDown(sleepButton);

  if (sleepButtonPressed) {
    lastButtonPressTime = millis();
    //also acts as enter/next button
    cursorPosition++;
    if (!sleepEnabled) {
      enableSleep();
    }
    else {
      disableSleep();
    }
  }

  //Manages the back button
  if (buttons.getButtonDown(backButton)) {
    lastButtonPressTime = millis();
    cursorPosition--;
    //Allows the mode to override the go back function
    //Useful for cleaning up variables before moving screen
    if (currentMode != setMusicMode) {
      //Go back a screen
      if (cursorPosition == -1) {
        setMode(currentMode - 1);
      }
    }
  }

  //Used for determining if idle
  if(buttons.getButton(upButton)) {
    lastButtonPressTime = millis();
  }

  if(buttons.getButton(downButton)) {
    lastButtonPressTime = millis();
  }
}

/*
   Manages all of the alarm states including
   what to do when it is time for the alarm to go off and
   when the alarm is going off and sleep is enabled/disabled
*/
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
  else if (alarmBuzzing) { //Happens when the alarm finishes or when the alarm is turned off
    alarmBuzzing = false;
    disableSleep();
    stopAlarm();
  }


  if (currentTime.hour() == alarmTime.hour && currentTime.minute() == alarmTime.minute && currentTime.second() == alarmTime.second && !sleepEnabled && isAlarmEnabled && !alarmBuzzing) {
    //random song
    if (usersAlarmChoice == 1) {
      chosenAlarmSong = chooseRandomSong();
    }

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
  if (usersAlarmChoice == 0 ) {
    tone(alarmBuzzPin, alarmFrequency);
  }
  else {
    playSongAtIndex(chosenAlarmSong);
  }
}

//Stops the alarm buzzing
void stopAlarm() {
  buzzer.resetSong();
  noTone(alarmBuzzPin);
}
/*
   Controls the clock and display
   Depending on what the current mode of the clock
   is.
   Modes:
   0-(Time Display) 1-(Set Time) 2-(Set Alarm) 3-(Set Alarm Music) 4-(Set LCD Color)
*/
void LCDControl() {
  //This must be changed before calling the screen function
  //because the screen function can change the mode
  //itself, making this incorrect when the function gives control
  //back to this one
  static uint8_t previousMode = 0;

  switch (currentMode) {
    //Display Time
    case 0:
      previousMode = currentMode;
      printDate();
      printTime(getCurrentTime());
      cursorPosition = 0; //Make sure using sleep didn't change it
      break;
    //Set time
    case 1:
      if (currentMode != previousMode) {
        previousMode = currentMode;
        lcd.setCursor(0, 0);
        lcd.print(F("Set Time:"));
        editTime(true, false);
      }
      else {
        editTime(false, false);
      }
      break;
    //Set Alarm
    case 2:
      if (currentMode != previousMode) {
        previousMode = currentMode;
        lcd.setCursor(0, 0);
        lcd.print(F("Set Alarm:"));
        editTime(true, true);
      }
      else {
        editTime(false, true);
      }
      break;
    //Set Alarm Music
    case 3:
      if (currentMode != previousMode) {
        previousMode = currentMode;
        lcd.setCursor(0, 0);
        lcd.print(F("Alarm Sound:"));
        setAlarmMusic(true);
      }
      else {
        setAlarmMusic(false);
      }
      break;
    //Set LCD Color
    case 4:
      if (currentMode != previousMode) {
        previousMode = currentMode;
        lcd.setCursor(0, 0);
        lcd.print(F("Set RGB:"));
        setRGBColor(true);
      }
      else {
        setRGBColor(false);
      }
      break;
  }
}

/*
   Sets the mode properly and resets
   variables for the new screen
*/
void setMode(int8_t modeNumber) {
  currentMode = wrap(0, maxMode, modeNumber);
  cursorPosition = 0;

  //Erase the previous screen
  lcd.setCursor(0, 0);
  lcd.print(F("                "));
  lcd.setCursor(0, 1);
  lcd.print(F("                "));
}

void setRGBColor(bool initialize) {

  static uint8_t redLight = 255;
  static uint8_t greenLight = 0;
  static uint8_t blueLight = 255;

  if (cursorPosition == 3) {
    setMode(displayMode);
    return;
  }

  int8_t desiredChange = 0;

  if (buttons.getButtonHeld(upButton, 20)) {
    desiredChange = 1;
  }
  else if (buttons.getButtonHeld(downButton, 20)) {
    desiredChange = -1;
  }

  switch (cursorPosition) {
    case 0:
      redLight = constrain(redLight + desiredChange, 0, 255);
      break;
    case 1:
      greenLight = constrain(greenLight + desiredChange, 0, 255);
      break;
    case 2:
      blueLight = constrain(blueLight + desiredChange, 0, 255);
      break;
  }

  //Printing the colors
  lcd.setCursor(0, 1);

  //Red light print
  if (cursorPosition == 0) {
    lcd.print(F("R"));
  }
  else {
    lcd.print(F("r"));
  }

  lcd.print(redLight);
  if (redLight < 10) {
    lcd.print(F("   "));
  }
  else if (redLight < 100) {
    lcd.print(F("  "));
  }
  else {
    lcd.print(F(" "));
  }

  //Green light print
  if (cursorPosition == 1) {
    lcd.print(F("G"));
  }
  else {
    lcd.print(F("g"));
  }

  lcd.print(greenLight);
  if (greenLight < 10) {
    lcd.print(F("   "));
  }
  else if (greenLight < 100) {
    lcd.print(F("  "));
  }
  else {
    lcd.print(F(" "));
  }

  //Blue light print
  if (cursorPosition == 2) {
    lcd.print(F("B"));
  }
  else {
    lcd.print(F("b"));
  }

  lcd.print(blueLight);
  if (blueLight < 10) {
    lcd.print(F("   "));
  }
  else if (blueLight < 100) {
    lcd.print(F("  "));
  }
  else {
    lcd.print(F(" "));
  }

  setBacklight(redLight, greenLight, blueLight);
}

/*
   Allows the user to choose what to play when the
   alarm goes off.
*/
void setAlarmMusic(bool initialize) {
  static uint8_t previousUserChoice = usersAlarmChoice;
  static bool sampleMode = false;
  static bool previousSampleMode = false;
  //Changing the users choice
  if (buttons.getButtonDown(upButton)) {
    usersAlarmChoice = wrap(0, 1 + songListLength, usersAlarmChoice + 1);
    sampleMode = false;
    previousSampleMode = false;
    stopAlarm();
  }
  else if (buttons.getButtonDown(downButton)) {
    usersAlarmChoice = wrap(0, 1 + songListLength, usersAlarmChoice - 1);
    sampleMode = false;
    previousSampleMode = false;
    stopAlarm();
  }

  //Clear the current song to make new for new one
  if (previousUserChoice != usersAlarmChoice) {
    lcd.setCursor(0, 1);
    lcd.print(F("                "));
  }

  //Submit song choice (Maybe trial the song)
  if (cursorPosition == 1) {
    sampleMode = !sampleMode;
    if (!sampleMode) {
      //Reset the song for next time sample mode is turned on
      stopAlarm();
    }
    cursorPosition = 0;
  }
  else if (cursorPosition == -1) { //If can't go back on current screen, go back to previous screen, also reset sampleMode
    sampleMode = false;
    previousSampleMode = false;
    stopAlarm();
    setMode(currentMode - 1);
    return;
  }

  //!initialize ensures that this screen doesn't get skipped while going forwards by calling setMode(currentMode + 1) twice
  if (buttons.getButtonDown(modeSwitchButton) && !initialize) {
    sampleMode = false;
    previousSampleMode = false;
    stopAlarm();
    setMode(currentMode + 1);
    return;
  }

  //Line 2
  lcd.setCursor(0, 1);

  switch (usersAlarmChoice) {
    default:
      chosenAlarmSong = usersAlarmChoice - 2;
      Song song;
      memcpy_P(&song, &alarmSongs[chosenAlarmSong], sizeof(song));
      lcd.print(song.name);
      if (sampleMode) {
        soundAlarm();
      }
      break;
    case 0:
      lcd.print("Normal Buzzer   ");
      if (sampleMode) {
        soundAlarm();
      }
      break;
    case 1:
      lcd.print("Random Song     ");
      if (sampleMode) {
        if (previousSampleMode != sampleMode) {
          chosenAlarmSong = chooseRandomSong();
        }
        soundAlarm();
      }
      break;
  }


  previousUserChoice = usersAlarmChoice;
  previousSampleMode = sampleMode;
}

/*
   Controls the editTime menu
   Set alarm true to edit alarm time
   Set alarm false to edit the actual time
*/
void editTime(bool initialize, bool alarm) {
  static struct Time desiredTime;

  if (initialize) {
    if (!alarm) {
      desiredTime = getCurrentTime();
    }
    else {
      desiredTime = alarmTime;
    }
  }

  //Controlling the blinking of whatever is being changed
  static bool displayNumber = true;
  //0.5seconds per blink
  if (timer(500)) {
    displayNumber = !displayNumber;
  }

  //If hit enter after finish setting time go back to main display
  if (cursorPosition == 3) {
    DateTime now = rtc.now();
    if (!alarm) {
      rtc.adjust(DateTime(now.year(), now.month(), now.day(), desiredTime.hour, desiredTime.minute, desiredTime.second));
    }
    else {
      alarmTime = desiredTime;
      //Save the new alarm time into memory
      EEPROM.put(eeAlarmAddress, alarmTime);
    }
    setMode(displayMode);
    return;
  }

  //Finding desired change
  int8_t timeChange = 0;

  if (buttons.getButtonDown(upButton)) {
    timeChange = 1;
  }
  else if (buttons.getButtonDown(downButton)) {
    timeChange = -1;
  }

  //Applying changes
  switch (cursorPosition) {
    case 0:
      desiredTime.hour = wrap(0, 23, desiredTime.hour + timeChange);
      break;
    case 1:
      desiredTime.minute = wrap(0, 59, desiredTime.minute + timeChange);
      break;
    case 2:
      desiredTime.second = wrap(0, 59, desiredTime.second + timeChange);
      break;
  }

  //update the desired time on the display
  printTime(desiredTime, (displayNumber) ? -1 : cursorPosition);
}

/*
   Plays the song stored in the song array at specified index
   This function will not continously play the song on a seperate thread.
   It must be repeatedly called with the same index for the song to finish.
   If you changed the song index, make sure you call buzzer.resetSong()
   for it to properly start the new one correctly.
*/
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

//Returns a random index in the alarmSongs array
int chooseRandomSong() {
  buzzer.resetSong();
  return random(0, songListLength);
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
   returns the current time
   in a struct format
*/
struct Time getCurrentTime() {
  DateTime currentTime = rtc.now();

  struct Time time;

  time.hour = currentTime.hour();
  time.minute = currentTime.minute();
  time.second = currentTime.second();

  return time;
}

/*
   Prints out the time in format HH:MM:SS P/A
   On the second line of the LCD
   Using the given time struct
   Exempt is used as -1, 0, 1, or 2
   It will put a blank space instead of the
   time value in hour, minute, or sec
   -1 = ignore
*/
void printTime(struct Time time, int exempt) {
  bool AM = true;
  if (time.hour >= 12) {
    AM = false;
    if (time.hour >= 13)
      time.hour -= 12;
  }
  else if (time.hour == 0) {
    time.hour = 12;
  }

  lcd.setCursor(0, 1);

  //Used as a way to make the number not appear
  if (exempt == 0) {
    lcd.print(F("  "));
  }
  else {
    if (time.hour < 10)
      lcd.print(0);
    lcd.print(time.hour);
  }
  lcd.print(F(":"));

  //Used as a way to make the number not appear
  if (exempt == 1) {
    lcd.print(F("  "));
  }
  else {
    if (time.minute < 10)
      lcd.print(0);
    lcd.print(time.minute);
  }
  lcd.print(F(":"));

  if (exempt == 2) {
    lcd.print(F("  "));
  }
  else {
    if (time.second < 10)
      lcd.print(0);
    lcd.print(time.second);
  }
  lcd.print((AM) ? F(" A") : F(" P"));
}

void printTime(struct Time time) {
  printTime(time, -1);
}

//min inclusive, max inclusive
int wrap(int min, int max, int number) {
  //Ex: 0, 60, -1 = 60
  //0, 60, -2 = 59
  if (number < min) {
    int difference = min - number;
    return max + difference - 1;
  }
  else if (number > max) {
    int difference = number - max;
    return min + difference - 1;
  }
  return number;
}

/*
   This timer will return true every inputed amount of seconds.
   Depending on when the timer is canceled, it could
   trigger faster then expected next time it is called.

   Should not be used for very time sensitive things, and
   can only be used for one purpose at a time, two functions
   should not use this at the same time
*/
bool timer(uint32_t milliseconds) {
  static unsigned long start = 0;

  if (abs(millis() - start) > milliseconds) {
    start = millis();
    return true;
  }

  return false;
}

void setBacklight(uint8_t r, uint8_t g, uint8_t b) {
  // normalize the light colors. Some are brighter then others
  r = map(r, 0, 255, 0, 100);
  g = map(g, 0, 255, 0, 150);

  //set to brightness level
  r = map(r, 0, 255, 0, brightness);
  g = map(g, 0, 255, 0, brightness);
  b = map(b, 0, 255, 0, brightness);

  //backlight is common annode, invert the signal
  r = map(r, 0, 255, 255, 0);
  g = map(g, 0, 255, 255, 0);
  b = map(b, 0, 255, 255, 0);

  //Set the backlight
  analogWrite(redLightPin, r);
  analogWrite(greenLightPin, g);
  analogWrite(blueLightPin, b);
}

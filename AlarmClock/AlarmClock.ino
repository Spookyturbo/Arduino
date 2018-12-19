#include <avr/pgmspace.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>

LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
RTC_DS3231 rtc;

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

const char* const months[] PROGMEM = 
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

void setup() {
  Serial.begin(9600);

  //setting up the display
  lcd.begin(16, 2);
  //setting up the clock
  if(!rtc.begin()) {
    Serial.println(F("Clock could not properly initialise"));
  }

  if(rtc.lostPower()) {
    Serial.println(F("Clock lost power, defaulting to compile time."));
    //Set time to compile time
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop() {
  printDate();
  printTime();
}

void printDate() {
  DateTime currentTime = rtc.now();
  
  char month[4];
  strcpy_P(month, (char*)pgm_read_word(&(months[currentTime.month() - 1])));
  
  uint8_t day = currentTime.day();
  uint8_t year = currentTime.year();
  
  char weekDay[4];
  strcpy_P(weekDay, (char*)pgm_read_word(&(weekDays[currentTime.dayOfTheWeek()])));

  lcd.setCursor(0, 0);
  lcd.print(weekDay);
  lcd.print(", ");
  lcd.print(month);
  lcd.print(" ");
  lcd.print(day);
  
}

void printTime() {
  DateTime currentTime = rtc.now();

  uint8_t hour = currentTime.hour();
  uint8_t minute = currentTime.minute();
  uint8_t second = currentTime.second();

  bool AM = true;
  if(hour >= 12) {
    AM = false;
    if(hour >= 13)
    hour -= 12;
  }
  else if(hour == 0) {
    hour = 12;
  }
  
  lcd.setCursor(0, 1);

  if(hour < 10)
    lcd.print(0);
  lcd.print(hour);
  lcd.print(F(":"));

  if(minute < 10)
    lcd.print(0);
  lcd.print(minute);
  lcd.print(F(":"));

  if(second < 10)
    lcd.print(0);
  lcd.print(second);
  lcd.print((AM) ? F(" AM") : F(" PM"));
}

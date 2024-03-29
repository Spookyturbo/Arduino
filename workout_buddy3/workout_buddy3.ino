// IMPORTANT: Elegoo_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Elegoo_TFTLCD.h FOR SETUP.
//Technical support:goodtft@163.com

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <EEPROM.h>

// Timer Settings
#include <TimeLib.h>
#define buzzerPin 52
int setTimeVal = 10;
int rndnum = 32000;
time_t t;
time_t pt;
int countDown = 0;
int countDownVal = 0;
int prevCountDownVal = 0;

int buttonPressed = 0;
#define eepromTimePosition 0

// constants won't change. They're used here to set pin numbers:
const int buttonPin = 50;     // the number of the pushbutton pin
const int ledPin =  37;      // the number of the LED pin
int led_status = 0;

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status


// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******************* UI details */
#define BUTTON_X 50
#define BUTTON_Y 90
#define BUTTON_W 80
#define BUTTON_H 30
#define BUTTON_SPACING_X 30
#define BUTTON_SPACING_Y 2
#define BUTTON_TEXTSIZE 2

// text box where numbers go
#define TEXT_X 10
#define TEXT_Y 10
#define TEXT_W 300
#define TEXT_H 50
#define TEXT_TSIZE 3
#define TEXT_TCOLOR ILI9341_MAGENTA
// the data (phone #) we store in the textfield
#define TEXT_LEN 12
char textfield[TEXT_LEN + 1] = "";
uint8_t textfield_i = 0;

bool buttonPushed = false;
#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920
// We have a status line for like, is FONA working
#define STATUS_X 10
#define STATUS_Y 65

bool screenPressed = false;

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Elegoo_TFTLCD tft;

Elegoo_GFX_Button buttons[15];
/* create 15 buttons, in classic candybar phone style */
char buttonlabels[15][6] = {"Start", "Clr", "Stop", "1", "2", "3", "4", "5", "6", "7", "8", "9", "*", "0", "#" };
uint16_t buttoncolors[15] = {ILI9341_DARKGREEN, ILI9341_DARKGREY, ILI9341_RED,
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE,
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE,
                             ILI9341_BLUE, ILI9341_BLUE, ILI9341_BLUE,
                             ILI9341_ORANGE, ILI9341_BLUE, ILI9341_ORANGE
                            };

void setup(void) {
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  
#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.8\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.8\" TFT Breakout Board Pinout"));
#endif

  Serial.print("TFT size is "); Serial.print(tft.width()); Serial.print("x"); Serial.println(tft.height());

  tft.reset();

  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  } else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if (identifier == 0x0101)
  {
    identifier = 0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier = 0x9341;

  }

  tft.begin(identifier);
  tft.setRotation(1);
  tft.fillScreen(BLACK);

  // create buttons
  for (uint8_t row = 0; row < 5; row++) {
    for (uint8_t col = 0; col < 3; col++) {
      buttons[col + row * 3].initButton(&tft, BUTTON_X + col * (BUTTON_W + BUTTON_SPACING_X),
                                        BUTTON_Y + row * (BUTTON_H + BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
                                        BUTTON_W, BUTTON_H, ILI9341_WHITE, buttoncolors[col + row * 3], ILI9341_WHITE,
                                        buttonlabels[col + row * 3], BUTTON_TEXTSIZE);
      buttons[col + row * 3].drawButton();
    }
  }

  // create 'text field'
  tft.drawRect(TEXT_X, TEXT_Y, TEXT_W, TEXT_H, ILI9341_WHITE);

  setTime(21, 42, 0, 18, 9, 2017);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  int LMB = EEPROM.read(eepromTimePosition);
  int RMB = EEPROM.read(eepromTimePosition + 1);
  countDownVal = ((LMB << 8) + RMB);
  prevCountDownVal = countDownVal;
  //Serial.println("prevCountDownVal is: ");
  //Serial.println(prevCountDownVal);
  sprintf(textfield, "%d", countDownVal);
  textfield_i = strlen(textfield);
  writeTime(countDownVal);

  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);

  //Ensure random numbers after each restart
  randomSeed(analogRead(5));
}
// Print something in the mini status bar with either flashstring
void status(const __FlashStringHelper *msg) {
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(msg);
}
// or charstring
void status(char *msg) {
  tft.fillRect(STATUS_X, STATUS_Y, 240, 8, ILI9341_BLACK);
  tft.setCursor(STATUS_X, STATUS_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  tft.print(msg);
}

#define MINPRESSURE 10
#define MAXPRESSURE 1000
void loop(void) {
  buttonPushed = false;
  //digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  //digitalWrite(13, LOW);

  // if sharing pins, you'll need to fix the directions of the touchscreen pins
  //pinMode(XP, OUTPUT);
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  //pinMode(YM, OUTPUT);

  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!

  // p = ts.getPoint();
  /*
    if (ts.bufferSize()) {

    } else {
    // this is our way of tracking touch 'release'!
    p.x = p.y = p.z = -1;
    }*/

  // Scale from ~0->4000 to tft.width using the calibration #'s
  /*
    if (p.z != -1) {
    p.x = map(p.x, TS_MINX, TS_MAXX, 0, tft.width());
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    Serial.print("("); Serial.print(p.x); Serial.print(", ");
    Serial.print(p.y); Serial.print(", ");
    Serial.print(p.z); Serial.println(") ");
    }*/
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.height(), 0);
    p.y = (tft.width() - map(p.y, TS_MINY, TS_MAXY, 0, tft.width()));
    int x = p.x;
    p.x = p.y;
    p.y = x;
    //Serial.print("X: ");
    //Serial.println(p.x);
    //Serial.print(" Y: ");
    //Serial.println(p.y);
    screenPressed = true;
  }
  else {
    for (uint8_t b = 0; b < 15; b++) {
      buttons[b].press(false);  // tell the button it is NOT pressed
    }
    screenPressed = false;
  }

  // go thru all the buttons, checking if they were pressed
  if (screenPressed || buttonPressed)
  {
    for (uint8_t b = 0; b < 15; b++)
    {
      if (buttons[b].contains(p.x, p.y) && !buttonPushed)
      {
        //Serial.print("Pressing: "); Serial.println(b);
        buttons[b].press(true);  // tell the button it is pressed
        buttonPushed = true;
      }
      else
      {
        buttons[b].press(false);  // tell the button it is NOT pressed
      }
    }
  }

  // now we can ask the buttons if their state has changed
  for (uint8_t b = 0; b < 15; b++) {
    if (buttons[b].justReleased()) {
      // Serial.print("Released: "); Serial.println(b);
      buttons[b].drawButton();  // draw normal
    }

    if (buttons[b].justPressed() || buttonPressed)
    {
      if (!buttonPressed)
        buttons[b].drawButton(true);  // draw invert!

      // if a numberpad button, append the relevant # to the textfield
      if (b >= 3) {
        if (textfield_i < TEXT_LEN) {
          textfield[textfield_i] = buttonlabels[b][0];
          textfield_i++;
          textfield[textfield_i] = 0; // zero terminate

          // fona.playDTMF(buttonlabels[b][0]);
        }
      }

      // clr button! delete char
      if ((b == 1) && (countDown == 0)) {

        // debug
        //Serial.print("b is: ");
        //Serial.println(b);
        //Serial.print("textfield is: ");
        //Serial.println(textfield);
        //Serial.print("textfield_i is: ");
        //Serial.println(textfield_i);

        textfield[textfield_i] = 0;
        if (textfield_i > 0) {
          //if (textfield > 0) {
          textfield_i--;
          textfield[textfield_i] = ' ';

          // debug
          //Serial.print("b is: ");
          //Serial.println(b);
          //Serial.print("textfield is: ");
          //Serial.println(textfield);
          //Serial.print("textfield_i is: ");
          //Serial.println(textfield_i);


        }
      }

      // update the current text field
      tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
      tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
      tft.setTextSize(TEXT_TSIZE);
      tft.print(textfield);

      // its always OK to just hang up
      if ((b == 2) || (buttonPressed && countDown))
      {
        status(F("Stopped"));
        b = 20;
        buttonPressed = 0;
        countDown = 0;
        countDownVal = atoi(textfield);
        //Serial.println("b2 called");
        tone(buzzerPin, 31, 100);
        delay(200);
        tone(buzzerPin, 31, 100);
        // turn LED off:
        digitalWrite(ledPin, LOW);
        led_status = 0;

        //fona.hangUp();

      }

      //Serial.println("between buttons");

      // we dont really check that the text field makes sense
      // just try to call
      if ((b == 0) || (buttonPressed && (countDown == 0)))
      {
        tone(buzzerPin, 31, 100);
        buttonPressed = 0;

        char* randomSplit = strchr(textfield, '#');

        //Handles the random function
        if (randomSplit) {
          //Converts up to the #
          int min = atoi(textfield);
          //Increment pointer past # then convert (Max is exclusive, raise by 1)
          int max = atoi(++randomSplit) + 1;

          //If no max was entered, use largest value possible
          if(max == 1) {
            max = 32767; //Largest signed int value
          }
          
//          Serial.print("Min: "); Serial.print(min); Serial.print(" Max: "); Serial.println(max);
//          int value = random(min, max);
//          Serial.print("Value: "); Serial.println(value);
          
          countDownVal = random(min, max);
        }
        else {
          //If not generating a random number, revert to default
          countDownVal = atoi(textfield);
        }

        if (countDownVal > 0)
        {
          status(F("Counting Down"));
          // turn LED on:
          digitalWrite(ledPin, HIGH);
          led_status = 1;

          //Serial.print("Counting Down "); Serial.print(textfield);
          //Serial.print("buttonPressed = "); Serial.println(buttonPressed);
          //             Serial.print("b = "); Serial.println(b);

          countDown = 1;
          if (prevCountDownVal != countDownVal)
          {
            //Serial.println("writing eeprom");
            EEPROM.write(eepromTimePosition, (countDownVal >> 8));
            EEPROM.write(eepromTimePosition + 1, ((byte) countDownVal));
            prevCountDownVal = countDownVal;
          }
          pt = t = now();
        }
        else
        {
          status(F("Must Select Value > 0"));
        }

        //fona.callPhone(textfield);
      }

      delay(100); // UI debouncing

    }

    // check timer here
    // debug
    //Serial.println("looping");
    //writeTime(10);
    if (countDown == 1)
    {
      t = now();
      if (pt != t)
      {
        countDownVal--;
        writeTime(countDownVal);

        // toggle LED
        if (led_status == 0)
        {
          led_status = 1;
          digitalWrite(ledPin, HIGH);

        }
        else
        {
          led_status = 0;
          digitalWrite(ledPin, LOW);
        }

        pt = t;
        if (countDownVal == 0)
        {
          setOffAlarm();
          countDown = 0;
          buttonPressed = 0;
          delay(1000);
          countDownVal = atoi(textfield);
          writeTime(textfield);
          //writeTime(countDownVal);
          status(F("Time to Workout Buddy!"));
          // turn LED off:
          digitalWrite(ledPin, LOW);
          led_status = 0;

        }
      }
    }

  }

  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    led_status = 1;
    //Serial.println("button pressed");
    buttonPressed = 1;
    delay(500);
  } else {
    // turn LED off:
    //digitalWrite(ledPin, LOW);
  }

}

void setOffAlarm() {
  int x = 0;
  tone(buzzerPin, 31, 2000);

  for (x = 0; x < 8; x++)
  {
    delay(250);
    if (led_status == 1)
    {
      led_status = 0;
      digitalWrite(ledPin, LOW);

    }
    else
    {
      led_status = 1;
      digitalWrite(ledPin, HIGH);
    }

  }

  digitalWrite(ledPin, LOW);
  led_status = 0;

}

void writeTime(int currentTime)
{
  char displayBuf[13];

  tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
  tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
  tft.setTextSize(TEXT_TSIZE);
  sprintf(displayBuf, "%-12d", currentTime);
  tft.print(displayBuf);

}

void writeTime(char* currentTime) {
  tft.setCursor(TEXT_X + 2, TEXT_Y + 10);
  tft.setTextColor(TEXT_TCOLOR, ILI9341_BLACK);
  tft.setTextSize(TEXT_TSIZE);
  tft.print(currentTime);
}

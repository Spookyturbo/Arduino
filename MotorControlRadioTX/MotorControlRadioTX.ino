#include <SPI.h>
#include <RH_RF69.h>

/*********Radio********/
#define RF69_FREQ 915.0
#define RFM69_INT     3  // 
#define RFM69_CS      4  //
#define RFM69_RST     2  // "A"

RH_RF69 rf69(RFM69_CS, RFM69_INT);

#define SET_SPEED 0x05
#define ENABLE_DRIVE 0x06
#define DISABLE_DRIVE 0x07
#define ENABLE_PEDAL 0x08
#define DISABLE_PEDAL 0x09
#define GET_PEDAL 0x10
#define SET_MOTOR_TIMEOUT 0x11
#define GET_MOTOR_TIMEOUT 0x12
#define GET_LEVER 0x13
#define FEED_TIMEOUT 0x14

/*********Motors*******/
//All in microseconds
const uint16_t maxPWM = 2000;
const uint16_t minPWM = 1000;
const uint16_t neutralPWM = 1500;

const uint8_t potPin = A0;
const uint8_t fwdButton = 6;
const uint8_t speedIncreaseButton = 5;
const uint8_t revButton = 7;
const uint8_t powerButton = 8;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  //Radio Config
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  //Reset radio
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");

  if (!rf69.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  rf69.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true

  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                  };

  rf69.setEncryptionKey(key);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  //Set up motor
  pinMode(fwdButton, INPUT_PULLUP);
  pinMode(revButton, INPUT_PULLUP);
  pinMode(powerButton, INPUT_PULLUP);
  pinMode(speedIncreaseButton, INPUT_PULLUP);

  //Initialize some stuff on the car
  setMotorTimeout(200);
  disableDrive();
}

void loop() {
  //Used to not spam update the radio
  static int lastPWM = 0;
  static unsigned long lastRemoteControl = 0;
  int PWMValue;

  //  int input = analogRead(potPin);
  //  //Map the analog input to a usable PWM signal
  //  int PWMValue = map(input, 0, 1022, minPWM, maxPWM);
  //
  //  //Create a larger deadzone for stopping
  //  if(PWMValue > 1450 && PWMValue < 1550) {
  //    PWMValue = neutralPWM;
  //  }

  //Check for calling to enable/disable
  if (powerButtonPressed()) {
    enableDrive();
  }
  
  //Remote drive control
  if (!digitalRead(fwdButton)) {
    if (!digitalRead(speedIncreaseButton) && lastPWM <= 1300 && lastPWM >= 1003) {
      PWMValue = lastPWM - 3;
    }
    else if (digitalRead(speedIncreaseButton)) {
      PWMValue = 1300;
    }
    lastRemoteControl = millis();
  }
  else if (!digitalRead(revButton)) {
    PWMValue = 1700;
    lastRemoteControl = millis();
  }
  else if(millis() - lastRemoteControl > 4000){ //If controller hasn't been in use for 4 seconds, use pedal on board (Kid control)
    bool lever = getLever();
    bool pedal = getPedalPressed();
    delay(10);
    Serial.print("Lever: "); Serial.print(lever); Serial.print(" Pedal: "); Serial.print(pedal); Serial.print(" PWM: "); Serial.println(PWMValue);
    if(pedal) {
      if(lever) {
        PWMValue = 1300;
      }
      else {
        PWMValue = 1700;
      }
    }
    else {
      PWMValue = neutralPWM;
    }
  }
  else {
    PWMValue = neutralPWM;
  }
  
  //This might need to be used to stop stuttering
//  if (PWMValue <= lastPWM + 2 && PWMValue >= lastPWM - 1) {
//    //Set speed wont be called so feed the timeout
//    feedTimeout();
//    return;
//  }
  lastPWM = PWMValue;
  //Also feeds the timeout
  setSpeed(PWMValue);
}



//1000-2000 (1500 being neutral)
void setSpeed(uint16_t speed) {
  Serial.print("Setting speed "); Serial.println(speed);
  uint8_t info[] = {(uint8_t) SET_SPEED, (uint8_t) speed, (uint8_t) (speed >> 8)};
  rf69.send(info, sizeof(info));
  rf69.waitPacketSent();
}

void enableDrive() {
  Serial.println("Enabling drive");
  uint8_t request[] = {(uint8_t) ENABLE_DRIVE };
  rf69.send(request, sizeof(request));
  rf69.waitPacketSent();
}

void disableDrive() {
  uint8_t request[] = {(uint8_t) DISABLE_DRIVE };
  rf69.send(request, sizeof(request));
  rf69.waitPacketSent();
}

bool getPedalPressed() {
  uint8_t request[] = {(uint8_t) GET_PEDAL};
  rf69.send(request, sizeof(request));
  rf69.waitPacketSent();

  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf69.waitAvailableTimeout(400)) {
    if (rf69.recv(buf, &len)) {
      return buf[0];
    }
  }

  return false;
}

//Returns whether lever is flipped up or not
bool getLever() {
  uint8_t request[] = {(uint8_t) GET_LEVER};
  rf69.send(request, sizeof(request));
  rf69.waitPacketSent();

  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf69.waitAvailableTimeout(400)) {
    if (rf69.recv(buf, &len)) {
      return buf[0];
    }
  }

  return false;
}

void setMotorTimeout(unsigned long timeout) {
  uint8_t request[] = {(uint8_t) SET_MOTOR_TIMEOUT, (uint8_t) timeout, (uint8_t) (timeout >> 8), (uint8_t) (timeout >> 16), (uint8_t) (timeout >> 24)};
  rf69.send(request, sizeof(request));
  rf69.waitPacketSent();
}

unsigned long getMotorTimeout() {
  uint8_t request[] = {(uint8_t) GET_MOTOR_TIMEOUT};
  rf69.send(request, sizeof(request));
  rf69.waitPacketSent();

  uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

  if (rf69.waitAvailableTimeout(400)) {
    if (rf69.recv(buf, &len)) {
      unsigned long timeout = buf[3] << 24 | buf[2] << 16 | buf[1] << 8 | buf[0];
      return timeout;
    }
  }

  return 0;
}

void feedTimeout() {
  uint8_t request[] = {(uint8_t) FEED_TIMEOUT};
  rf69.send(request, sizeof(request));
  rf69.waitPacketSent();
}

bool powerButtonPressed() {
  static bool previousState = false;
  bool state = !digitalRead(powerButton);
  
  if (state && !previousState) {
    previousState = state;
    return true;
  }

  previousState = state;
  return false;
}

#include <SPI.h>
#include <RH_RF69.h>
#include <Servo.h>

//>1500 = backwards <1500 = forwards

/*********Radio********/
#define RF69_FREQ 915.0
#define RFM69_INT     3  // 
#define RFM69_CS      4  //
#define RFM69_RST     2  // "A"

RH_RF69 rf69(RFM69_CS, RFM69_INT);
/*********Motor*******/
const uint8_t enableDrive = 8;
bool enabled = false;
const uint8_t servoPin = 6;

const uint8_t pedalInput = A0;
const uint8_t leverDown = A1;
const uint8_t lever2 = A2;
const uint8_t leverUp = A3;
const uint8_t lever4 = A4;

Servo motor;

/*******SpeedControl********/
//Max change in pwm microseconds per second
const uint16_t maxAcceleration = 500;
const uint16_t neutralPWM = 1500;
const uint16_t maxPWM = 2000;
const uint16_t minPWM = 1000;
const uint16_t maxPedalPWM = 1700;
const uint16_t minPedalPWM = 1300;

/*********ModeControl***********/
const uint16_t remoteTimeout = 4000;

void setup() {
  Serial.begin(9600);

  //Radio config
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  //Radio reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69.init()) {
    Serial.println(F("RFM69 radio init failed"));
    while (1);
  }

  Serial.println("RFM69 radio init OK!");

  if (rf69.setFrequency(RF69_FREQ)) {
    Serial.println("Set frequency failed");
  }

  rf69.setTxPower(20, true);

  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                  };

  rf69.setEncryptionKey(key);
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  //Set up motors
  motor.attach(servoPin);
  pinMode(enableDrive, OUTPUT);
  pinMode(pedalInput, INPUT);
  pinMode(leverUp, INPUT);
  pinMode(lever2, INPUT);
  pinMode(leverDown, INPUT);
  pinMode(lever4, INPUT);
  digitalWrite(enableDrive, LOW);
}

void loop() {
  //Used to time out radio control overriding pedal
  static unsigned long lastRemoteTime = 0;
  //used to calculate delta time
  static unsigned long lastTime = 0;
  static float lastPWMValue = neutralPWM;
  static uint16_t PWMValue = neutralPWM;
  static uint16_t remotePWM = neutralPWM;
  static long pedalValue = 0;

  pedalValue = digitalRead(pedalInput);

  //Read from the radio the PWMValue
  if (rf69.available()) {
    uint8_t buf[2];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (len) {
        //Enable/Disable
        if(len == 1) {
          enabled = !enabled;
          digitalWrite(enableDrive, enabled);
          delay(1000);
          return;
        }
        remotePWM = (buf[1] << 8) | buf[0];
        //Serial.print("Remote: "); Serial.println(remotePWM);
        if (remotePWM != neutralPWM) {
          lastRemoteTime = millis();
        }
      }
    }
  }

  //Driving from the remote
  if (millis() - lastRemoteTime < remoteTimeout) {
    PWMValue = remotePWM;
  }
  else { //Driving from kid control
    if (digitalRead(leverUp)) {                           //Min is forward
      PWMValue = (digitalRead(pedalInput)) ? minPedalPWM : neutralPWM;
    }
    else {
      PWMValue = (digitalRead(pedalInput)) ? maxPedalPWM : neutralPWM;
    }
  }

  //Some speed tuning
  if (abs(PWMValue - lastPWMValue) > maxAcceleration && PWMValue > lastPWMValue) {
    lastPWMValue += maxAcceleration * ((millis() - lastTime) / 1000.0);
    motor.writeMicroseconds(floor(lastPWMValue));
  }
  else {
    lastPWMValue = PWMValue;
    motor.writeMicroseconds(PWMValue);
  }
  lastTime = millis();
  Serial.println(PWMValue);
}

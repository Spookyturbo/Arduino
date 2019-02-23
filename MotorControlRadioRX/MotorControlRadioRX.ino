#include <SPI.h>
#include <RH_RF69.h>
#include <Servo.h>

//>1500 = backwards <1500 = forwards

/*********Radio********/
#define RF69_FREQ 915.0
#define RFM69_INT     3  // 
#define RFM69_CS      4  //
#define RFM69_RST     2  // "A"

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

RH_RF69 rf69(RFM69_CS, RFM69_INT);
/*********Motor*******/
const uint8_t enableDrive = 8;
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

bool pedalEnabled = false;
uint16_t PWMValue = neutralPWM;
bool driveEnabled = false;
unsigned long motorTimeout = 20; //Time in milliseconds
unsigned long lastFed = 0;

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

void checkForUpdates() {
  //Read from the radio the PWMValue
  //Serial.println(rf69.available());
  if (rf69.available()) {
    uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (len) {
        //if(buf[0] == 0x06)
          //Serial.print(buf[0], HEX); Serial.print(" ");Serial.print(buf[1]); Serial.print(" ");Serial.print(buf[2]); Serial.print(" ");Serial.print(buf[3]); Serial.print(" ");Serial.print(buf[4]); Serial.println(" ");
        uint8_t command = buf[0];
        if (command == SET_SPEED) {
          //Serial.println("Setting speed");
          PWMValue = buf[2] << 8 | buf[1];
          lastFed = millis();
        }
        else if (command == ENABLE_DRIVE) {
          Serial.print("Enable drive ");
          Serial.print(driveEnabled);
          driveEnabled = true;
          digitalWrite(enableDrive, driveEnabled);
          Serial.println(driveEnabled);
          lastFed = millis();
        }
        else if (command == DISABLE_DRIVE) {
          //Serial.println("Disable drive");
          driveEnabled = false;
          digitalWrite(enableDrive, driveEnabled);
        }
        else if (command == ENABLE_PEDAL) {
          //Serial.println("Enable pedal");
          pedalEnabled = true;
        }
        else if (command == DISABLE_PEDAL) {
          //Serial.println("Disable pedal");
          pedalEnabled = false;
        }
        else if (command == GET_PEDAL) {
          //Serial.println("Get pedal");
          uint8_t pedalValue[] = {(uint8_t) digitalRead(pedalInput)};
          rf69.send(pedalValue, sizeof(pedalValue));
          rf69.waitPacketSent();
        }
        else if (command == GET_LEVER) {
          //Serial.println("Get lever");
          uint8_t leverValue[] = {(uint8_t) digitalRead(leverUp)};
          rf69.send(leverValue, sizeof(leverValue));
          rf69.waitPacketSent();
        }
        else if (command == GET_MOTOR_TIMEOUT) {
          //Serial.println("Get motor timeout");
          motorTimeout = buf[4] << 24 | buf[3] << 16 | buf[2] << 8 | buf[1];
        }
        else if (command == SET_MOTOR_TIMEOUT) {
          //Serial.println("Set motor timeout");
          uint8_t info[] = {(uint8_t) motorTimeout, (uint8_t) (motorTimeout >> 8), (uint8_t) (motorTimeout >> 16), (uint8_t) (motorTimeout >> 24)};
          rf69.send(info, sizeof(info));
          rf69.waitPacketSent();
        }
        else if (command == FEED_TIMEOUT) {
          //Serial.println("Feed timeout");
          lastFed = millis();
        }
        else {
          Serial.print("Unrecognized command: ");
          Serial.println(command);
        }
      }
    }
  }
}

void loop() {
  checkForUpdates();
  //Serial.print("Drive enabled: ");Serial.println(driveEnabled);
  //If the speed wasn't update in the specified time frame, disable motors for safety reasons
  if (millis() - lastFed > motorTimeout) {
    //driveEnabled = false;
    //digitalWrite(enableDrive, driveEnabled);
    //Should this disable or just set speed to neutral?
  }
  Serial.println(PWMValue);
  //Update motor controller
  if (driveEnabled) {
    //motor.writeMicroseconds(PWMValue);
  }
  else {
    motor.writeMicroseconds(neutralPWM);
  }
}

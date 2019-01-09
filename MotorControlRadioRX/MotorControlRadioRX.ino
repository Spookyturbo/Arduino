#include <SPI.h>
#include <RH_RF69.h>
#include <Servo.h>
//1000-2000 - 1500 = no movement

/*********Radio********/
#define RF69_FREQ 915.0
#define RFM69_INT     3  // 
#define RFM69_CS      4  //
#define RFM69_RST     2  // "A"
#define LED           13

RH_RF69 rf69(RFM69_CS, RFM69_INT);
/*********Inputs*******/
#define servoPin 6

Servo motor;

int motorSpeed = 1600;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Feather RFM69 TX Test!");
  Serial.println();

  // manual reset
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

  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  //Set up motor
  motor.attach(servoPin);
}

void loop() {

  if (rf69.available()) {

    uint8_t buf[2];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (len) {
        uint16_t PWMValue = (buf[1] << 8) + buf[0];
        motorSpeed = PWMValue;
      }
    }
  }
  Serial.println(motorSpeed);
  motor.writeMicroseconds(motorSpeed);

}

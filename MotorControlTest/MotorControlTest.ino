#include <SPI.h>
#include <RH_RF69.h>
#include <Servo.h>

/*********Radio********/
#define RF69_FREQ 915.0
#define RFM69_INT     3  // 
#define RFM69_CS      4  //
#define RFM69_RST     2  // "A"

RH_RF69 rf69(RFM69_CS, RFM69_INT);
/*********Motor*******/
const uint8_t servoPin = 6;
Servo motor;

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
}

void loop() {
  static uint16_t PWMValue = 1500;

  //Read from the radio the PWMValue
  if (rf69.available()) {

    uint8_t buf[2];
    uint8_t len = sizeof(buf);
    if (rf69.recv(buf, &len)) {
      if (len) {
        PWMValue = (buf[1] << 8) | buf[0];
      }
    }
  }

  Serial.print(F("Received: ")); Serial.println(PWMValue);
  motor.writeMicroseconds(PWMValue);
}

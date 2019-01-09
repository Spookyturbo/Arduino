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
//#define servoPin 6
#define button 7

//Servo motor;

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
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
                    
  rf69.setEncryptionKey(key);
  
  pinMode(LED, OUTPUT);
  
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  //Set up motor
  pinMode(A0, INPUT);
  pinMode(button, INPUT);
  //motor.attach(servoPin);
}

int lastSpeed = 1500;

void loop() {
  // put your main code here, to run repeatedly: 

  float percent;
  float input = analogRead(A0);

  if(input >= 500 && input <= 520) {
    percent = 0;
  }
  else {
    input -= 500.0;
    percent = (input / 500) * 100;
  }

  int PWMValue = percent * 5 + 1500;
  Serial.println(PWMValue);
  if(!digitalRead(button)) {
   PWMValue = 1500; 
  }

  if(PWMValue <= lastSpeed + 4 && PWMValue >= lastSpeed - 1) {
    return;
  }
  lastSpeed = PWMValue;
  uint8_t infoSend[2] = {(uint8_t) PWMValue, (uint8_t) (PWMValue >> 8)};
  rf69.send(infoSend, 2);
  rf69.waitPacketSent();
  
  //motor.writeMicroseconds(PWMValue);
  
}

#include <SPI.h>
#include <RH_RF69.h>

/*********Radio********/
#define RF69_FREQ 915.0
#define RFM69_INT     3  // 
#define RFM69_CS      4  //
#define RFM69_RST     2  // "A"

RH_RF69 rf69(RFM69_CS, RFM69_INT);
/*********Motors*******/
//All in microseconds
const uint16_t maxPWM = 2000;
const uint16_t minPWM = 1000;
const uint16_t neutralPWM = 1500;

/*********Inputs*******/
const uint8_t potPin = A0;

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
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
                    
  rf69.setEncryptionKey(key);
  
  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  //Set up motor
  pinMode(potPin, INPUT);
}

void loop() {
  //Used to not spam update the radio
  static int lastPWM = 0;
  
  int input = analogRead(potPin);
  //Map the analog input to a usable PWM signal
  int PWMValue = map(input, 0, 1022, minPWM, maxPWM);

  //Create a larger deadzone for stopping
  if(PWMValue > 1450 && PWMValue < 1550) {
    PWMValue = neutralPWM;
  }
  
  Serial.print(F("Received: ")); Serial.println(PWMValue);

  //This might need to be used to stop stuttering
//  if(PWMValue <= lastPWM + 4 && PWMValue >= lastSpeed - 1) {
//    return;
//  }
//  lastPWM = PWMValue;

  uint8_t infoSend[2] = {(uint8_t) PWMValue, (uint8_t) (PWMValue >> 8)};
  rf69.send(infoSend, 2);
  rf69.waitPacketSent();
}

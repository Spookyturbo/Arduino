/*
ESP8266 library

When you use with UNO board, uncomment the follow line in uartWIFI.h.
#define UNO

When you use with MEGA board, uncomment the follow line in uartWIFI.h.
#define MEGA

Connection:
When you use it with UNO board, the connection should be like these:
ESP8266_TX->D0
ESP8266_RX->D1
ESP8266_CH_PD->3.3V
ESP8266_VCC->3.3V
ESP8266_GND->GND

FTDI_RX->D3			//The baud rate of software serial can't be higher that 19200, so we use software serial as a debug port
FTDI_TX->D2

When you use it with MEGA board, the connection should be like these:
ESP8266_TX->RX1(D19)
ESP8266_RX->TX1(D18)
ESP8266_CH_PD->3.3V
ESP8266_VCC->3.3V
ESP8266_GND->GND

When you want to output the debug information, please use DebugSerial. For example,

DebugSerial.println("hello");


Note:	The size of message from ESP8266 is too big for arduino sometimes, so the library can't receive the whole buffer because  
the size of the hardware serial buffer which is defined in HardwareSerial.h is too small.

Open the file from \arduino\hardware\arduino\avr\cores\arduino\HardwareSerial.h.
See the follow line in the HardwareSerial.h file.

#define SERIAL_BUFFER_SIZE 64

The default size of the buffer is 64. Change it into a bigger number, like 256 or more.





*/


#define SSID       "ELECFREAKS"
#define PASSWORD   "elecfreaks2013"


#include "uartWIFI.h"
#include <SoftwareSerial.h>
WIFI wifi;

extern int chlID;	//client id(0-4)


void setup()
{
  
  wifi.begin();
  bool b = wifi.Initialize(STA, SSID, PASSWORD);
  if(!b)
  {
    DebugSerial.println("Init error");
  }
  delay(8000);  //make sure the module can have enough time to get an IP address 
  String ipstring  = wifi.showIP();
  DebugSerial.println(ipstring);		//show the ip address of module
  
  delay(5000);
  wifi.confMux(1);
  delay(100);
  if(wifi.confServer(1,8080))
	DebugSerial.println("Server is set up");
	

}
void loop()
{
  
  char buf[100];
  int iLen = wifi.ReceiveMessage(buf);
  if(iLen > 0)
  {
    //if receive a "HELLO", send a "HELLO BACK" back to the client
    if (strcmp(buf, "HELLO") == 0)
    {
      DebugSerial.print("Get a message from id ");
      DebugSerial.print(chlID);
      DebugSerial.println(":");
      DebugSerial.println(buf); 
      
      DebugSerial.print("Send a message back to id ");
      DebugSerial.print(chlID);
      DebugSerial.println(":");
      DebugSerial.println("HELLO BACK");      
      wifi.Send(chlID,"HELLO BACK");
    }

  }
}

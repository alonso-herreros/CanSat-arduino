/*
In this project,we'll show how to get GPS data from a remote Arduino via Wireless LoRa Protocol
Hardware Set up
by Dragino Tech<support@dragino.com>
Dragino Technology Co., Limited
*/

#include <String.h>
#include <SPI.h>
#include <RH_RF95.h>
#ifdef LG01_GATEWAY
#include <Console.h>
#include <Process.h>
#define SerialPrint Console
#else
#define SerialPrint Serial
#endif

RH_RF95 rf95;

String str_humid;
String str_temp;
String str_out; // Define output strings

char gps_lon[20]="\0";           //Storage longtitude
char gps_lat[20]="\0";           //Storage latitude
char gps_alt[20]="\0";           //Storage altitude

//void receivepacket();    //Processing receive message and store it in the appropriate array

void setup() { 
  #ifdef LG01_GATEWAY
    Bridge.begin(9600);
    SerialPrint.begin();
  #else
    SerialPrint.begin(9600);
  #endif 
  
   if (!rf95.init()) {// Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  /* Set frequency is 868MHz,SF is 7,SB is 125KHz,CR is 4/5 and Tx power is 20dBm */
  rf95.setFrequency(868);
  rf95.setTxPower(13);
  
  Serial.println("Ready to receive!");
}
void loop(){
 receivepacket();
}

//Receiver LoRa packets and forward it 
void receivepacket() {
  if (rf95.available()){
    // received a packet
    int i =0,j=0,code[4];
    int m1=0,m2=0,m3=0;   
    uint8_t buf[50];
    char message[50]="\0";
    uint8_t len = sizeof(buf);
    // Check if received packet is correct size
    if (rf95.recv(buf, &len)){
    // Convert received data into string
      str_out = String((char*)buf);   
      // Split string into two values
      for (int h = 0; h < str_out.length(); h++) {
        if (str_out.substring(h, h+1) == ",") {
          str_humid = str_out.substring(0, h);
          str_temp = str_out.substring(h+1);
          break;
        }
      }
     
      delay(2000);
      Serial.println();

      Serial.print((char*)buf);
      uint8_t data[] = "Gateway receive GPS data";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
    }  
  }
}

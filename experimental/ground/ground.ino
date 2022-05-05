/*
Code for the ground unit
Created by Leopoldo Mosquera and Alonso Herreros.
Owned by DeltaRiver.
*/

#include <String.h>
#include <SPI.h>
#include <RH_RF95.h>

RH_RF95 rf95;

static uint8_t buf[64];
static uint8_t len = sizeof(buf);

//void receivepacket();    //Processing receive message and store it in the appropriate array

void setup() {
  Serial.begin(9600);
  
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
    for (int i=0; i<64; i++) 
      buf[i] = 0;
    //char message[50]="\0"; //unused
    if (rf95.recv(buf, &len)){ // Check if received packet is correct size
      delay(100); // ESTO estaba a 2000, lo he reducido, si no va súbelo a 500...

      Serial.println((char*)buf);
      uint8_t data[] = "Gateway receive GPS data";
      rf95.send(data, sizeof(data));
      rf95.waitPacketSent();
    }  
  }
}

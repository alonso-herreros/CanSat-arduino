/*
Code for the flight unit
Created by Leopoldo Mosquera and Alonso Herreros.
Owned by DeltaRiver.
*/


#include <DHT.h>
#define DHTPIN 5 // Use 5 pin to connect the data line of DHT22
#define DHTTYPE DHT22
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <String.h>

TinyGPS gps;
RH_RF95 rf95;
SoftwareSerial ss(4, 3); // Use those ports as RX & TX to conenct to GPS module
static void smartdelay(unsigned long ms); // We don't need this here, but it's not bad

String datastring1=""; // written to, but not used
String datastring2=""; // written to, but not used
String datastring3=""; // written to, but not used
uint8_t datasend[50];  // Storage  longtitude,latitude and altitude. written to, but not used

char gps_lon[50]="\0"; //Storage GPS info. unused
char gps_lat[20]="\0"; //Storage latitude. unused
char gps_alt[20]="\0"; //Storage altitude. unused

DHT dht(DHTPIN, DHTTYPE);

//String str_humid;
//String str_temp;
static float hum;    // Stores humidity value in percent
static float temp;   // Stores temperature value in Celsius
static String str_out; // Define output string

void setup(){
  dht.begin();
  // initialize both serial ports:
  Serial.begin(9600);  // Serial to print out GPS info in Arduino IDE
  ss.begin(9600);       // SoftSerial port to get GPS data. 
  while (!Serial);
    if (!rf95.init()) {  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    Serial.println("$[DR]TXW:Starting LoRa failed!;");
    while (1);
  } 
  /* Set frequency is 868MHz,SF is 7,SB is 125KHz,CR is 4/5 and Tx power is 20dBm */
  rf95.setFrequency(868);
  rf95.setTxPower(13);
  Serial.println("$[DR]TXW:Ready to send!;");
}

void loop(){
  delay(2000);
  hum = dht.readHumidity(); // %
  temp = dht.readTemperature(); // C
  //str_humid = String(hum);  // Convert Humidity to string  
  //str_temp = String(temp);  // Convert Temperature to string
  
  if (isnan(hum) || isnan(temp)) { // If we get a NAN value, we have a problem
    Serial.println("$[DR]WRN:Failed to read from DHT sensor!;");
    return; // [Alonso] I don't think we should return here, just move on...
  }
  else {
    str_out = "$[DR]HUM:" + String(hum, 4) + ",TEM:" + String(temp, 4) + ";\n";

    const char *msg = str_out.c_str(); // Convert to cstring (ntca)
    rf95.send((uint8_t *)msg, strlen(msg)); // Cast to uint8_t and send
    rf95.waitPacketSent();

    Serial.print(str_out); // Print the original string
  }


  /*uint8_t buf[50]; // unused
  Serial.print("$[DR]HUM:");
  Serial.print(hum);
  Serial.print(",TEM:");
  Serial.print(temp);
  Serial.println(";"); */
   
  bool newData = false; // written to, but unread
  unsigned long chars; // unused
  unsigned short sentences, failed; // unused
  
  float flat, flon, falt;
  unsigned long age; // written to, but not used
  gps.f_get_position(&flat, &flon, &age);
  falt=gps.f_altitude();  //get altitude
  flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6;//save six decimal places
  flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6;
  falt == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : falt, 2;//save two decimal places
  datastring1 += dtostrf(flat, 0, 6, gps_lat); // unused
  datastring2 += dtostrf(flon, 0, 6, gps_lon); // unused
  datastring3 += dtostrf(falt, 0, 2, gps_alt); // unused
  if(flon!=1000.000000){ // Maybe we don't need this check, but just in case we'll keep it
    //strcat(gps_lon,",");
    //strcat(gps_lon,gps_lat); 
    //strcat(gps_lon,","); 
    //strcat(gps_lon,gps_alt);
    //strcpy((char *)datasend,gps_lon);//the format of datasend is longtitude,latitude,altitude
    //Serial.println((char *)datasend);


    // send data
    //rf95.send(datasend, sizeof(datasend));

    str_out = "$[DR]LAT:" + String(flat, 6) + ",LON:" + String(flon, 6) + ",ALT:" + String(falt, 6) + ";\n";
    const char *msg2 = str_out.c_str(); // Compose output cstring

    Serial.print(str_out);
    rf95.send((uint8_t *)msg2, strlen(msg2));
    rf95.waitPacketSent();
    
    // Now wait for a reply
    receivepacket();
  }

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;){
    while (ss.available()){
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }
  // For 1 more second we parse GPS data, and check if there's a valid reading
  newData = gpsdelay(1000);
}

//If the packet arrive LG01, LG01 will send a ACK and here will receive it and turn on the led.  
void receivepacket(){
  uint8_t indatabuf[50];
  uint8_t len = sizeof(indatabuf);
  
  if (rf95.waitAvailableTimeout(3000)){ 
    // Should be a reply message for us now   
    if (rf95.recv(indatabuf, &len)){
      Serial.println((char*)indatabuf);        
    }
    else {
      Serial.println("receive failed!");
    }
  }
  else{
    Serial.println("No reply. Is ground module running or overranging the receive distance?");
  }
}


/*static void send(String msg) {
  char *s = msg.c_str();
  rf95.send((uint8_t *)s, strlen(s));
  rf95.waitPacketSent();
  Serial.println(msg);
}*/


bool gpsdelay(unsigned long ms){
  bool newData = false;

  // Save the current time and repeat this until "ms" milliseconds have passed
  for (unsigned long start = millis(); millis() - start < ms;) {
    while (ss.available()) {
      ss.print(Serial.read()); // [Alonso] I dont think this does anything...
      if ( gps.encode(ss.read()) ) // If there is a valid reading, report it
        newData = true;
    }
  }

  return newData;
}

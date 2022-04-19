#include <SD.h>
#include <Adafruit_BMP280.h>
#include <Wire.h>
#include <SPI.h>

#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BMP280 bmp; 

File myFile;
String Temperature, Pressure, Altitude, Data;
int paquete=0;

void setup() {
  Serial.begin(9600);
  delay(2000);
  // see if the sensor is present and can be initialized:
  bool status;
  while ( !Serial ) delay(100);   // wait for native usb
  Serial.println(F("BMP280 test"));
  //status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  status = bmp.begin(0x76);
  if (!status) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1) delay(10);
  }
// see if the card is present and can be initialized:
  Serial.print("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    myFile.println( "Temperature(°C), Pressure(hPa), Altitude(m) \r\n");
    myFile.close();
  } 
  else {
    Serial.println("error opening data.txt");
  }
  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loop() { 
  data_logging();                                                 
  delay(10000);

    Serial.print(paquete);
    Serial.print(",");
    
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.print("C");
    Serial.print(",");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.print("hPa");
    Serial.print(",");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.print("m");
    Serial.println();
    
    delay(1000);
    paquete++;
}

void data_logging() {
  String Temperature = String(bmp.readTemperature(), 2);
  String Pressure = String(bmp.readPressure()/ 100.0F, 2);
  String Altitude = String(bmp.readAltitude(SEALEVELPRESSURE_HPA), 2);
  
  Data = Temperature + "," + Pressure + "," + Altitude ;
  Serial.print("Save data: ");
  Serial.println(Data);

  myFile = SD.open("data.txt", FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to data.txt...");
    myFile.println(Data);
    myFile.close();
    Serial.println("done.");
  } else {
    Serial.println("error opening data.txt");
  }  
  Serial.println();
}

#include "eeprom.h"


structDato dato;

void setup()
{
  Serial.begin(9600);
  uint16_t total = 1024 / SIZE_STRUCT;
  for (uint16_t i = 0; i <= total; i++)
  {
    EEPROM.get(SIZE_STRUCT * i, dato);
    Serial.print(i);
    Serial.print(",");
    Serial.print(dato.temperatura);
    Serial.print(",");
    Serial.println(dato.altitud);
    
  }
}

void loop()
{

}

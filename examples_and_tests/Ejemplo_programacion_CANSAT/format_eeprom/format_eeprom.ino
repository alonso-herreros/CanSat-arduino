#include <EEPROM.h>
void setup()
{
  EEPROM.begin(1024);
  for (uint16_t i=0;i<1024;i++)
  {
    EEPROM.write(i,0);
  }
  EEPROM.commit();
 }

void loop() {
  // put your main code here, to run repeatedly:

}

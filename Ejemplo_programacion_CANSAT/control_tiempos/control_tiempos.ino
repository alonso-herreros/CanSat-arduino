#define runEvery(t) for (static uint32_t _lasttime;millis() - _lasttime >= (t);_lasttime += (t))


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}
long contador1=0; 
long contador2=0;

void loop() 
{
 runEvery(500)
 {
  //Se ejecuta cada 500 milisegundos
  contador1++;
 }
 runEvery(100)
 {
  //Se ejecuta cada 100 milisengundos
  contador2++;
 }
  runEvery(1000)
 {
  //Se muestra cada segundo.
  Serial.print(contador1);
  Serial.print('\t'); //tabulador
  Serial.println(contador2);
 }
}

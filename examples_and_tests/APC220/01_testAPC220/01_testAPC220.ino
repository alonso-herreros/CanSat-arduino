/*
   PROGRAMA DE PRUEBA: APC220
   CONEXION:
             RXD: Arduino Pin 1
             TXD: Arduino Pin 0
             GND: Arduino GND
             VCC: Arduino 5V
             
   Autor: Renato H.
   http://beetlecraft.blogspot.pe/
  
   El siguiente programa es de uso publico, cualquier modificacion o mal uso del mismo que pudiera 
   ocasionar el mal funcionamiento de la plataforma de uso de la misma no es responsabilidad del autor
*/

void setup(){
  Serial.begin(9600); // Velocidad de comunicacion
                      // La velocidad del puerto serial debe ser
                      // la misma que la de configuracion del modulo
}

void loop(){
 Serial.println("Hello World"); // Mensaje "Hola mundo"
 delay(1000);                   // Retraso de envio cada 1 segundo
}

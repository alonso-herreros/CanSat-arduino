#include "define.h"
#include "barometro.h"
#include "radio.h"
#include "eeprom.h"

structDato dato;


//declaracion variables
int paquete = 0;


void setup() {

  //inicializacion puerto serie// Dont put this on the stack:
  Serial.begin(9600);

  if (configurarRadio())
  {
    Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");

  }
  else
  {
    Serial.println("FALLO EN EL INICIO DE LA RADIO");
    //SERÍA CONVENIENTE ALGÚN LED QUE LO INDIQUE
  }
  if (configurarBarometro())
  {
    Serial.println("Barómetro ok");
  }
  else
  {
    Serial.println("Barómetro NOK");
    //SERÍA CONVENIENTE ALGÚN LED QUE LO INDIQUE
  }
}


//Tamaño del mensaje
/*
  #define RH_RF69_MAX_ENCRYPTABLE_PAYLOAD_LEN 64 //tamaño del mensaje codificable
  #define RH_RF69_HEADER_LEN 4 //tamaño de la cabecera
  #define RH_RF69_MAX_MESSAGE_LEN (RH_RF69_MAX_ENCRYPTABLE_PAYLOAD_LEN - RH_RF69_HEADER_LEN)
*/


uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
uint8_t data[] = "  OK";

void loop()
{

  //envio datos


  runEvery(1000)
  {
    //Construimos un mensaje de texto sumando los valores interpretados como un objeto String (NOTA: String NO ES IGUAL cadena de texto)
    String s = "data:";

    float temperatura = bmp.readTemperature();
    float presion = bmp.readPressure();
    /**
       Mirad la presión a nivel del mar en el momento del lanzamiento.
       o tomad la de lanzamiento como altura inicial.
    */
    float altitud = bmp.readAltitude(1013.25);
    //Creamos un mensaje que habará que convertir a cade de letras.
    //Se van retransmitiendo de una en una (a 9600bps = 9600 bits por segundo = 1200 bytes segundo= 1200 caracteres)
    s = s + paquete + ";" + temperatura + ";" + presion + ";" + altitud;

    char radiopacket[20];//Tenemos una cadena de texto

    sprintf(radiopacket, "%s", s.c_str()); //Metemos en la cadena de texto la parte que nos interesa del mensaje(tipo String)
    ///mensaje ---> 'm','e','n','s','a','j','e',\0'
    Serial.print("enviando ");
    Serial.print(radiopacket);
    /**
       Grabamos el dato
    */
    dato.temperatura= temperatura;
    dato.altitud=altitud;
    EEPROM.put(SIZE_STRUCT * paquete, dato);
    //Cada vez que se envia un dato se incrementa el contador de paquetes. 
    // y movemos la grabación la distancia del paqueta * tamaño 

    // Enviar el mensaje al DESTINO (DEST_ADDRESS)
    if (rf69_manager.sendtoWait((uint8_t *)radiopacket, strlen(radiopacket), DEST_ADDRESS))
    {
      // Podemos esperar un acuse de recibo
      uint8_t len = sizeof(buf);
      uint8_t from;

      if (rf69_manager.recvfromAckTimeout(buf, &len, 2000, &from))
      {
        buf[len] = 0; // zero out remaining string

        Serial.println(" OK");
      }
      else
      {
        Serial.println("No hay respuesta, ¿alquien nos escucha?");
      }
    }
    else
    {
      Serial.println(" Fallo");
    }
    paquete++;
  }

}

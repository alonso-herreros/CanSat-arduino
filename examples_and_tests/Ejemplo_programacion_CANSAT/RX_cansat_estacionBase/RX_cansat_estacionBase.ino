
#include <SPI.h>
#include <RH_RF69.h>
#include <RHReliableDatagram.h>

/************ Configuración de la radio ***************/

// Cambiar la frecuencia a la que vamos a trabajar, definida por el fabricante
#define RF69_FREQ 433.0 //868.0

// Mi dirección como estación base
#define MY_ADDRESS     1
//No tocar , depende de la placa KEYESESTUDIO
#define RFM69_INT     2  // 
#define RFM69_CS      10  //
#define RFM69_RST     3  // "A"
#define LED           13

// Declaramos nuestra radio a nivel software
RH_RF69 rf69(RFM69_CS, RFM69_INT);

/*
 * DATAGRAMA
 * EMISOR|RECEPTOR|MENSAJE|CÓDIGO DE VERIFICACIÓN
 * 
 * 
 */
// Gestión software de los mensajes
RHReliableDatagram rf69_manager(rf69, MY_ADDRESS);

int16_t packetnum = 0;  // Contador de mensajes 

void setup()
{
  Serial.begin(9600);
  
  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  Serial.println("Recepción en la estación base");
  Serial.println();

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!rf69_manager.init()) {
    Serial.println("Radio falla");
    while (1);
  }
  Serial.println("RFM69 radio init OK!");
  if (!rf69.setFrequency(RF69_FREQ))
  {
    Serial.println("setFrequency failed");
  }

  rf69.setTxPower(20);// rango de 14 a 20 para la potencia

  // Clave de cifrado de los mensajes, debe ser la misma en el emisor y el receptor
  uint8_t key[] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
                  };
  rf69.setEncryptionKey(key);

  pinMode(LED, OUTPUT);

  Serial.print("RFM69 radio @");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}


// Dont put this on the stack:
uint8_t data[] = "And hello back to you";
// Dont put this on the stack:
uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];
//vector[enteros]={'1','2','m','e','n','s','a','j','e',/0','7'}
void loop() 
{
  //Esperamos a recibir un mensaje
  if (rf69_manager.available())
  {
    /*
     * datagrama: EMISOR|RECEPTOR|MENSAJE|CÓDIGO VERIFICACIÓN
     */
    //Si se ha recibido algún mensaje
    uint8_t len = sizeof(buf); //nos da el tamaño del mensaje
    uint8_t from; //Nos indica desde donde, (podríamos tener varios satélites, o compartir estación base)
    if (rf69_manager.recvfromAck(buf, &len, &from)) 
    {
      buf[len] = 0; // Esto se llama cero terminador, necesario a nivel interno

      Serial.print("Got packet from #"); Serial.print(from);
      Serial.print(" [RSSI :");
      Serial.print(rf69.lastRssi());
      Serial.print("] : ");
      Serial.println((char*)buf);
      
      if (!rf69_manager.sendtoWait(data, sizeof(data), from))
      {
        Serial.println("Sending failed (no ack)");
      }
    }
  }
}

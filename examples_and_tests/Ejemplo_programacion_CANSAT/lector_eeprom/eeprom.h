
#include <EEPROM.h>



#define SIZE_STRUCT sizeof(structDato)
#define SIZE 1024

/*
 * struct nombre
 * {
 *  tipo_dato nombre_del_dato;
 *  ...
 *  ...
 *  ...
 *  };
 *  
 *  La estructura nos permite grabar datos por bloques
 *  En nuestro caso altitud, temperatura (a éste tipo de uniones se les llama tupla de datos) 
 *  A la hora de escribir en la eeprom iremos incrementado en el mismo tamaño 
 *  
 */


/*
 * La eeprom nos crea una "cinta" donde podremos escribir los datos unos detrás de otros, 
 * esté modo de escritura se llama secuencial. 
 * 
 * Para escribir empezaremos en la dirección '0' e iremos incrementado en el tamaño 
 * de nuestra estructura, en este caso 3 Bytes, pero lo hará automáticamente. 
 * No vamos a guardar datos decimales.  
 * Si usaramos datos gastriamos 8 bytes (solo tenemos 1024)
 * 
 * 1024 / 3 = 341 datos  
 * 1 dato por segundo = 5 minutos de grabación. 
 * 
 * 
 */
 
struct structDato
{
  int8_t temperatura; //permite valores desde -127 a 127 (son los 255 valores )
  uint16_t altitud;  //permite valore >255
};

/*
 * Esto se llama una macro. 
 * 
 * Lo que hace es ejecutar acciones cada cierto tiempo "t" en milisegundos
 * 
 */

#define runEvery(t) for (static uint32_t _lasttime;millis() - _lasttime >= (t);_lasttime += (t))

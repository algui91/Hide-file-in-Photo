/*
 * @file procesar.cpp
 * @brief Encargada de codificar y descodificar mensajes ocultos
 *  Created on: Mar 15, 2012
 *      @author: Alejandro Alcalde
 */
#include "../include/codificar.h"
#include <iostream>
#include <fstream>
#include <string.h>

using namespace std;


int ocultar(unsigned char buffer[],int tamImage, char sms[], int tamSms){

	unsigned short int indiceLetra		  = 0;
	char letra							  = sms[indiceLetra];
	short int bitsLetraRestantes 		  = 7;
	unsigned char mask					  = 0x80; //Empezamos por el bit más significativo (10000000)

	ifstream f(sms);
	if (f) {

cout << basename(sms);
		strcpy(sms,basename(sms));

		//Cabecera para identificar el nombre del archivo
		buffer[0] = 0xff;

		//Calculo el pixel donde tiene que terminar el nombre del archivo
		int fin_cabecera = strlen(sms)*8 + 1;
		buffer[fin_cabecera] = 0xff;

		//Escribo el nombre del archivo a ocultar
		for (int i = 1; i <= fin_cabecera - 1 ; i++ ){
			//TODO: hacerlo en una operacion lógica
			buffer[i] = buffer[i] >> 1; //Ponemos a 0 el ultimo bit haciendo dos desplazamientos
			buffer[i] = buffer[i] << 1;
			//TODO: Hacer con dos for
			if (bitsLetraRestantes < 0) {
				bitsLetraRestantes = 7;
				mask = 0x80;
				letra = sms[++indiceLetra];
			}
			char c = (letra & mask) >> bitsLetraRestantes--;
			mask >>= 1;
			buffer[i] ^= c; //Almacenamos en el ultimo bit del pixel el valor del caracter
		}

		bitsLetraRestantes 		  = 7;
		mask					  = 0x80;

		//TODO: (MEter en función) Calcular el tamaño en bytes del fichero
		f.seekg(0, std::ios_base::end);
		size_t size = f.tellg();
		f.seekg(0, std::ios_base::beg);
		int tamanio_en_bits = size*8;

		f.read(&letra, 1);
		// i empieza justo despues del fin de la cabecera del nombre
		// el for acaba cuando escribe todos los caracteres del fichero, hay que calcular
		// el indice sumando el desplazamiento que ya acarreamos + los bytes del archivo pasados a bits
		int datos_fichero = fin_cabecera + 1;
		for (int i = datos_fichero; i <= tamanio_en_bits + datos_fichero; i++){
			buffer[i] = buffer[i] >> 1; //Ponemos a 0 el ultimo bit haciendo dos desplazamientos
			buffer[i] = buffer[i] << 1;
			if (bitsLetraRestantes < 0) {
				bitsLetraRestantes = 7;
				mask = 0x80;
				f.read(&letra, 1);
			}
			char c = (letra & mask) >> bitsLetraRestantes--;
			mask >>= 1;
			buffer[i] ^= c; //Almacenamos en el ultimo bit del pixel el valor del caracter
		}
		//TODO: el fin de contenido del ficherco hay que almacenarlos como el resto de contenido
		bitsLetraRestantes 		  = 7;
		mask					  = 0x80;
		unsigned char fin_contenido = 0xff;
		for (int i = tamanio_en_bits + datos_fichero + 1;
				i < tamanio_en_bits + datos_fichero + 1 + 8; i++) {
			buffer[i] = buffer[i] >> 1; //Ponemos a 0 el ultimo bit haciendo dos desplazamientos
			buffer[i] = buffer[i] << 1;
			char c = (fin_contenido  & mask) >> bitsLetraRestantes--;
			mask >>= 1;
			buffer[i] ^= c; //Almacenamos en el ultimo bit del pixel el valor del caracter
		}
		cout << "FIN: " << tamanio_en_bits + datos_fichero + 1;


	/*
	unsigned short int indiceLetra		  = 0;
	char letra							  = sms[indiceLetra];
	short int bitsLetraRestantes 		  = 7;
	unsigned char mask					  = 0x80; //Empezamos por el bit más significativo (10000000)

	//Imagen demasiado pequeña para la cadena suministrada
	if (tamImage < tamSms)
		return -1;

	for (int i = 0; i <= tamImage && i < tamSms; i++){
		buffer[i] = buffer[i] >> 1; //Ponemos a 0 el ultimo bit haciendo dos desplazamientos
		buffer[i] = buffer[i] << 1;

		if (bitsLetraRestantes < 0){
			bitsLetraRestantes = 7;
			mask			   = 0x80;
			letra 			   = sms[++indiceLetra];
		}
		// Sacamos el bit de la letra en su posición y lo desplazamos a la pos menos significativa
		char c = (letra & mask) >> bitsLetraRestantes--;
		mask >>= 1;
		buffer[i] ^= c; //Almacenamos en el ultimo bit del pixel el valor del caracter
		//buffer[i] ^= 0xff; //Negativo e invertido
		//buffer[i] = ~buffer[i];
		//buffer[i] >>= 1; //Más oscura
		//buffer[i] <<=1; //Más clara
	}
*/
	}
	return 0;
}

//____________________________________________________________________________

int revelar(unsigned char buffer[], int tamImage, char sms[], int tamSMS){

	int indice		  		  = 0;
	char value 		  		  = 0;
	bool finCadena 	  		  = false;

	unsigned char* ptr;
	int in = 1;
	ptr = buffer;

	//Me posiciono en la pos siguiete del nombre del archivo, donde empieza el contenido del mismo.
	while(ptr[in++] != 0xff);
	ptr = 0;

	//TODO poner en funciones, getName, getRawData
	for (int i = 1; i <= in; i++) {
		value = value << 1 | (buffer[i] & 0x01); //vamos almacenando en value los 8 bits
		//Cuando recorramos 7 bits, lo almacenamos al array, almacenamos cada 8 iteraciones (1byte).
		//Para que el if sea mas rápido (Ya que es una op ||), pongo la condicion i == 7 al final, ya que solo se va a evaluar una vez
		if (((i - 8) % 8) == 0 || i == 8) {
			sms[indice++] = value;
			value = 0;
			if (indice > tamSMS)
				return -1; //cadena de mayor tamaño que que la cadena donde almacenarlo
		}
	}
	//Ahora en sms está el nombre del fichero, lo creamos:.
	cout << sms;
	ofstream f(sms);
	if(f){
		// TODO: getRawData
		//seguimos leyendo hasta que encontremos un byte a 0xff, que indica el fin del archivo
		bool fin_datos = false;
		int iteraciones_8 = 1;
		value = 0;
		for (int i = in; i < tamImage && !fin_datos; i++){
				value = value << 1 | (buffer[i]&0x01); //vamos almacenando en value los 8 bits
				//Cuando recorramos 7 bits, lo almacenamos al array, almacenamos cada 8 iteraciones (1byte).
				//Para que el if sea mas rápido (Ya que es una op ||), pongo la condicion i == 7 al final, ya que solo se va a evaluar una vez
				if ((iteraciones_8++) == 8){
					if(value == 0x7f){ //TODO revisar por qué no es 0xff
						fin_datos = true;
						continue;
					}
					f.write(&value,1);
					value = 0;
					iteraciones_8 = 1;
				}
			}

			if(!finCadena)
				return -2; //Si finCadena == false, no hemos encontrado caracter \0
	}


	//for (int i = 0; i < tamImage && /*sms[indice] != '\0'*/!finCadena; i++){
	/*	value = value << 1 | (buffer[i]&0x01); //vamos almacenando en value los 8 bits
		//Cuando recorramos 7 bits, lo almacenamos al array, almacenamos cada 8 iteraciones (1byte).
		//Para que el if sea mas rápido (Ya que es una op ||), pongo la condicion i == 7 al final, ya que solo se va a evaluar una vez
		if (((i-7)%8) == 0 || i == 7){
			sms[indice] = value;
			value = 0;
			if(sms[indice++] == '\0')
				finCadena = true;
			if(indice > tamSMS)
				return -1; //cadena de mayor tamaño que que la cadena donde almacenarlo
		}
	}

	if(!finCadena)
		return -2; //Si finCadena == false, no hemos encontrado caracter \0
	*/

	return 0;
}

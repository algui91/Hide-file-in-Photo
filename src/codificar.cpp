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

const bool WRITE_FILE_DATA = true;
const bool WRITE_FILE_NAME = false;

//Calcula el tamaño en bytes del fichero
int get_file_size(ifstream& f){
	f.seekg(0, std::ios_base::end);
	size_t size = f.tellg();
	f.seekg(0, std::ios_base::beg);

	return size;
}

int write_bit_by_bit(unsigned char buffer[], ifstream& f,int from, int to, char sms[], bool type){

	unsigned short int indiceLetra		  = 0;
	unsigned char mask					  = 0x80; //Empezamos por el bit más significativo (10000000)

	char* file_buffer = 0;

	if(type){ //Write file data
		int number_of_bytes_to_read = to;//get_file_size(f); //TODO, se lo paso como parametro, no calcular de nuevo
		file_buffer = new char[number_of_bytes_to_read];
		f.read(file_buffer, number_of_bytes_to_read);
	}

	const char* place_to_get_stuff_from = type ? file_buffer : sms;
	char letra = place_to_get_stuff_from[0];
	int indice = from;

	for (int i = 0; i < to; i++) { //TODO antes <=
		for (int k = 7; k >= 0; k--){
			char c = (letra & mask) >> k;
			mask >>= 1;

			buffer[indice] &= 0xfe; //hacemos 0 último bit con máscara 11111110
			buffer[indice++] ^= c;
		}
		letra = place_to_get_stuff_from[++indiceLetra];//letra = sms[++indiceLetra];
		mask = 0x80;
	}
	if (file_buffer) delete[] file_buffer;
		place_to_get_stuff_from = 0;

	return indice;
}


int ocultar(unsigned char buffer[],int tamImage, char sms[], int tamSms){

	ifstream f(sms);

	if (f) {

		strcpy(sms,basename(sms));

		//Cabecera que indica el comienzo del nombre del archivo
		buffer[0] = 0xff;

		//Calculo el pixel donde tiene que terminar el nombre del archivo
		int fin_cabecera = strlen(sms) * 8 +1;
		buffer[fin_cabecera] = 0xff;

		//Escribo el nombre del archivo a ocultar
		write_bit_by_bit(buffer, f, 1, strlen(sms), sms, WRITE_FILE_NAME);//TODO el 1

		int tamanio_en_bytes = get_file_size(f) /** 8*/;

		// i empieza justo despues del fin de la cabecera del nombre
		// el for acaba cuando escribe todos los caracteres del fichero, hay que calcular
		// el indice sumando el desplazamiento que ya acarreamos + los bytes del archivo pasados a bits
		int datos_fichero = fin_cabecera + 1;
		int ind = write_bit_by_bit(buffer, f, datos_fichero, tamanio_en_bytes/*tamanio_en_bytes + datos_fichero*/, sms, WRITE_FILE_DATA);

		//Escribo 0xff para indicar EOF de los datos
		unsigned char fin_contenido = 0x7f;

		short int bitsLetraRestantes 		  = 7;
		unsigned char mask					  = 0x80; //Empezamos por el bit más significativo (10000000)

		for (int i = ind;	i < ind+8; i++) {
			buffer[i] &= 0xfe;
			char c = (fin_contenido & mask) >> bitsLetraRestantes--;
			mask >>= 1;
			buffer[i] ^= c; //Almacenamos en el ultimo bit del pixel el valor del caracter
		}
		cout << "FIN: " << tamanio_en_bytes + datos_fichero + 1;
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

	//TODO poner en funciones, getName, getRawData O, read_bit_by_bit
	for (int i = 1; i <= in; i++) { //TODO, despercicio iteraciones
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
					f.write(&value,1); //TODO, ir almacenanto en array y luego escribir a archivo
					value = 0;
					iteraciones_8 = 1;
				}
			}

			if(!finCadena)
				return -2; //Si finCadena == false, no hemos encontrado caracter \0
	}

	return 0;
}

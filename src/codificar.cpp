/*
 * @file codificar.cpp
 * @brief Encargada de codificar y descodificar mensajes ocultos
 *  Created on: Mar 15, 2012
 *      @author: Alejandro Alcalde
 */
#include "../include/codificar.h"
#include <fstream>
#include <string.h>

using namespace std;

const bool WRITE_FROM_FILE = true;
const bool WRITE_FROM_ARRAY = false;

//TODO hacer que ifstream sea opcional
int write_bit_by_bit(unsigned char buffer[], ifstream& f, int from, int to, char sms[], bool type){

	unsigned short int indiceLetra		  = 0;
	unsigned char mask					  = 0x80; //Empezamos por el bit más significativo (10000000)

	char* file_buffer = 0;

	if(type){ //Write file data
		int number_of_bytes_to_read = to;//get_file_size(f);
		file_buffer = new char[number_of_bytes_to_read];
		f.read(file_buffer, number_of_bytes_to_read);
	}

	const char* place_to_get_stuff_from = type ? file_buffer : sms;
	char letra = place_to_get_stuff_from[0];
	int indice = from;

	for (int i = 0; i < to; i++) {
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


int ocultar(unsigned char buffer[],int tamImage, char archivo[]){

	ifstream f(archivo);

	if (f) {

		strcpy(archivo,basename(archivo));

		//Cabecera que indica el comienzo del nombre del archivo
		buffer[0] = 0xff;

		//Calculo el pixel donde tiene que terminar el nombre del archivo
		int fin_cabecera = strlen(archivo) * 8 +1;
		buffer[fin_cabecera] = 0xff;

		//Escribo el nombre del archivo a ocultar
		write_bit_by_bit(buffer, f, 1, strlen(archivo), archivo, WRITE_FROM_ARRAY);

		int tamanio_en_bytes = get_file_size(f) /** 8*/;

		// i empieza justo despues del fin de la cabecera del nombre
		// el for acaba cuando escribe todos los caracteres del fichero, hay que calcular
		// el indice sumando el desplazamiento que ya acarreamos + los bytes del archivo pasados a bits
		int datos_fichero = fin_cabecera + 1;
		int ind = write_bit_by_bit(buffer, f, datos_fichero, tamanio_en_bytes, archivo, WRITE_FROM_FILE);

		//Escribo 0xff para indicar EOF de los datos
		unsigned char eof = 0xff;
		unsigned char* fin_contenido = &eof;

		write_bit_by_bit(buffer, f, ind, 2, fin_contenido, WRITE_FROM_ARRAY);
	}
	return 0;
}

//____________________________________________________________________________

int revelar(unsigned char buffer[], int tamImage, char sms[], int tamSMS){

	int indice_sms	  		  = 0;
	char value 		  		  = 0;

	unsigned char* ptr;
	int in = 1;
	ptr = buffer;

	//Me posiciono en la pos siguiete del nombre del archivo, donde empieza el contenido del mismo.
	while(ptr[in++] != 0xff);
	ptr = 0;

	int i = 1;
	while (i != in-1){
		for (int k = 8; k > 0; k--)
			value = value << 1 | (buffer[i++] & 0x01); //vamos almacenando en value los 8 bits
		sms[indice_sms++] = value;
		value = 0;
		if (indice_sms > tamSMS)
			return -1; //cadena de mayor tamaño que que la cadena donde almacenarlo
	}

	//Ahora en sms está el nombre del fichero, lo creamos:.
	ofstream f(sms);
	if (f) {
		//seguimos leyendo hasta que encontremos un byte a 0x7f, que indica el fin del archivo
		char fin_datos = 0;
		int indice = in;
		value = 0;
		for (int i = in; i < tamImage && fin_datos != 2; i++) {
			for (int k = 0; k < 8; k++)
				value = value << 1 | (buffer[indice++] & 0x01); //vamos almacenando en value los 8 bits
			if (value == 0x7f) {
				fin_datos += 1;
				value = 0;
				continue;
			}
			f.write(&value, 1);
			//TODO, ir almacenanto en array y luego escribir a archivo
			value = 0;
		}
	}

	return 0;
}

//Calcula el tamaño en bytes del fichero
int get_file_size(ifstream& f){
	f.seekg(0, std::ios_base::end);
	size_t size = f.tellg();
	f.seekg(0, std::ios_base::beg);

	return size;
}

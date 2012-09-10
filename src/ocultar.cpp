/**
  * @file ocultar.cpp
  * @brief Fichero principal encargado de ocultar el mensaje
  *
  *	Created on: Mar 15, 2012
  *		@author: Alejandro Alcalde
  */
#include "../include/imagenES.h"
#include "../include/codificar.h"
#include <string.h>
#include <iostream>
//#include <istream>

using namespace std;

/**********************************************************************************
 * @brief Oculta un mensaje en una imagen de tipo pgm
 * @param inputImage[] nombre de la imagen en la que ocultar el mensaje
 * @param filas Filas de la imagen, es decir, la anchura
 * @param columnas Columnas de la imagen, es decir, la altura
 * @param outputImage[] Nombre que tendrá la imagen con el mensaje oculto
 * @param buffer[] Vector unidimensional con los valores de cada uno de los pixeles de la imagen
 * @param mensaje[] Cadena de texto a ocultar en la imagen.
 **********************************************************************************/
void oculta_archivo_en_PGM(char inputImage[], int& filas, int& columnas,
		char outputImage[], unsigned char buffer[], char archivo[]){

	int tamTotalImagen = filas*columnas;
		//tamTotalMensaje= (strlen(mensaje)+1)*8; //(strlen(mensaje)+1)*8) -> tamaño cadena +1 (Caracter \0), por 8bits de cada caracter

	if (LeerImagenPGM(inputImage, filas, columnas, buffer)) {
			switch (ocultar(buffer, tamTotalImagen, archivo)) {
				case 0:
					if (EscribirImagenPGM(strcat(outputImage,".pgm"), buffer, filas, columnas))
						cout << "Se ha ocultado correctamente el archivo " << archivo << " en " << outputImage << endl;
					else
						cout << "Error al escribir la imagen " << outputImage << endl;
					break;
				case -1:
					cout << "Imagen demasiado pequeña para el archivo, " << archivo << endl;
					break;
			}
		}
	else
		cout << "Error al leer la imagen." << endl;
}

/**********************************************************************************
 * @brief Oculta un mensaje en una imagen de tipo ppm
 * @param inputImage[] nombre de la imagen en la que ocultar el mensaje
 * @param filas Filas de la imagen, es decir, la anchura
 * @param columnas Columnas de la imagen, es decir, la altura
 * @param outputImage[] Nombre que tendrá la imagen con el mensaje oculto
 * @param buffer[] Vector unidimensional con los valores de cada uno de los pixeles de la imagen
 * @param mensaje[] Cadena de texto a ocultar en la imagen.
 **********************************************************************************/
void oculta_archivo_en_PPM(char inputImage[], int& filas, int& columnas,
		char outputImage[], unsigned char buffer[], char archivo[]){

	int tamTotalImagen = filas*columnas*3;		//(filas*columnas)*3 -> ppm usa 3 bytes por cada pixel,
		//tamTotalMensaje= (strlen(mensaje)+1)*8;

	if (LeerImagenPPM(inputImage, filas, columnas, buffer)) {
			switch (ocultar(buffer, tamTotalImagen, archivo)) {
				case 0:
					if (EscribirImagenPPM(strcat(outputImage,".ppm"), buffer, filas, columnas))
						cout << "Se ha ocultado correctamente el archivo " << archivo << " en " << outputImage << endl;
					else
						cout << "Error al escribir la imagen " << outputImage << endl;
					break;
				case -1:
					cout << "Imagen demasiado pequeña para el archivo " << archivo;
					break;
			}
		}
	else
		cout << "Error al leer la imagen." << endl;
}

int main(int argc, char* argv[]){
	const int MAX_FILE_NAME	 = 50;

	int filas, columnas;

	TipoImagen tipo = LeerTipoImagen(argv[1], filas, columnas);

	const int MAX_BUFFER = filas * columnas;
	unsigned char buffer[MAX_BUFFER];

	char archivo[MAX_FILE_NAME];
	cin.getline(archivo, 49, '\n');

	switch (tipo) {
		case IMG_PGM:
			cout << "Ocultando...";
			oculta_archivo_en_PGM(argv[1], filas, columnas, argv[2], buffer, archivo);
			break;
		case IMG_PPM:
			cout << "Ocultando...";
			oculta_archivo_en_PPM(argv[1], filas, columnas, argv[2], buffer, archivo);
			break;
		case IMG_DESCONOCIDO:
			cout << "Tipo de imagen desconocida" << endl;
			break;
		}
}

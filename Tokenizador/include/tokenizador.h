#ifndef _TOKENIZADOR_H_
#define _TOKENIZADOR_H_

#include <iostream>
#include <string>
#include <list>
//Fichero
#include <fstream>
//Directorio
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h> 
#include <cstdlib>

using namespace std;


class Tokenizador {
    // Aunque se modifique el almacenamiento de los delimitadores por temas de eficiencia, el campo delimiters se imprimirá con el string con el que se inicializó el tokenizador
    friend ostream& operator<<(ostream&, const Tokenizador&);

    private:
        //Delimitadores de términos
        //Aunque se modifique la forma de almacenamiento interna para mejorar la eficiencia
        //Debe permanecer para indicar el orden en que se introdujeron los delimitadores
        string delimiters;

        //TRUE --> detectará palabras compuestas y casos especiales
        //FALSE --> Trabaja como: "Versión del tokenizador vista en clase"
        bool casosEspeciales;
        
        //TRUE --> pasará el token a minúsculas y quitará acentos, ANTES de realizar tokenización
        bool pasarAminuscSinAcentos;

        //Función auxiliar para copiar Tokenizadores
        void Copiar(const Tokenizador&);

        //Función auxiliar que se encarga de leer un fichero y generar un fichero con la salida deseada
        bool GestionaFicheros (const string& NomFichEntr, const string& NomFichSal) const;

        //Lee el string de izquierda a derecha y elimina los duplicados de la derecha
        string FiltrarDelimitadores(const string&) const;

        //Transforma las mayúsculas a minúsculas y quita los acentos
        void Filtrar(string&) const;

        //Función auxiliar
        //TRUE si ninguno de sus char adyacentes son o delimitadores o espacios en blanco
        bool checkAdyacentes(const string& str, const string& delimiters, string::size_type& pos) const;

        //Función auxiliar para eliminar los delimitadores especiales de delimiters de cada caso
        string EliminaEspeciales(const string& delimiters, const string& especiales) const;

        //Heurística aplicada a Tokenizar una URL
        bool TokenizarURL(const string& str, const string& delimiters, string::size_type&, string::size_type&,list<string>& tokens) const;

        //Funciones que gestionan el caso especial Numero
        bool Numero(const string& str, string& numero, const string& delimiters, string::size_type& pos, string::size_type& lastPos, string::size_type& fin) const;
        bool TokenizarNumero(const string& str, const string& delimiters, string::size_type&, string::size_type&,list<string>& tokens) const;

        //Funciones que gestionan el caso especial Email
        bool Email(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos, const string &especiales, const string &email, const string::size_type &fin) const;
        bool TokenizarEmail(const string& str, const string& delimiters, string::size_type&, string::size_type&,list<string>& tokens) const;

        //Funciones que gestionan el caso especial Acronimo
        bool Acronimo(const string& str, const string& acronimo, string::size_type& lastPos, string::size_type& pos) const;
        bool TokenizarAcronimo(const string& str, const string& delimiters, string::size_type&, string::size_type&, list<string>& tokens) const;

        //Heurística aplicada a Tokenizar una multipalabra
        bool TokenizarMultipalabra(const string& str, const string& delimiters, string::size_type&, string::size_type&, list<string>& tokens) const;

    public:
        // Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha -> eliminar si repetidos por la derecha)
        //casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos
        Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos);

        //Constructor de Copia
        Tokenizador (const Tokenizador&);

        // Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@" | casosEspeciales a true; pasarAminuscSinAcentos a false
        Tokenizador ();

        // Pone delimiters=""
        ~Tokenizador (); 

        //Sobrecarga del operator=
        Tokenizador& operator= (const Tokenizador&);

        // Tokeniza str devolviendo el resultado en tokens. 
        //La lista tokens se vaciará antes de almacenar el resultado de la tokenización.
        void Tokenizar (const string& str, list<string>& tokens) const;

        // Tokeniza el fichero i guardando la salida en el fichero f (una palabra en cada línea del fichero)
        // TRUE si se realiza la tokenización de forma correcta.
        // FALSE en caso contrario enviando a cerr el mensaje correspondiente.
        bool Tokenizar (const string& NomFichEntr, const string& NomFichSal) const;

        //Tokeniza el fichero i guardando la salida en un fichero de nombre i + contendrá una palabra en cada línea del fichero.
        //añadiéndole extensión .tk (sin eliminar previamente la extensión de i por ejemplo, del archivo pp.txt se generaría el resultado en pp.txt.tk)
        //TRUE si se realiza la tokenización de forma correcta.
        //FALSE en caso contrario enviando a cerr el mensaje correspondiente.
        bool Tokenizar (const string & i) const;

        //Tokeniza el fichero i que contiene un nombre de fichero por línea guardando la salida en ficheros (uno por cada línea de i)
        //Cuyo nombre será el leído en i añadiéndole extensión .tk, y que contendrá una palabra en cada línea del fichero leído en i
        //TRUE si se realiza la tokenización de forma correcta de todos los archivos que contiene i
        //FALSE en caso contrario enviando a cerr el mensaje correspondiente. (pag 4)
        bool TokenizarListaFicheros (const string& i) const;

        //Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios
        //Cuyo nombre será el leído en i añadiéndole extensión .tk, y que contendrá una palabra en cada línea del fichero leído en i
        //TRUE si se realiza la tokenización de forma correcta de todos los archivos
        //FALSE en caso contrario envaindo a cerr el mensaje correspondiente. (pag 4)
        bool TokenizarDirectorio (const string& i) const;

        // Cambia “delimiters” por “nuevoDelimiters” comprobando que no hayan delimitadores repetidos (de izquierda a derecha)
        // Delimiters repetidos --> se eliminan los hayan sido repetidos (por la derecha)
        void DelimitadoresPalabra(const string& nuevoDelimiters);

        // Añade al final de “delimiters” los nuevos delimitadores que aparezcan en “nuevoDelimiters” (no se almacenarán caracteres repetidos)
        void AnyadirDelimitadoresPalabra(const string& nuevoDelimiters);
        
        // Devuelve “delimiters”
        string DelimitadoresPalabra() const;
        
        // Cambia la variable privada “casosEspeciales”
        void CasosEspeciales (const bool& nuevoCasosEspeciales);
        
        // Devuelve el contenido de la variable privada “casosEspeciales”
        bool CasosEspeciales () const;

        // Cambia la variable privada “pasarAminuscSinAcentos”. Atención al formato de codificación del corpus (comando “file” de Linux).
        // Para la corrección de la práctica se utilizará el formato actual (ISO-8859).
        void PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos);
        
        // Devuelve el contenido de la variable privada “pasarAminuscSinAcentos”
        bool PasarAminuscSinAcentos () const;
};

#endif
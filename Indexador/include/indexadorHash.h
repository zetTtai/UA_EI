#ifndef _INDEXADORHASH_H_
#define _INDEXADORHASH_H_

#include <iostream>
#include <list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <stdlib.h>

// Puntos h
#include "tokenizador.h"
#include "indexadorInformacion.h"
#include "stemmer.h"
// Exportar/Importar
#include <vector>
#include <sstream>
#include <iterator>
#include <fstream>
#include <limits>

using namespace std;

class IndexadorHash {
    friend ostream& operator<<(ostream& s, const IndexadorHash& p) {
        s << "Fichero con el listado de palabras de parada: " << p.ficheroStopWords << '\n';
        s << "Tokenizador: " << p.tok << '\n';
        s << "Directorio donde se almacenara el indice generado: "<< p.directorioIndice << '\n';
        s << "Stemmer utilizado: " << p.tipoStemmer << '\n';
        s << "Informacion de la coleccion indexada: " << p.informacionColeccionDocs << '\n';
        s << "Se almacenara parte del indice en disco duro: " << p.almacenarEnDisco << '\n';
        s << "Se almacenaran las posiciones de los terminos: " << p.almacenarPosTerm;
        return s;
    }
    // // Struct para almacenar datos de indiceDisco en en el fichero binario
    // struct BinInfTerm;
    // // Struct para almacenar datos de indiceDocsDisco en el fichero binario
    // struct BinInfDoc;
    private:
        // Este constructor se pone en la parte privada porque no se permitirá crear un indexador sin inicializarlo convenientemente.
        // La inicialización la decidirá el alumno
        IndexadorHash();

        // Índice de términos indexados accesible por el término
        unordered_map<string, InformacionTermino> indice;
        // Indice de términos indexados, cada término tiene asociada una linea de 0 a n 
        // La linea se encuentra siempre en el fichero "./indiceDisco.txt"
        unordered_map<string, long int> indiceDisco;

        // Índice de documentos indexados accesible por el nombre del documento
        unordered_map<string, InfDoc> indiceDocs;
        // Indice de documentos indexados, cada documento tiene asociada una linea de 0 a n 
        // La linea se encuentra siempre en el fichero "./indiceDocsDisco.txt"
        unordered_map<string, long int> indiceDocsDisco;

        // Información recogida de la colección de documentos indexada
        InfColeccionDocs informacionColeccionDocs;

        // Pregunta indexada actualmente. Si no hay ninguna indexada, contendría el valor “”
        string pregunta;

        // Índice de términos indexados en una pregunta. Se almacenará memoria principal en memoria principal
        unordered_map<string, InformacionTerminoPregunta> indicePregunta;

        // Información recogida de memoria principal en la pregunta indexada. Se almacenará memoria principal
        InformacionPregunta infPregunta;

        // Palabras de parada.
        // Se aplican tanto en preguntas como en documentos + parámetro minuscSinAcentos + tipoStemmer
        // Se aplica este proceso a las palabras de parada almacendadas en el doc ficheroStopWords se guarda la versión modificada, no la original del fichero
        unordered_set<string> stopWords;
        int numStopWords;
        // Esto se pide así para casos en los que en el documento/pregunta aparezca: “La casa de Él” y estén almacenadas como stopWords “la, el”, 
        // si se activa el parámetro minuscSinAcentos, entonces debería filtrar “La, Él”, si no hubiese estado activo ese parámetro, entonces no se hubiesen filtrado.

        // Nombre del fichero que contiene las palabras de parada
        string ficheroStopWords;

        // Tokenizador que se usará en la indexación. 
        // Se inicializará con los parámetros del constructor: detectComp y minuscSinAcentos, los cuales determinarán qué término se ha de indexar
        Tokenizador tok;
        // Si se activa minuscSinAcentos, entonces se guardarán los términos en minúsculas y sin acentos

        // “directorioIndice” será el directorio del disco duro donde se almacenará el índice. En caso que contenga la cadena vacía se creará en el directorio donde se ejecute el indexador
        string directorioIndice;

        // 0 = no se aplica stemmer: se indexa el término tal y como aparece tokenizado
        // Los siguientes valores harán que los términos a indexar se les aplique el stemmer y se almacene solo dicho stem.
        // 1 = stemmer de Porter para español
        // 2 = stemmer de Porter para inglés
        // Para el stemmer de Porter se utilizarán los archivos stemmer.cpp y stemmer.h, concretamente las funciones de nombre “stemmer”
        int tipoStemmer;

        // Para poder indexar colecciones de documentos tan grandes que su indexacion no quepa en memoria
        // TRUE -> Mientras se va generando el índice, se almacenará la mínima parte de los indices de los documentos en la memoria principal
        // FALSE -> Todo el indice se almacena en memoria principal
        bool almacenarEnDisco;
        // Se valorará el equilibrio para conseguir una indexación independientemente del tamaño del corpus a indexar, pero reduciendo el nº  de accesos a disco.
        // EJEMPLO: solo la estructura “indice” para saber las palabras indexadas.
        // guardando únicamente PUNTEROS a las posiciones de los archivos almacenados en disco que contendrán el resto de información

        // Si es TRUE se almacenará la posición en la que aparecen los términos dentro del documento en la clase InfTermDoc
        bool almacenarPosTerm;

        // Función auxiliar para copiar
        void Copiar(const IndexadorHash&);

        // Función auxiliar para almacenar las palabras del ficheroStopWords en StopWords
        void AlmacenarStopWords();

        // Función auxiliar para exportar indice, indiceDoc e indicePregunta
        void ExportarIndices() const;

        // Función auxiliar para importar los elementos del indexador(excepto stopWords e indices)
        void Importar(const string&);
        // Función auxiliar para importar indices desde una ruta
        void ImportarIndices(const string&);
        // Función auxiliar para importar stopWords indexadas desde una ruta
        void ImportarStopWords(const string&);

        // Función que importa los distintos elementos del indexador almacenado en directorioIndexacion
        void importarIndexador(const string&, const int&);

        // Función auxiliar para indexar un único fichero
        void IndexarFichero(const string&, InfDoc&, bool&);

        unordered_map<string,list<int>> ExtractInfo(string&, InfDoc&) const;

        // Función auxiliar para obtener informacion de los terminos y añadirlos al indice correspondiente
        // T -> InfTermDoc o InformacionTerminoPregunta
        // I -> infDoc o infPregunta
        // FALSE -> Si no hay memoria disponible para almacenar más términos
        bool AddTerms(const string&, const unordered_map<string, list<int>>&, InfDoc&);

        // Función auxiliar que se encarga de devolver el string asociado a linea de un fichero
        // En caso de no poder abrir el fichero se duvuelve un string vacio
        string GetInfo(const string&, const long int&) const;

        // Función auxiliar que se encarga de obtener la información necesaria para devolver un objeto de tipo InformacionTermino a partir de un string
        InformacionTermino GetInfTerm(const string& str, const string::size_type&) const;

        // Función auxiliar que se encarga de obtener la información necesaria para devolver un objeto de tipo InfDoc a partir de un string
        InfDoc GetInfDoc(const string& str) const;

        // Función auxiliar para añadir una nueva linea en el fichero donde se guarda la informacion de los InformacionTermino
        void AddInfTerm(const InformacionTermino& inf, const string& aux);

        // Función auxiliar que se encarga de modificar la linea corresponiente el long int pasado por parámetros
        // El tercer parámetro se trata de la ruta del fichero a modificar
        void ModifyLine(const long int&, const int&, const string&, const string&) const;

        double getcputime();

    public:
        // "fichStopWords" será el nombre del archivo que contendrá todas las palabras de parada (una palabra por cada línea del fichero) Guardar en FicheroStopWords
        // Asimismo, almacenará todas las palabras de parada que contenga el archivo en el campo privado “stopWords”, el índice de palabras de parada.
        // "delimitadores" será el string que contiene todos los delimitadores utilizados por el tokenizador + detectComp y minuscSinAcentos
        // "dirIndice" será el directorio del disco duro donde se almacenará elíndice (campo privado “directorioIndice”). Si dirIndice="" -> directorio raiz
        // “tStemmer” inicializará la variable privada "tipoStemmer" (0,1,2)
        // “almEnDisco” inicializará la variable privada “almacenarEnDisco”
        // “almPosTerm” inicializará la variable privada “almacenarPosTerm”
        // Los índices (p.ej. índice, indiceDocs e informacionColeccionDocs) quedarán vacíos
        IndexadorHash(const string& fichStopWords, const string& delimitadores, const bool& detectComp, const bool& minuscSinAcentos, 
                    const string& dirIndice, const int& tStemmer, const bool& almEnDisco, const bool& almPosTerm);

        // Constructor para inicializar IndexadorHash a partir de una indexación previamente realizada que habrá sido almacenada en “directorioIndexacion” mediante el método “bool GuardarIndexacion()”.
        // Con ello toda la parte privada se inicializará convenientemente, igual que si se acabase de indexar la colección de documentos. En caso que no
        // exista el directorio o que no contenga los datos de la indexación se tratará la excepción correspondiente
        IndexadorHash(const string& directorioIndexacion);
        ~IndexadorHash();
        IndexadorHash(const IndexadorHash&);
        IndexadorHash& operator= (const IndexadorHash&);

        // TRUE -> Cuando consigue crear el indice para la coleccion de documentos de ficheroDocumentos(FORMATO: nombre de documento por línea) parecido a .lista_fich
        // Añade los nuevos datos a los ya existentes
        // FALSE -> Cuando no finaliza la indexación (Falta de memoria) indicando el documento y término en el que se ha quedado, se mantienen los anteriormente indexado
        // Documentos repetidos, documentos que no existen, documentos ya indexados (mismo nombre y directorio) devuelve TRUE + exepción
        bool Indexar(const string& ficheroDocumentos);
        // REINDEXACIÓN
        // Comparar fechas, si el nuevo tiene una fecha de modificación más reciente que el ya almacenado
        //                  --> borrar anterior, añadir el nuevo + MANTENER ID del anteriormente indexado
        
        // TRUE -> Cuando consigue crear el índice para la colección de documentos que se encuentra en el directorio dirAIndexar (Se asume que TODOS son ficheros de texto)
        // Añade los nuevos datos a los ya existentes
        // FALSE -> Cuando no finaliza la indexación (Falta de memoria) indicando el documento y término en el que se ha quedado, se mantienen los anteriormente indexado
        // Documentos repetidos, documentos que no existen, documentos ya indexados (mismo nombre y directorio) devuelve TRUE + exepción
        bool IndexarDirectorio(const string& dirAIndexar);
        // REINDEXACIÓN
        // Comparar fechas, si el nuevo tiene una fecha de modificación más reciente que el ya almacenado
        //                  --> borrar anterior, añadir el nuevo + MANTENER ID del anteriormente indexado

        // Guarda TODOS los atributos privados de la clase actual en el directorio almacenado en la variable "directorioIndice" (como quieras)
        // Devuelve FALSO si no finaliza la operación (p.ej. por falta de memoria, o el nombre del directorio contenido en “directorioIndice” no es correcto) + vaciar ficheros generados
        bool GuardarIndexacion() const;
        // EJEMPLO: supongamos que se ejecuta esta secuencia de comandos: 
        //     IndexadorHash a(“./fichStopWords.txt”, “[,.”, “./dirIndexPrueba”, 0, false);
        //     a.Indexar(“./fichConDocsAIndexar.txt”);
        //     a.GuardarIndexacion();
        // entonces mediante el comando: 
        //     “IndexadorHash b(“./dirIndexPrueba”);” 
        // se recuperará la indexación realizada en la secuencia anterior, cargándola en “b”

        // Vaciar la indexación actual al principio
        // Acceder a "directorioIndexacion" e inicializar todas las variables privadas "igual que si se acabase de indexar la colección de documentos" //TODO preguntar
        // FALSO si no existe el directorio, lanzando excepción + dejando la indexación vacía.
        bool RecuperarIndexacion (const string& directorioIndexacion);

        void ImprimirIndexacion() const {
            cout << "Terminos indexados: " << endl;
            // A continuación aparecerá un listado del contenido del campo privado “índice” donde para cada término indexado se imprimirá: cout << termino << ‘\t’ << InformacionTermino << endl;
            cout << "Documentos indexados: " << endl;
            // A continuación aparecerá un listado del contenido del campo privado “indiceDocs” donde para cada documento indexado seimprimirá: cout << nomDoc << ‘\t’ << InfDoc << endl;
        }

        // Devuelve TRUE si consigue crear el índice para la pregunta “preg”.
        // Antes de realizar la indexación vaciará los campos privados indicePregunta e infPregunta
        // Generará la misma información que en la indexación de documentos, pero dejándola toda accesible en memoria principal (mediante las variables privadas “pregunta, indicePregunta, infPregunta”)
        // Devuelve FALSE si no finaliza la operación (p.ej. por falta de memoria o bien si la pregunta no contiene ningún término con contenido), mostrando el mensaje de error correspondiente
        bool IndexarPregunta(const string& preg);

        // Devuelve TRUE si hay una pregunta indexada (con al menos un término que no sea palabra de parada, o sea, que haya algún término indexado en indicePregunta), devolviéndola en “preg”
        bool DevuelvePregunta(string& preg) const;

        // Devuelve TRUE si word (aplicándole el tratamiento de stemming y mayúsculas correspondiente) está indexado en la pregunta, 
        // devolviendo su información almacenada “inf”. En caso que no esté, devolvería “inf” vacío
        bool DevuelvePregunta(const string& word, InformacionTerminoPregunta& inf) const;

        // Devuelve TRUE si hay una pregunta indexada, devolviendo su información almacenada (campo privado “infPregunta”) en “inf”. 
        // En caso que no esté, devolvería “inf” vacío
        bool DevuelvePregunta(InformacionPregunta& inf) const;


        void ImprimirIndexacionPregunta() const;

        void ImprimirPregunta() const {
            cout << "Pregunta indexada: " << pregunta << endl;
            cout << "Informacion de la pregunta: " << infPregunta << endl;
        }
        
        // Devuelve TRUE si word (aplicándole el tratamiento de stemming y
        // mayúsculas correspondiente) está indexado, devolviendo su información almacenada “inf”. En caso que no esté, devolvería “inf” vacío
        bool Devuelve(const string& word, InformacionTermino& inf) const;

        // Devuelve TRUE si word (aplicándole el tratamiento de stemming y mayúsculas correspondiente) está indexado y aparece en el documento de
        //nombre nomDoc, en cuyo caso devuelve la información almacenada para word en el documento. En caso que no esté, devolvería “InfDoc” vacío
        bool Devuelve(const string& word, const string& nomDoc, InfTermDoc& InfDoc) const;

        // Devuelve TRUE si word (aplicándole el tratamiento de stemming y mayúsculas correspondiente) aparece como término indexado
        bool Existe(const string& word) const;

        // Devuelve TRUE si se realiza el borrado (p.ej. si word, aplicándole el tratamiento de stemming y mayúsculas correspondiente, aparecía como término indexado)
        bool Borra(const string& word);

        // Devuelve TRUE si nomDoc está indexado y se realiza el borrado de todos los términos del documento y 
        // del documento en los campos privados “indiceDocs” e “informacionColeccionDocs”
        bool BorraDoc(const string& nomDoc);

        // Borra todos los términos del índice de documentos: toda la indexación de documentos.
        void VaciarIndiceDocs() {this->indiceDocs.clear(); this->informacionColeccionDocs.~InfColeccionDocs();}

        // Borra todos los términos del indexación índice de la pregunta: toda la indexación de la pregunta actual
        void VaciarIndicePreg() {this->indicePregunta.clear();}

        // Será TRUE si word (aplicándole el tratamiento de stemming y mayúsculas correspondiente) está indexado, sustituyendo la información almacenada por “inf”
        bool Actualiza(const string& word, const InformacionTermino& inf);

        // Será TRUE si se realiza la inserción (p.ej. si word, aplicándole el tratamiento de stemming y mayúsculas correspondiente, no estaba previamente indexado)
        bool Inserta(const string& word, const InformacionTermino& inf);


        //-------------------------------------------------------------------------------------------------------------------------------------------------
        // GETTERS Y SETTERS
        //-------------------------------------------------------------------------------------------------------------------------------------------------


        // Devolverá el número de términos diferentes indexados (cardinalidad de campo privado “índice”)
        int NumPalIndexadas() const
        {
            if(!this->almacenarEnDisco)
                return this->indice.size();
            else 
                return this->indiceDisco.size();
        }

        // Devuelve el contenido del campo privado “ficheroStopWords”
        string DevolverFichPalParada () const {return this->ficheroStopWords;}

        // Mostrará por pantalla las palabras de parada almacenadas (originales, sin aplicar stemming): 
        // una palabra por línea (salto de línea al final de cada palabra)
        void ListarPalParada() const;

        // Devolverá el número de palabras de parada almacenadas
        int NumPalParada() const {return this->numStopWords;}

        // Devuelve los delimitadores utilizados por el tokenizador
        string DevolverDelimitadores () const {return this->tok.DelimitadoresPalabra();}
        
        // Devuelve si el tokenizador analiza los casos especiales
        bool DevolverCasosEspeciales () const {return this->tok.CasosEspeciales();}
        
        // Devuelve si el tokenizador pasa a minúsculas y sin acentos
        bool DevolverPasarAminuscSinAcentos () const { return this->tok.PasarAminuscSinAcentos();}
        
        // Devuelve el valor de almacenarPosTerm
        bool DevolverAlmacenarPosTerm () const {return this->almacenarPosTerm;}

        // Devuelve “directorioIndice” (el directorio del disco duro donde se almacenará el índice)
        string DevolverDirIndice () const {return this->directorioIndice;}

        // Devolverá el tipo de stemming realizado en la indexación de acuerdo con el valor indicado en la variable privada “tipoStemmer”
        int DevolverTipoStemming () const {return this->tipoStemmer;}

        // Devolverá el valor indicado en la variable privada “almEnDisco”
        bool DevolverAlmEnDisco () const {return this->almacenarEnDisco;}

        // Mostrar por pantalla: cout << informacionColeccionDocs << endl;
        void ListarInfColeccDocs() const;

        // Mostrar por pantalla el contenido el contenido del campo privado “índice”: cout << termino << ‘\t’ << InformacionTermino << endl;
        void ListarTerminos() const;

        // Devuelve TRUE si nomDoc existe en la colección y muestra por pantalla todos los términos indexados del documento con nombre “nomDoc”: 
        // cout << termino << ‘\t’ << InformacionTermino << endl; . Si no existe no se muestra nada
        bool ListarTerminos(const string& nomDoc) const;

        // Mostrar por pantalla el contenido el contenido del campo privado “indiceDocs”: cout << nomDoc << ‘\t’ << InfDoc << endl;
        void ListarDocs() const;

        // Devuelve TRUE si nomDoc existe en la colección y muestra por pantalla el contenido del campo privado “indiceDocs” 
        // para el documento con nombre “nomDoc”: cout << nomDoc << ‘\t’ << InfDoc << endl; . Si no existe no se muestra nada
        bool ListarDocs(const string& nomDoc) const;
};

#endif
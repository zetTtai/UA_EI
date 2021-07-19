#ifndef _BUSCADOR_H_
#define _BUSCADOR_H_

#include "indexadorHash.h"
#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
// #include <string>
// #include <list>
// #include <cstring>
//Fichero
#include <fstream>
// //Directorio
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <sys/resource.h> 
// #include <cstdlib>

using namespace std;

class ResultadoRI {
    friend ostream& operator<<(ostream& os, const ResultadoRI& res)
    {
        os << res.vSimilitud << "\t\t" << res.idDoc << "\t" << res.numPregunta << endl;
        return os;
    }
    private:
        double vSimilitud;
        long int idDoc;
        int numPregunta;
    public:
        ResultadoRI(const double& kvSimilitud, const long int& kidDoc, const int& np)
        {
            vSimilitud = kvSimilitud;
            idDoc = kidDoc;
            numPregunta = np;
        }
        int getNumPregunta() const {return this->numPregunta;}
        double VSimilitud() const {return this->vSimilitud;}
        long int IdDoc() const {return this->idDoc;}
        bool operator< (const ResultadoRI& lhs) const
        {
            if(numPregunta == lhs.numPregunta)
                return (vSimilitud < lhs.vSimilitud);
            else
                return (numPregunta > lhs.numPregunta);
        }
};
// trec_eval -q -o frelevancia_trec_eval_TIME.txt fich_salida_buscador.txt > fich_salida_trec_eval.res”
class Buscador: public IndexadorHash {
    friend class IndexadorHash;
    friend ostream& operator<<(ostream& s, const Buscador& p) 
    {
        string preg;
        s << "Buscador: " << endl;
        if(p.DevuelvePregunta(preg))
            s << "\tPregunta indexada: " << preg << endl;
        else
            s << "\tNo hay ninguna pregunta indexada" << endl;
        // Invoca a la sobrecarga de la salida del Indexador
        s << "\tDatos del indexador: " << endl << (IndexadorHash) p;
        
        return s;
    }
    private:
        // Este constructor se pone en la parte privada porque no se permitirá
        // crear un buscador sin inicializarlo convenientemente a partir de una indexación.
        // Se inicializará con todos los campos vacíos y la variable privada
        // “formSimilitud” con valor 0 y las constantes de cada modelo: “c = 2; k1= 1.2; b = 0.75”
        Buscador();
        // Contendrá los resultados de la última búsqueda realizada en orden
        // decreciente según la relevancia sobre la pregunta. El tipo
        // “priority_queue” podrá modificarse por cuestiones de eficiencia. La
        // clase “ResultadoRI” aparece en la sección “Ejemplo de modo de uso de la cola de prioridad de STL”
        priority_queue<ResultadoRI> docsOrdenados;
        // vector<ResultadoRI> docsOrdenados;
        // 0: DFR, 1: BM25
        int formSimilitud;
        // Constante del modelo DFR
        double c;
        // Constante del modelo BM25
        double k1;
        // Constante del modelo BM25
        double b;
        // Función auxiliar para consguir el nombre de un documento sin extensión ni ruta
        string getDocName(const long int& id) const;
        // Función auxiliar que devuelve un string con lo que hay que imprimir por pantalla
        string Imprimir(const int&) const;
        // Función auxiliar usada para el caso DFR, devuelve el vSimilitud
        void DFR(const int&, const int&);
        // Función auxiliar usada para el caso BM25, devuelve el vSimilitud
        void BM25(const int&, const int&);

        // Función auxiliar usada para el caso DFR, devuelve wid
        double WeightDoc(InformacionTermino, InfDoc, const double &ft, const double &ld, const double& avg) const;
        // Función auxiliar usada para el caso BM25, devuelve IDF
        double IDF(const InformacionTermino&) const;
        // Función auxiliar usada para el caso BM25, devuelve la otra parte de la fórmula usada para BM25
        double FrecuencyDoc(const InformacionTermino&, const InfDoc&, const double& ft) const;
        // Función auxiliar usada en las funciones Buscar para no duplicar código
        bool GestionarBusqueda(const int&, const int&);

        double getcputime(void);
    public:
        // Constructor para inicializar Buscador creando un IndexadorHash a partir del directorioIndexacion
        // Si no existe directorio que contenga datos se enviará un cerr indicando el error, se continuará la ejecución con los índices vacios
        // formSimilitud= f && "c = 2; k1 = 1.2; b = 0.75;"
        Buscador(const string& directorioIndexacion, const int& f);
        Buscador(const Buscador&);
        ~Buscador();
        Buscador& operator= (const Buscador&);
        // En los métodos de “Buscar” solo se evaluarán TODOS los documentos que 
        // contengan alguno de los términos de la pregunta (tras eliminar las palabras de parada).

        // TRUE -> Si hay pregunta indexada (Comprobar IndexadorHash.pregunta) y sobre esa pregunta indexada
        //         finalizar la búsqueda correctamente con la formSimilutd indicada
        // "docsOrdenados" se vaciará previamente
        // FALSE -> Si no finaliza la búsqueda por falta de memoria; mostrando el error + documento y término en el que se ha quedado (Indexar)
        // Se guardarán los primeros "numDocumentos" documentos más relevantes en la variable "docsOrdenados" en orden decreciente según la relevancia
        // Se almacenarán los documentos que contengan algún termino de la query (aunque sea inferior a numDocumentos)
        // Como número de pregunta en ResultadoRI.numPregunta se almacenará el valor 0
        bool Buscar(const int& numDocumentos = 99999);

        // Realizará la búsqueda entre el número de pregunta "numPregInicio" y "numPregFin", ambas preguntas incluidas.
        // El corpus de preguntas estará en "dirPreguntas" y tendrá una estrcutura de cada pregunta en un fichero (1.txt 2.txt 3.txt ...)
        // Indexar cada pregunta por separaddo y ejecutar una búsqueda por cada pregunta añadiendo los resultados de cada pregunta junto a su número a la variable "docOrdenados"
        // Previamente se mantendrá la indexación del corpus
        // Se guardarán los primeros "numDocumentos" documentos más relevantes en la variable "docsOrdenados" en orden decreciente según la relevancia
        // Se almacenarán los documentos que contengan algún termino de la query (aunque sea inferior a numDocumentos)
        // FALSE -> Si no finaliza la búsqueda (por falta de memoria) mostrando un cerr
        bool Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin);

        // Imprime por pantalla los resultados de la última búsqueda (un numero maximo de "numDocumentos") por CADA pregunta
        // En el siguiente formato (una linea por cada documento):
        // NumPregunta FormulaSimilitud NomDocumento Posicion PuntuacionDoc
        // PreguntaIndexada
        // FormulaSimilitud -> O "DFR" o "BM25" si formSimilitud igual a 0 o 1
        // PuntuacionDoc -> "ResultadoRI.vSimilitud"
        // PreguntaIndexada corresponde al operator<< de IndexadorHash.pregunta
        // Se mostrará los decimales con punto en lugar de coma
        // Posicion empezaría desde 0 (indicando el documento más relevante para la pregunta) incrementándose por cada
        // Se imprimirá un máximo de líneas de “numDocumentos” (numDocumentos - 1)
        void ImprimirResultadoBusqueda(const int& numDocumentos = 99999) const;
        // Por ejemplo:
            // 1 BM25 EFE19950609-05926 0 64.7059 ConjuntoDePreguntas
            // 1 BM25 EFE19950614-08956 1 63.9759 ConjuntoDePreguntas
            // 1 BM25 EFE19950610-06424 2 62.6695 ConjuntoDePreguntas
            // 2 BM25 EFE19950610-00234 0 0.11656233535972 ConjuntoDePreguntas
            // 2 BM25 EFE19950610-06000 1 0.10667871616613 ConjuntoDePreguntas
        // Este archivo debería usarse con la utilidad “trec_eval -q -o frelevancia_trec_eval_TIME.txt fich_salida_buscador.txt > fich_salida_trec_eval.res”, 
        // para obtener los datos de precisión y cobertura

        // Lo mismo que “ImprimirResultadoBusqueda()” pero guardando la salida en el fichero de nombre “nombreFichero”
        // Devolverá false si no consigue crear correctamente el archivo
        bool ImprimirResultadoBusqueda(const int& numDocumentos, const string& nombreFichero) const;
        // Devuelve el valor del campo privado “formSimilitud”
        int DevolverFormulaSimilitud() const {return this->formSimilitud;}
        // Cambia el valor de “formSimilitud” a “f” si contiene un valor correcto (f == 0 || f == 1);
        // Devolverá false si “f” no contiene un valor correcto, en cuyo caso no cambiaría el valor anterior de “formSimilitud”
        bool CambiarFormulaSimilitud(const int& f)
        {
            if(f == 0 || f == 1)
                this->formSimilitud= f;
            else
                return false;
            return true;
        }
        // Cambia el valor de “c = kc”
        void CambiarParametrosDFR(const double& kc) {this->c= kc;}
        // Devuelve el valor de “c”
        double DevolverParametrosDFR() const {return c;}
        // Cambia el valor de “k1 = kk1; b = kb;”
        void CambiarParametrosBM25(const double& kk1, const double& kb) {this->k1= kk1; this->b= kb;}
        // Devuelve el valor de “k1” y “b”
        void DevolverParametrosBM25(double& kk1, double& kb) const {kk1= this->k1; kb= this->b;}
};

#endif
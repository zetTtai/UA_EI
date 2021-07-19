#ifndef _INDEXADORINFORMACION_H_
#define _INDEXADORINFORMACION_H_

#include <iostream>
#include <stdlib.h>
#include <list>
#include <string>
#include <unordered_map>
// Gestionar strings
#include <algorithm>
#include <vector>
#include <iterator>
#include <sstream>
// Fecha
#include <ctime>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

using namespace std;

class InfTermDoc {
    friend ostream& operator<<(ostream& s, const InfTermDoc& p) {
        s << "ft: " << p.ft;
        // A continuación se mostrarían todos los elementos de p.posTerm 
        //(“posicion TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB”): s <<“\t“ << posicion;
        for(auto& pos : p.posTerm)
            s << "\t" << pos;
        return s;
    }
    private:
        // Frecuencia del término en el documento
        int ft;
        // Solo se almacenará esta información si el campo privado del indexador si almacenarPosTerm == true
        // Lista de números de palabra en los que aparece el término en el documento.
        // Los números de palabra comenzarán desde cero (la primera palabra del documento). 
        // Se numerarán las palabras de parada. Estará ordenada de menor a mayor posición.
        vector<int> posTerm;
        void Copiar(const InfTermDoc&);
    public:
        // Inicializa ft = 0
        InfTermDoc ();
        InfTermDoc (const int&, const vector<int>&);
        InfTermDoc(const int& ft); //Inicializa posTerm como una lista vacia
        // Pone ft = 0
        ~InfTermDoc ();
        // Constructor de copia
        InfTermDoc (const InfTermDoc &);
        // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
        InfTermDoc& operator= (const InfTermDoc &);
        int getFt() const { return this->ft;}
        vector<int> getPosTerm() const { return this->posTerm;}
        void setFt(const int& ft) { this->ft= ft;}
        void setPosTerm(const vector<int>&);
};

class InformacionTermino {
    friend ostream& operator<<(ostream& s, const InformacionTermino& p) {
        s << "Frecuencia total: " << p.ftc << "\tfd: "<< p.l_docs.size();
        // A continuación se mostrarían todos los elementos de p.l_docs: s << “\tId.Doc: “ << idDoc << “\t” << InfTermDoc;
        for(auto& doc : p.l_docs)
            s << "\tId.Doc: " << doc.first << "\t" << doc.second;
        return s;
    }
    private:
        // Frecuencia total del término en la colección
        int ftc; 
        // Tabla Hash que se accederá por el id del documento, devolviendo un objeto de la clase InfTermDoc que contiene toda la información de
        //aparición del término en el documento
        unordered_map<long int, InfTermDoc> l_docs;
        // Función auxiliar para copiar
        void Copiar(const InformacionTermino&);
        
    public:
        // Inicializa ftc = 0
        InformacionTermino ();
        InformacionTermino (const int& ftc, const long int& id, const InfTermDoc& inf);
        // Pone ftc = 0 y vacía l_docs
        ~InformacionTermino ();
        // Constructor de copia
        InformacionTermino (const InformacionTermino &);
        // Añadir cuantos métodos se consideren necesarios para manejar la parte privada de la clase
        InformacionTermino & operator= (const InformacionTermino &);
        unordered_map<long int, InfTermDoc> getDocs() const { return this->l_docs;}
        int getFtc() const { return this->ftc;}
        void setFtc(const int& ftc) { this->ftc= ftc;}
        void addDoc(const long int& id, const InfTermDoc& inf) { this->l_docs.insert({id, inf});}
        void deleteDoc(const long int& id) { this->l_docs.erase(id);}
};

class Fecha {
    friend ostream& operator<<(ostream& s, const Fecha& f) {
        s << f.day << '/' << f.month << '/' << f.year << '\t' << f.hour;
        return s;
    }
    private:
        int day;
        int month;
        int year;
        string hour;
        bool moreRecentHour(const Fecha&) const;
    public:
        Fecha();
        Fecha (const string& str);
        Fecha (const struct tm *);
        Fecha(int day, int month, int year);
        ~Fecha();
        Fecha& operator= (const Fecha &);
        void setDay(int day);
        void setMonth(int month);
        void setYear(int year);
        int getDay(void) {return day;}
        int getMonth(void) { return month;}
        int getYear(void) { return year;}
        string getHour() { return hour;}
        // TRUE -> Si la esta fecha es más reciente que la pasada por argumentos
        bool moreRecentThan(const Fecha&) const;
};

class InfDoc {
    friend ostream& operator<<(ostream& s, const InfDoc& p) {
        s << "idDoc: " << p.idDoc << "\tnumPal: " << p.numPal << "\tnumPalSinParada: " << p.numPalSinParada << 
        "\tnumPalDiferentes: " << p.numPalDiferentes << "\ttamBytes: " << p.tamBytes;
        return s;
    }
    private:
        // Identificador del documento. El primer documento indexado en la colección será el identificador 1
        long int idDoc;
        // No total de palabras del documento
        int numPal;
        // No total de palabras sin stop-words del documento
        int numPalSinParada;
        // No total de palabras diferentes que no sean stop-words (sin acumular la frecuencia de cada una de ellas) // TODO respecto al indice?
        int numPalDiferentes;
        // Tamaño en bytes del documento
        int tamBytes;
        // Atributo correspondiente a la fecha y hora de modificación del documento. El tipo “Fecha/hora” lo elegirá/implementará el alumno
        Fecha fechaModificacion;
        void Copiar(const InfDoc&);
    public:
        InfDoc ();
        InfDoc (const long int& id, const int& numPal, const int& numPalSinParada, const int& numPalDiferentes, const int& tamBytes);
        InfDoc (const int& numPal, const int& numPalSinParada, const int& numPalDiferentes, const int& tamBytes);
        ~InfDoc ();
        InfDoc (const InfDoc &);
        InfDoc & operator= (const InfDoc &);
        void addPal(const int& numPal, const int& sinParada, const int& diferentes)
        {
            this->numPal= numPal; this->numPalSinParada= sinParada; this->numPalDiferentes= diferentes;
        }
        long int getId() const { return this->idDoc;}
        int getNumPal() const { return this->numPal;}
        int getNumPalSinParada() const { return this->numPalSinParada;}
        int getNumPalDiferentes() const { return this->numPalDiferentes;}
        int getTamBytes() const { return this->tamBytes;}
        Fecha getFecha() const { return this->fechaModificacion;}

        void setFecha(Fecha f) { this->fechaModificacion= f;}
        void setId(long int id) { this->idDoc= id;}
        void setId();
        void setTamBytes(int tam) { this->tamBytes= tam;}
};




class InfColeccionDocs {
    friend ostream& operator<<(ostream& s, const InfColeccionDocs& p) {
        s << "numDocs: " << p.numDocs << "\tnumTotalPal: " << p.numTotalPal <<
        "\tnumTotalPalSinParada: " << p.numTotalPalSinParada << 
        "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes << "\ttamBytes: " << p.tamBytes;
        return s;
    }
    private:
        // No total de documentos en la colección
        long int numDocs;
        // No total de palabras en la colección
        long int numTotalPal;
        // No total de palabras sin stop-words en la colección
        long int numTotalPalSinParada;
        // No total de palabras diferentes en la colección que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
        long int numTotalPalDiferentes;
        // Tamaño total en bytes de la colección
        long int tamBytes;
        //Función auxiliar para copiar
        void Copiar(const InfColeccionDocs&);
    public:
        InfColeccionDocs ();
        InfColeccionDocs (const long int&, const long int&, const long int&, const long int&, const long int&);
        ~InfColeccionDocs ();
        InfColeccionDocs (const InfColeccionDocs &);
        InfColeccionDocs & operator= (const InfColeccionDocs &);
        // Añade un documento a la coleccion de Docs
        void addDoc(const InfDoc&);
        // Elimina un documento de la coleccion de Docs
        void deleteDoc(const InfDoc&);
        //TODO Hacer deleteDoc(string Doc) Lee el documento y lo elimina de la colección
        int getNumDocs() const { return this->numDocs;}
        int getNumTotalPal() const { return this->numTotalPal;}
        int getNumTotalPalSinParada() const { return this->numTotalPalSinParada;}
        int getNumPalTotalDiferentes() const { return this->numTotalPalDiferentes;}
        int getTamBytes() const { return this->tamBytes;}
        void setDiferentes(const long int& diferentes) { this->numTotalPalDiferentes= diferentes;}
};




class InformacionTerminoPregunta {
    friend ostream& operator<<(ostream& s, const InformacionTerminoPregunta& p) {
        s << "ft: " << p.ft;
        // A continuación se mostrarían todos los elementos de p.posTerm (“posicion TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB”): s <<“\t“ << posicion;
        for(auto& pos : p.posTerm)
            s << "\t" << pos;
        return s;
    }
    private:
        // Frecuencia total del término en la pregunta
        int ft;
        // Solo se almacenará esta información si el campo privado del indexador almacenarPosTerm == true
        // Lista de números de palabra en los que aparece el término en la
        // pregunta. Los números de palabra comenzarán desde cero (la primera
        // palabra de la pregunta). Se numerarán las palabras de parada. Estará
        // ordenada de menor a mayor posición.
        vector<int> posTerm;
        void Copiar(const InformacionTerminoPregunta&);
        
    public:
        InformacionTerminoPregunta ();
        InformacionTerminoPregunta (const int&, const vector<int>&);
        InformacionTerminoPregunta (const int& ft) //Inicializa posTerm como una lista vacia
        {
            this->ft= ft;
            posTerm.clear();
        }
        ~InformacionTerminoPregunta ();
        InformacionTerminoPregunta (const InformacionTerminoPregunta &);
        InformacionTerminoPregunta & operator= (const InformacionTerminoPregunta&);
        int getFt() const { return this->ft;}
        vector<int> getPosTerm() const { return this->posTerm;}
        void setFt(const int& ft) { this->ft= ft;}
};



class InformacionPregunta {
    friend ostream& operator<<(ostream& s, const InformacionPregunta& p) {
        s << "numTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: "<< p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes;
        return s;
    }
    private:
        // No total de palabras en la pregunta
        long int numTotalPal;
        // No total de palabras sin stop-words en la pregunta
        long int numTotalPalSinParada;
        // No total de palabras diferentes en la pregunta que no sean stop-words (sin acumular la frecuencia de cada una de ellas)
        long int numTotalPalDiferentes;
        void Copiar(const InformacionPregunta&);
    public:
        InformacionPregunta ();
        InformacionPregunta (const long int&, const long int&, const long int&);
        ~InformacionPregunta ();
        InformacionPregunta (const InformacionPregunta &);
        InformacionPregunta & operator= (const InformacionPregunta &);
        long int getNumTotalPal() const { return this->numTotalPal;}
        long int getNumTotalPalSinParada() const { return this->numTotalPalSinParada;}
        long int getNumTotalPalDiferentes() const { return this->numTotalPalDiferentes;}
};

#endif
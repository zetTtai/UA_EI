#include <iostream>
#include <string>
#include <list>
#include <sys/resource.h>
#include "buscador.h"

using namespace std;

double getcputime(void) {
	struct timeval tim;
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	tim=ru.ru_utime;
	double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
	tim=ru.ru_stime;
	t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
	return t;
}
main() {
	
	// IndexadorHash(const string& fichStopWords, const string& delimitadores, const bool& detectComp, const bool& minuscSinAcentos, 
    //                 const string& dirIndice, const int& tStemmer, const bool& almEnDisco, const bool& almPosTerm);
	IndexadorHash b("./StopWordsIngles.txt", ",;:.-/+*\\ '\"{}[]()<>¡!?&#=\t\r@", false, true, "./indicePruebaEspanyol", 2, false, false);
	b.Indexar("ficherosTimes.txt");
	b.GuardarIndexacion();

	double aa=getcputime();
	Buscador a("./indicePruebaEspanyol", 0);

	a.Buscar("./CorpusTime/Preguntas/", 423, 1, 83);
	a.ImprimirResultadoBusqueda(423, "fich_salida_buscador_alumno.txt");
	double bb=getcputime()-aa;
	cout << "\nHa tardado " << bb << " segundos\n\n";
}
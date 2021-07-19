#include <iostream> 
#include <string> 
#include <list> 
#include <sys/resource.h> 
#include "tokenizador.h"

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
int
main() { 
        long double aa=getcputime(); 
        Tokenizador a("\t ,;:.-+/*_`'{}[]()!?&#\"\\<>", true, true); 
        list<string> tokens;
        a.TokenizarListaFicheros("listaFicheros.txt"); // TODO EL CORPUS 
        a.Tokenizar(" p=0,050. ", tokens);
        cout << "Ha tardado " << getcputime() - aa << " segundos" << endl; 
        return 0;
} 
// #include <iostream>
// #include <fstream>
// using namespace std;
// void openfile()
// {
//     ifstream inputfile;
//     string inputfilename= "corpus/aas-100-mg-comprimidos-20-comprimidos.txt";
//     int length;
//       char * buffer;
//       inputfile.open (inputfilename.c_str(), ios::binary );

//       get length of file:
//       inputfile.seekg (0, ios::end);
//       length = inputfile.tellg();
//       inputfile.seekg (0, ios::beg);

//       allocate memory:
//       buffer = new char [length];

//       read data as a block:
//       inputfile.read (buffer,length);

//       inputfile.close();

//       cout.write (buffer,length);
//       string aux= buffer;
//       cout << aux << endl;

//       delete[] buffer;
// }

// int main()
// {
//     openfile();
//     return 0;
// }
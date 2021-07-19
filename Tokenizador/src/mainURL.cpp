#include <iostream> 
#include <string>
#include <list> 
#include "tokenizador.h"

using namespace std;

///////// Comprobaci�n de que vac�e la lista resultado

void imprimirListaSTL(const list<string>& cadena)
{
        list<string>::const_iterator itCadena;
        for(itCadena=cadena.begin();itCadena!=cadena.end();itCadena++)
        {
                cout << (*itCadena) << ", ";
        }
        cout << endl;
}

int main(){
    Tokenizador a(",", true, false);
    list<string> tokens;
    string s = "p0 http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013&newspaper=catedraTelefonicaUA@iuii.ua.es p1 p2";

    a.Tokenizar(s, tokens);
    imprimirListaSTL(tokens);

    a.DelimitadoresPalabra("@");

    a.Tokenizar(s, tokens);
    imprimirListaSTL(tokens);
    
    a.DelimitadoresPalabra("/ ");

    a.Tokenizar(s, tokens);
    imprimirListaSTL(tokens);

    a.DelimitadoresPalabra("_:/.?&-=#@");

    a.Tokenizar(s, tokens);
    imprimirListaSTL(tokens);

    a.DelimitadoresPalabra("/&");

    s = "p0 hhttp://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013 p1 p2";

    a.Tokenizar(s, tokens);
    imprimirListaSTL(tokens);

    s = "p0 Http://intime.dlsi.ua.es:8080/dossierct/index.jsp?lang=es&status=probable&date=22-01-2013 p1 p2";
    
    a.Tokenizar(s, tokens);
    imprimirListaSTL(tokens);

    a.Tokenizar("http:////ab.", tokens);
    imprimirListaSTL(tokens);

    a.PasarAminuscSinAcentos(true);

    a.Tokenizar(s, tokens);
    imprimirListaSTL(tokens);

    return 0;
}
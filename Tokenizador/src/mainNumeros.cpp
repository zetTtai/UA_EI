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
    Tokenizador a("", true, false);
    list<string> lt1;
    a.DelimitadoresPalabra("@.,&");

    a.Tokenizar("pal1 10.000,34 10,000.34 10.000.123.456.789.009,34 10,000,123,456,789,009.34 20.03 40,03 2005 10. 20, 10.0 20,0 La 20,12.456,7.8.9,", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar(".34 ,56", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 10.35% 10,35% 23.000,3% 23,5$ 23.05$ 23,05$ 11.1$ 11.05$ 3.4% 4,3%", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 10.00a 10.000.a.000 10/12/85 1,23E+10", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1&10.00@10.000&abc@10/12/85", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar(".34@@&,56", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("...10.000.a.000 ,,23.05 10/12/85 1,23E+10", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("...10.000.000 ,,23.05 10/12/85 1,23E+10", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("3..2 4,,,,5 ..35", lt1);
    imprimirListaSTL(lt1);

    a.DelimitadoresPalabra("");

    a.Tokenizar("...10.000.a.000 ,,23.05 10/12/85 1,23E+10", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("3..2 4,,,,5 ..35", lt1);
    imprimirListaSTL(lt1);

    return 0;
}
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
    a.DelimitadoresPalabra("@.&");

    a.Tokenizar("U.S.A p1 e.g. p2. La", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("U..S.A p1 e..g. p2. La", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("U.S....A.BC.D ", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("...U.S.A p1 e..g. p2. La", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("...U.S.A... p1 e..g. p2. La", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("...U.S.A@p1 e..g-p2. La", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("Hack.4.Good p1 ", lt1);
    imprimirListaSTL(lt1);

    a.DelimitadoresPalabra("");

    a.Tokenizar("U.S.A .U.S.A .p1 p1 e.g. p2. La", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("U..S.A p1 e..g. p2. La", lt1);
    imprimirListaSTL(lt1);
    
    a.Tokenizar("...U.S.A p1 e..g. p2. La", lt1);
    imprimirListaSTL(lt1);
    
    a.Tokenizar("a&U.S.A p1 e.g. p2. La", lt1);
    imprimirListaSTL(lt1);
    
    a.DelimitadoresPalabra("&");

    a.Tokenizar("a&U.S.A p1 e.g. p2. La", lt1);
    imprimirListaSTL(lt1);
    
    return 0;
}
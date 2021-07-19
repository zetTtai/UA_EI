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

    a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("catedraTelefonicaUA@@iuii.ua.es p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 @iuii.ua.es p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 cat@iuii@cd.ua.es p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.DelimitadoresPalabra("&.");

    a.Tokenizar("catedraTelefonicaUA@iuii.ua.es p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 @iuii.ua.es p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1&@iuii.ua.es p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1&catedra@iuii.ua.es p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 cat@iuii.ua.es@cd p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.DelimitadoresPalabra("@.-_");

    a.Tokenizar("-catedraTelefonicaUA@iuii.ua.es @p1 p2 ", lt1);
    imprimirListaSTL(lt1);
    
    a.Tokenizar("@p2@@dot.com@p1-p2", lt1);
    imprimirListaSTL(lt1);
    
    a.Tokenizar("a@a- b@- c@c--c d@d-- e@-e f@--", lt1);
    imprimirListaSTL(lt1);
    
    return 0;
}
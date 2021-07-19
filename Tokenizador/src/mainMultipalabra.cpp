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
    a.DelimitadoresPalabra("-#");

    a.Tokenizar("MS-DOS p1 p2 UN-DOS-TRES", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 -MS-DOS p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 -MS-DOS- p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 MS-DOS--TRES p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 MS-DOS-TRES--- p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 MS-DOS#p3 p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1#MS-DOS#p3 p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1#MS- DOS#p3 p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1#MS -DOS#p3 p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.DelimitadoresPalabra("/ ");

    a.Tokenizar("MS-DOS p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 -MS-DOS p1 p2", lt1);
    imprimirListaSTL(lt1);
    
    a.Tokenizar("pal1 MS-DOS#p3 p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1#MS-DOS#p3 p1 p2",lt1);
    imprimirListaSTL(lt1);
    
    a.Tokenizar("pal1 -MS-DOS- p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 MS-DOS--TRES p1 p2", lt1);
    imprimirListaSTL(lt1);

    a.Tokenizar("pal1 MS-DOS-TRES--- p1 p2", lt1);
    imprimirListaSTL(lt1);
    
    return 0;
}
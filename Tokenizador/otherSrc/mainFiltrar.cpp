#include <iostream>
#include <string>
#include <list>
#include <sys/resource.h>
#include "tokenizador.h"

using namespace std;

double 
getcputime(void) {
        struct timeval tim;
        struct rusage ru;
        getrusage(RUSAGE_SELF, &ru);
        tim=ru.ru_utime;
        double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
        tim=ru.ru_stime;
        t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
        return t;
}
void imprimirListaSTL(const list<string>& cadena)
{
        list<string>::const_iterator itCadena;
        for(itCadena=cadena.begin();itCadena!=cadena.end();itCadena++)
        {
                cout << (*itCadena) << ", ";
        }
        cout << endl;
}

string removeDuplicate(const string &s) 
{ 
    //TODO Calcular complejidad
    string str= s;
    string aux= "";
    char c;
    while(str.size() != 0)
    {
        c= str.at(0);
        aux += c;
        string output;
        for(size_t i = 0; i < str.size(); ++i)
            if(str[i] != c) 
                output += str[i];
        str= output;
    }
    return aux;
} 

string removeDuplicate2(const string& s) 
{ 
    //TODO Calcular complejidad
    string str= s;
    string output;
    char c;
    string::size_type repetido;
    while(str.size() != 0)
    {
        c= str[0];
        output+= c;//Guardamos el char en la salida
        repetido = str.find_first_of(c, 0);
        while(repetido != string::npos)
        {
                str.erase(str.begin() + repetido);
                repetido = str.find_first_of(c, 0);
        }
    }
    return output;
} 

int main() {
        string delimiters= ",,a-;,:.:";
        string result;
        long double aa=getcputime();
        long double aux1, aux2;
        result= removeDuplicate(delimiters);
        aux1= getcputime() - aa;
        cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
        cout << result << endl;

        aa=getcputime();
        result= removeDuplicate2(delimiters);
        aux2= getcputime() - aa;
        cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
        cout << result << endl;

        if(aux1 > aux2)
                cout << "DOS" << endl;
        else
                cout << "UNO" << endl;    

        cout << "-------------" << endl;
        
        delimiters= "abcdefghi";
        aa=getcputime();
        result= removeDuplicate(delimiters);
        aux1= getcputime() - aa;
        cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
        cout << result << endl;

        aa=getcputime();
        result= removeDuplicate2(delimiters);
        aux2= getcputime() - aa;
        cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
        cout << result << endl;

        if(aux1 > aux2)
                cout << "DOS" << endl;
        else
                cout << "UNO" << endl;    
        
        cout << "-------------" << endl;
        
        delimiters= "aaaaaaaaaaa";
        aa=getcputime();
        result= removeDuplicate(delimiters);
        aux1= getcputime() - aa;
        cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
        cout << result << endl;

        aa=getcputime();
        result= removeDuplicate2(delimiters);
        aux2= getcputime() - aa;
        cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
        cout << result << endl;

        if(aux1 > aux2)
                cout << "DOS" << endl;
        else if(aux1 == aux2)
                cout << "what" << endl;
        else
                cout << "UNO" << endl;    

        cout << "-------------" << endl;
        
        delimiters= "aabbccddeeff";
        aa=getcputime();
        result= removeDuplicate(delimiters);
        aux1= getcputime() - aa;
        cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
        cout << result << endl;

        aa=getcputime();
        result= removeDuplicate2(delimiters);
        aux2= getcputime() - aa;
        cout << "Ha tardado " << getcputime() - aa << " segundos" << endl;
        cout << result << endl;

        if(aux1 > aux2)
                cout << "DOS" << endl;
        else
                cout << "UNO" << endl;  
        
        return 0;
}
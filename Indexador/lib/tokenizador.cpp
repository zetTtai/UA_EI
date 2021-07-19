#include "tokenizador.h"
#include <algorithm>

ostream& operator<<(ostream &os, const Tokenizador &tokenizador) 
{
    os << "DELIMITADORES: " << tokenizador.delimiters << " TRATA CASOS ESPECIALES: ";
    os << tokenizador.casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << tokenizador.pasarAminuscSinAcentos;
    return os;
}

//Funci?n auxiliar para copiar Tokenizadores
void Tokenizador::Copiar(const Tokenizador& tokenizador)
{
    this->delimiters= tokenizador.DelimitadoresPalabra();
    this->casosEspeciales= tokenizador.CasosEspeciales();
    this->pasarAminuscSinAcentos= tokenizador.PasarAminuscSinAcentos();
}

//Lee el string de izquierda a derecha y elimina los duplicados de la derecha
string
Tokenizador::FiltrarDelimitadores(const string& s) const
{
    //TODO Calcular complejidad
    string str= s;
    string output;
    char c;
    string::size_type repetido;
    while(!str.empty())
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
void
Tokenizador::Filtrar(char * cstr) const
{
    string str(cstr);
    Filtrar(str);
    cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
}
//Transforma las may?sculas a min?sculas y quita los acentos
void
Tokenizador::Filtrar(string& str) const
{
    char ISO[260]= "                                 !\"#$%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[ ]^_`abcdefghijklmnopqrstuvwxyz{|}~ ? ,                                                            ¿aaaaaa ceeeeiiii  ooooo  uuuuy  aaaaaa ceeeeiiii  ooooo  uuuuy y";
    ISO[241]= ISO[209]= 'ñ';
    for(int i= 0; i < str.size(); i++)
        str[i]= ISO[(unsigned char) str[i]];
}

//Funci?n auxiliar: TRUE si ninguno de sus char adyacentes son o delimitadores o espacios en blanco
bool
Tokenizador::checkAdyacentes(const string& str, const string& delimiters, string::size_type& pos) const
{
    try
    {
        if(delimiters.find(str.at(pos-1)) == string::npos && str.at(pos-1) != ' ')//Izq NO es un delimitador o espacio en blanco
            if(delimiters.find(str.at(pos+1)) == string::npos && str.at(pos+1) != ' ')//Der NO es un delimitador o espacio en blanco
                return true;
    } catch(exception e){ return false; }

    return false;
}

//Funci?n auxiliar para eliminar los delimitadores especiales de delimiters de cada caso
string 
Tokenizador::EliminaEspeciales(const string& delimiters, const string& especiales) const
{
    string delimitadores= delimiters;//delimitadores se usa para conservar solo los delimitadores que no ignora el caso especial
    string::size_type delimitador = delimitadores.find_first_of(especiales,0);//El primer delimitador que hay que eliminar
    while(string::npos != delimitador)
    {
        delimitadores.erase(delimitadores.begin() + delimitador);
        delimitador= delimitadores.find_first_of(especiales, delimitador);//Buscamos el siguiente
    }
    return delimitadores;
}

//Heur?stica aplicada a Tokenizar una URL
bool
Tokenizador::TokenizarURL(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos, list<string>& tokens, string& tok, const bool& fichero) const
{
    size_t dospuntos;
    try//Comprobamos que despu?s de los ':' no hay espacio en blanco
    {
        if(str.at(pos) != ':')
        {
            dospuntos= str.find_first_of(':', lastPos);
            if(dospuntos == string::npos)
                return false;
        }
        else
            dospuntos= pos;
        if(str.at(dospuntos + 1) == ' ' || str.at(dospuntos + 1) == '\n')//':' seguido de ' ' o salto de l?nea
            return false;
    } catch(exception e)
    {
        return false;
    }
    string protocolo= str.substr(lastPos, dospuntos - lastPos);//Guardamos el supuesto protocolo de la URL
    if(protocolo != "http")
        if(protocolo != "https")
            if(protocolo != "ftp")
                return false;
    string delimitadores= EliminaEspeciales(delimiters, "_:/.?&-=#@");//delimitadores se usa para conservar solo los delimitadores que no ignora el caso especial
    pos = str.find_first_of(delimitadores, lastPos);//lastPos Inicio de URL y pos est? en el primer delimitador que no ignora URL
    if(!fichero)
        tokens.push_back(str.substr(lastPos, pos - lastPos));
    else
        tok += str.substr(lastPos, pos - lastPos) + '\n';
    //Actualizamos lastPos y pos
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
    return true;
}

//Funci?n auxiliar: Devuelve true si se trata de un Numero
bool
Tokenizador::Numero(const string& str, string& numero, const string& delimiters, string::size_type& pos, string::size_type& lastPos, string::size_type& fin) const 
{
    string::size_type delimitador= numero.find_first_of(",.$%", 0);//numero es un string que NO contiene letras
    size_t aux;
    while(delimitador != string::npos)
    {
        switch(numero[delimitador])
        {
            case ',':
            case '.':
                aux= lastPos + delimitador;
                try {
                    if(delimiters.find(str.at(aux + 1)) != string::npos)//Seguido de delimitador
                    {
                        pos= aux;
                        return false;
                    }
                } catch (exception e){
                    pos= aux;
                    return false;
                }
                break;
            case '$':
            case '%':
                aux=  lastPos + delimitador;
                try {
                    if(delimiters.find(str.at(aux + 1)) == string::npos)//NO est? seguido por un delimiter
                        return false;
                    else
                    {
                        numero.pop_back();//Quitamos el (%,$) del token
                        fin -= 1;//Quitamos uno a fin para poder a?adir el (%,$) despu?s
                    }
                } catch (exception e){
                    pos= aux;
                    return false;
                }
                break;
            default:
                break;
        }
        if(delimiters.find(str[delimitador] == string::npos))
            delimitador= numero.find_first_of(".,$%", delimitador+1);
        else
        {
            pos= delimitador;
            return false;
        }
    }
    return true;
}

//Heur?stica aplicada a Tokenizar un Numero
bool
Tokenizador::TokenizarNumero(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos,list<string>& tokens, string& tok, const bool& fichero) const
{
    string especiales= ".,";
    string delimitadores= EliminaEspeciales(delimiters, especiales);//delimitadores se usa para conservar solo los delimitadores que no ignora el caso especial
    string::size_type fin = str.find_first_of(delimitadores, lastPos);
    string::size_type aux= lastPos;//lastPos original
    if(fin > str.size())
        fin= str.size();
    if(str[lastPos - 1] == '.' || str[lastPos - 1] == ',' )//Posible (.,) que se ha saltado
        lastPos -= 1;

    string numero= str.substr(lastPos, fin - lastPos);
    if(numero.find_first_not_of("0123456789$%.,", 0) == string::npos)//NO ha encontrado letras en el n?mero
    {
        if(Numero(str, numero, delimiters, pos, lastPos, fin))
        {
            if(numero.at(0) == '.' || numero.at(0) == ',')
                numero= "0" + numero;

            if(!fichero)
                tokens.push_back(numero);
            else
                tok += numero + '\n';
            //Actualizamos lastPos y pos
            lastPos = str.find_first_not_of(delimiters, fin);
            pos = str.find_first_of(delimiters, lastPos);
            return true;
        }
    }
    lastPos= aux;
    return false;
}

//Funci?n auxiliar para determinar si el string email cumple los requisitos 
bool
Tokenizador::Email(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos, const string &especiales, const string &email, const string::size_type &fin) const
{
    string::size_type aux= pos;//Guardamos la pos original
    bool arroba= false;
    bool especial= false;
    string::size_type delimitador= email.find_first_of(delimiters, email.find_first_of('@', 0) + 1);//Encontramos el primer delimitador despu?s de @
    //Derecha del @
    while(delimitador != string::npos)
    {
        if(especiales.find(email[delimitador]) != string::npos)//El delimitador encontrado est? entre los especiales
        {
            if(!checkAdyacentes(email, delimiters, delimitador))//False si Delimitador: ".-_" + rodeado de delimitadores
            {
                pos= lastPos + delimitador;//Actualizamos pos porque hemos encontrado un delimitador
                especial= true;
                if(str[pos-1] == '@')
                    pos -= 1;//En caso de que el delimitador est? adyacente derecha del primer arroba -> restamos 1 a pos porque ese @ es delimitador
                break;
            }
        }
        else
        {
            pos= aux;//Cambios pos para que mantenga su valor original
            return false;
        }
        if(delimiters.find(email[delimitador+1]) == string::npos)//Adyacente a delimitador
            delimitador= email.find_first_of(delimiters, delimitador+1);
        else
        {
            pos= aux;//Cambios pos para que mantenga su valor original
            return false;
        }
    }
    if(!especial)
        pos= fin;
    return true;//Es email
}

//Heur?stica aplicada a Tokenizar un Email
bool
Tokenizador::TokenizarEmail(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos,list<string>& tokens, string& tok, const bool& fichero) const
{
    string especiales= ".-_@";
    string delimitadores= EliminaEspeciales(delimiters, especiales);//delimitadores se usa para conservar solo los delimitadores que no ignora el caso especial
    string::size_type fin = str.find_first_of(delimitadores, lastPos);
    especiales.pop_back();
    //string::size_type arroba; //Posici?n del arroba, se usar? para dividir el e-mail en dos
    string email= str.substr(lastPos, fin - lastPos);
    if(Email(str, delimiters, pos, lastPos, especiales, email, fin))
    {
        if(!fichero)
            tokens.push_back(str.substr(lastPos, pos - lastPos));
        else
            tok += str.substr(lastPos, pos - lastPos) + '\n';
        //Actualizamos lastPos y pos
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
        return true;
    }
    return false;
}

//Funci?n auxiliar: Devuelve true si se trata de un Acr?nimo
bool
Tokenizador::Acronimo(const string& str, const string& acronimo, string::size_type& pos, string::size_type& lastPos) const
{
    string::size_type punto= acronimo.find_first_of('.',0);
    while(punto != string::npos)
    {
        size_t aux= lastPos + punto;
        try
        {
            if(str.at(aux - 1) == '.' || str.at(aux + 1) == '.' || str.at(aux + 1) == ' ')
            {
                pos= aux;
                return false;
            }
        }
        catch (exception e)
        {
            pos= aux;
            return false; 
        }
        punto= acronimo.find_first_of('.',punto + 1);
    }
    pos= lastPos + acronimo.size();
    return true;
}

//Heur?stica aplicada a Tokenizar un Acronimo
bool
Tokenizador::TokenizarAcronimo(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos,list<string>& tokens, string& tok, const bool& fichero) const
{
    string delimitadores= delimiters;//delimitadores se usa para conservar solo los delimitadores que no ignora el caso especial
    delimitadores.erase(delimitadores.begin() + delimitadores.find('.'));
    string::size_type fin= str.find_first_of(delimitadores, lastPos);
    if (fin > str.size())
        fin= str.size();
    string acronimo= str.substr(lastPos, fin - lastPos);
    if(acronimo != "")
        if(Acronimo(str,acronimo,pos,lastPos)) {
            pos= lastPos + acronimo.size();
            if(acronimo.at(acronimo.length() - 1) == '.')
                acronimo.pop_back();
            if(acronimo.at(0) == '.')
                acronimo.erase(0);

            if(!fichero)
                tokens.push_back(acronimo);
            else
                tok += acronimo  + '\n';
            //Actualizamos lastPos y pos
            lastPos = str.find_first_not_of(delimiters, pos);
            pos = str.find_first_of(delimiters, lastPos);
            return true;
        }
    return false;
}

//Heur?stica aplicada a Tokenizar una multipalabra
bool
Tokenizador::TokenizarMultipalabra(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos,list<string>& tokens, string& tok, const bool& fichero) const
{
    if(checkAdyacentes(str, delimiters, pos))
    {
        string delimitadores= delimiters;//delimitadores se usa para conservar solo los delimitadores que no ignora el caso especial
        delimitadores.erase(delimitadores.begin() + delimitadores.find('-'));//Eliminamos '-'
        //string::size_type aux= lastPos;
        pos= str.find_first_of(delimitadores, lastPos);//Final de la supuesta multipalabra
        if(pos > str.size())
            pos= str.size();//Evitamos violaci?n de segmento

        string multipalabra= str.substr(lastPos, pos - lastPos);
        string::size_type guion= multipalabra.find_first_of('-', lastPos);
        while(guion != string::npos)
        {
            if(!checkAdyacentes(multipalabra, delimiters, guion))
                break;
            guion= multipalabra.find_first_of('-', guion + 1);
        }

        if(lastPos > str.size())
            lastPos= str.size();//Evitamos violaci?n de segmento
        if(guion != string::npos)//Hemos encontrado un delimitador
            pos= guion + lastPos;
        if(!fichero)
            tokens.push_back(str.substr(lastPos, pos - lastPos));
        else
            tok += str.substr(lastPos, pos - lastPos) + '\n';
        //Actualizamos lastPos y pos
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
        return true;
    }
    return false;
}

// Inicializa delimiters a delimitadoresPalabra filtrando que no se introduzcan delimitadores repetidos (de izquierda a derecha -> eliminar si repetidos por la derecha)
// casosEspeciales a kcasosEspeciales; pasarAminuscSinAcentos a minuscSinAcentos
Tokenizador::Tokenizador (const string& delimitadoresPalabra, const bool& kcasosEspeciales, const bool& minuscSinAcentos)
{
    this->casosEspeciales= kcasosEspeciales;
    this->pasarAminuscSinAcentos= minuscSinAcentos;
    this->delimiters= FiltrarDelimitadores(delimitadoresPalabra);
}

// Constructor de Copia
Tokenizador::Tokenizador (const Tokenizador &tokenizador)
{
    Copiar(tokenizador);
}

bool
Tokenizador::GestionaFicheros (const string& NomFichEntr, const string& NomFichSal) const
{
    // TODO Mejorar eficiencia
    ifstream fEntrada("");
    ofstream fSalida("");
    string tokens;
    struct stat dir;
    int err= stat(NomFichEntr.c_str(), &dir);
    // Comprobamos la existencia del directorio
    if(err == -1 || !S_ISDIR(dir.st_mode)) 
    {
        fEntrada.open (NomFichEntr.c_str());
        if(fEntrada)
        {
            string aux;
            int length;
            char * buffer;
            //Obtenemos la longitud del fichero
            fEntrada.seekg(0, ios::end);
            length = fEntrada.tellg();
            fEntrada.seekg(0, ios::beg);
            // Reservamos memoria
            buffer= (char *) malloc(sizeof(char)*length);
            // Leemos los datos como si fueran un bloque, todos a la vez
            fEntrada.read(buffer,length);
            string cadena(buffer, length);
            free(buffer);
            fEntrada.close();

            if(!cadena.empty())//Guardar en string y no en list
            {
                Tokenizar(cadena, aux);
                tokens+= aux;
            }
        }
        else
        {
            cerr << "ERROR: No existe el archivo: " << NomFichEntr << '\n';
            return false;
        }
    }
    else
    {
        cerr << "ERROR: No es posible leer un directorio: " << NomFichEntr << '\n';
        return false;
    }

    if(NomFichSal != "")
        fSalida.open(NomFichEntr.c_str());
    else
        fSalida.open((NomFichEntr + ".tk").c_str());
    
    fSalida << tokens;

    fSalida.close();
    return true;
}

// Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>?!??&#=\t\n\r@" | casosEspeciales a true; pasarAminuscSinAcentos a false
Tokenizador::Tokenizador ()
{
    this->delimiters= ",;:.-/+*\\ '\"{}[]()<>?!??&#=\t\n\r@";
    this->casosEspeciales= true;
    this->pasarAminuscSinAcentos= false;
}

// Pone delimiters=""
Tokenizador::~Tokenizador ()
{
    this->delimiters= "";
}

Tokenizador& Tokenizador::operator= (const Tokenizador& tokenizador)
{
    if (this != &tokenizador)
    {
        (*this).~Tokenizador();//Destructor
        this->Copiar(tokenizador);
    }
    return *this;
}

// Tokeniza str devolviendo el resultado en tokens. 
// La lista tokens se vaciar? antes de almacenar el resultado de la tokenizaci?n.
void
Tokenizador::Tokenizar (const string& s, list<string>& tokens) const
{
    tokens.clear();
    string str= s;
    if(pasarAminuscSinAcentos)
        Filtrar(str);
    if(!casosEspeciales)
    {
        char * cstr= new char[str.length() + 1];
        strcpy(cstr,str.c_str());
        char* token = std::strtok(cstr, delimiters.c_str());
        while(token != 0) 
        {
            string aux= token;
            tokens.push_back(aux);
            token= strtok(NULL, delimiters.c_str());
        }
        delete[] cstr;
        // string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        // string::size_type pos = str.find_first_of(delimiters, lastPos);//El primer delimitador
        // while(string::npos != pos || string::npos != lastPos)
        // {
        //     tokens.push_back(str.substr(lastPos, pos - lastPos));
        //     lastPos = str.find_first_not_of(delimiters, pos);
        //     pos = str.find_first_of(delimiters, lastPos);
        // }
        
    }
    else
    {
        string especiales= "_:/.?&-=#@,";
        string delimitadores= this->delimiters;//Necesario para a?adir ' ' y \n en caso de que haga falta (no podemos modificar delimiters)

        if(delimiters.find(' ') == string::npos)//No est? ' '
            delimitadores += ' ';
        if(delimiters.find('\n') == string::npos)//No est? '\n'
            delimitadores += '\n';
        
        string::size_type lastPos = str.find_first_not_of(delimitadores, 0);
        string::size_type pos = str.find_first_of(delimitadores, lastPos);//El primer delimitado
        string tok;
        while(string::npos != pos || string::npos != lastPos)
        {
            // bool especial= true;//En caso de ser false se tokeniza como siempre
            // bool numero= true;//Booleana que controla el caso de que se salte un (.,) debido a como funciona lastPos y pos
            bool especial= true;//En caso de ser false se tokeniza como siempre
            bool numero= true;//Booleana que controla el caso de que se salte un (.,) debido a como funciona lastPos y pos

            if(pos != string::npos)
            {
                if(especiales.find(str.at(pos)) != string::npos && str[pos+1] != ' ' && str[pos+1] != '\n')//Si es adyacente a un espacio no se hace el switch
                    switch(str.at(pos))
                    {
                        case '.'://Caso m?s usado
                            if(!TokenizarNumero(str, delimitadores,pos, lastPos, tokens, tok, false))
                            {
                                if(!TokenizarAcronimo(str, delimitadores,pos, lastPos, tokens, tok, false))
                                    especial= false;
                            }
                            else
                                numero= false;
                            break;
                        case ',':
                            if(!TokenizarNumero(str, delimitadores, pos, lastPos, tokens, tok, false))
                                especial= false;
                            else
                                numero= false;
                            break;
                        case '-':
                            if(!TokenizarMultipalabra(str, delimitadores, pos, lastPos, tokens, tok, false))
                                especial= false;
                            break;
                        case '@'://Si el primer delimitador NO es @ entonces nunca puede ser Email
                            if(!TokenizarEmail(str, delimitadores, pos, lastPos, tokens, tok, false))
                                especial= false;
                            break;
                        default:
                            if(!TokenizarURL(str, delimitadores,pos, lastPos, tokens, tok, false))
                                especial= false;
                            break;
                    }
                else
                    especial= false;
            }
            else
                especial= false;
            if(numero & !especial)
                if(str[lastPos - 1] == '.' || str[lastPos - 1] == ',' )//Posible decimal que se ha saltado
                    if(TokenizarNumero(str, delimitadores,pos, lastPos, tokens, tok, false))
                        especial= true;
            if(!especial)
            {
                tokens.push_back(str.substr(lastPos, pos - lastPos));//Tokenizaci?n normal
                lastPos = str.find_first_not_of(delimitadores, pos);
                pos = str.find_first_of(delimitadores, lastPos);
            }
        }
    }
}

// En vez de guardar el Token enuna lista lo guarda directamente en un string
void
Tokenizador::Tokenizar (const string& s, string& tokens) const
{
    tokens= "";
    string str= s;
    if(pasarAminuscSinAcentos)
        Filtrar(str);
    if(!casosEspeciales)
    {
        char * cstr= new char[str.length() + 1];
        strcpy(cstr,str.c_str());
        char* token = std::strtok(cstr, delimiters.c_str());
        while(token != 0) 
        {
            string aux= token;
            tokens += aux + '\n';
            token= strtok(NULL, delimiters.c_str());
        }
        delete[] cstr;
        // string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        // string::size_type pos = str.find_first_of(delimiters, lastPos);//El primer delimitador
        // while(string::npos != pos || string::npos != lastPos)
        // {
        //     tokens += str.substr(lastPos, pos - lastPos) + '\n';
        //     lastPos = str.find_first_not_of(delimiters, pos);
        //     pos = str.find_first_of(delimiters, lastPos);
        // }
    }
    else
    {
        string especiales= "_:/.?&-=#@,";
        string delimitadores= this->delimiters;//Necesario para a?adir ' ' y \n en caso de que haga falta (no podemos modificar delimiters)

        if(delimiters.find(' ') == string::npos)//No est? ' '
            delimitadores += ' ';
        if(delimiters.find('\n') == string::npos)//No est? '\n'
            delimitadores += '\n';
        
        string::size_type lastPos = str.find_first_not_of(delimitadores, 0);
        string::size_type pos = str.find_first_of(delimitadores, lastPos);//El primer delimitado
        list<string> tok;
        while(string::npos != pos || string::npos != lastPos)
        {
            // bool especial= true;//En caso de ser false se tokeniza como siempre
            // bool numero= true;//Booleana que controla el caso de que se salte un (.,) debido a como funciona lastPos y pos
            bool especial= true;//En caso de ser false se tokeniza como siempre
            bool numero= true;//Booleana que controla el caso de que se salte un (.,) debido a como funciona lastPos y pos

            if(pos != string::npos)
            {
                if(especiales.find(str.at(pos)) != string::npos && str[pos+1] != ' ' && str[pos+1] != '\n')//Si es adyacente a un espacio no se hace el switch
                    switch(str.at(pos))
                    {
                        case '.'://Caso m?s usado
                            if(!TokenizarNumero(str, delimitadores,pos, lastPos, tok, tokens, true))
                            {
                                if(!TokenizarAcronimo(str, delimitadores,pos, lastPos, tok, tokens, true))
                                    especial= false;
                            }
                            else
                                numero= false;
                            break;
                        case ',':
                            if(!TokenizarNumero(str, delimitadores, pos, lastPos, tok, tokens, true))
                                especial= false;
                            else
                                numero= false;
                            break;
                        case '-':
                            if(!TokenizarMultipalabra(str, delimitadores, pos, lastPos, tok, tokens, true))
                                especial= false;
                            break;
                        case '@'://Si el primer delimitador NO es @ entonces nunca puede ser Email
                            if(!TokenizarEmail(str, delimitadores, pos, lastPos, tok, tokens, true))
                                especial= false;
                            break;
                        default:
                            if(!TokenizarURL(str, delimitadores,pos, lastPos, tok, tokens, true))
                                especial= false;
                            break;
                    }
                else
                    especial= false;
            }
            else
                especial= false;
            if(numero & !especial)
                if(str[lastPos - 1] == '.' || str[lastPos - 1] == ',' )//Posible decimal que se ha saltado
                    if(TokenizarNumero(str, delimitadores,pos, lastPos, tok, tokens, true))
                        especial= true;
            if(!especial)
            {
                tokens += str.substr(lastPos, pos - lastPos) + '\n';//Tokenizaci?n normal
                lastPos = str.find_first_not_of(delimitadores, pos);
                pos = str.find_first_of(delimitadores, lastPos);
            }
        }
    }
}

// Tokeniza el fichero i guardando la salida en un fichero de nombre i + contendrï¿½ una palabra en cada lï¿½nea del fichero.
// a?adi?ndole extensi?n .tk (sin eliminar previamente la extensi?n de i por ejemplo, del archivo pp.txt se generar?a el resultado en pp.txt.tk)
// TRUE si se realiza la tokenizaci?n de forma correcta.
// FALSE en caso contrario enviando a cerr el mensaje correspondiente.
bool
Tokenizador::Tokenizar (const string& NomFichEntr, const string& NomFichSal) const
{
    return GestionaFicheros(NomFichEntr, NomFichSal);
}
bool Tokenizador::Tokenizar (const string & NomFichEntr) const
{
    return GestionaFicheros(NomFichEntr, "");
}

// Tokeniza el fichero i que contiene un nombre de fichero por l?nea guardando la salida en ficheros (uno por cada l??nea de i)
// Cuyo nombre ser? el le?do en i a?adi?ndole extensi?n .tk, y que contendr? una palabra en cada l?nea del fichero le?do en i
// TRUE si se realiza la tokenizaci?n de forma correcta de todos los archivos que contiene i
// FALSE en caso contrario enviando a cerr el mensaje correspondiente. (pag 4)
bool
Tokenizador::TokenizarListaFicheros (const string& lista) const
{
    ifstream lista_fich("");
    lista_fich.open(lista.c_str());
    if(lista_fich)
    {
        string ruta;
        while(getline(lista_fich, ruta))
        {
            if(!ruta.empty())
                GestionaFicheros(ruta, "");
        }
        lista_fich.close();
    }
    else
    {
        cerr << "ERROR: No existe el archivo: " << lista << '\n';
        return false;
    }
    return true;
}

// Tokeniza todos los archivos que contenga el directorio i, incluyendo los de los subdirectorios
// Cuyo nombre ser? el le??do en i a?adi?ndole extensi?n .tk, y que contendr? una palabra en cada l??nea del fichero le??do en i
// TRUE si se realiza la tokenizaci?n de forma correcta de todos los archivos
// FALSE en caso contrario enviando a cerr el mensaje correspondiente. (pag 4)
bool
Tokenizador::TokenizarDirectorio (const string& dirAIndexar) const
{
    // TODO Mejorar eficiencia
    struct stat dir;
    int err= stat(dirAIndexar.c_str(), &dir);
    // Comprobamos la existencia del directorio
    if(err == -1 || !S_ISDIR(dir.st_mode)) 
    {
        cerr << "ERROR: No existe el directorio: " << dirAIndexar << '\n';
        return false;
    }
    else
    {
        // Hago una lista en un fichero con find>fich
        string cmd="find ";
        cmd += dirAIndexar;
        cmd += " -type f -follow |sort > .lista_fich";
        system(cmd.c_str());
        return TokenizarListaFicheros(".lista_fich");
    }
}

// Cambia delimeters por nuevoDelimiters comprobando que no hayan delimitadores repetidos (de izquierda a derecha)
// Delimiters repetidos --> se eliminan los hayan sido repetidos (por la derecha)
void
Tokenizador::DelimitadoresPalabra(const string& nuevoDelimiters)
{
    if(nuevoDelimiters.size() > 1)
        this->delimiters= FiltrarDelimitadores(nuevoDelimiters);
    else
        this->delimiters= nuevoDelimiters;
}

// A?ade al final de ?delimiters? los nuevos delimitadores que aparezcan en ?nuevoDelimiters? (no se almacenar?n caracteres repetidos)
void
Tokenizador::AnyadirDelimitadoresPalabra(const string& nuevoDelimiters)
{
    this->delimiters= FiltrarDelimitadores(this->delimiters + nuevoDelimiters);
}

// Devuelve "delimiters"
string 
Tokenizador::DelimitadoresPalabra() const
{
    return this->delimiters;
}

// Cambia la variable privada "casosEspeciales"
void 
Tokenizador::CasosEspeciales (const bool& nuevoCasosEspeciales)
{
    this->casosEspeciales= nuevoCasosEspeciales;
}

// Devuelve el contenido de la variable privada "casosEspeciales"
bool
Tokenizador::CasosEspeciales () const
{
    return casosEspeciales;
}

// Cambia la variable privada ?pasarAminuscSinAcentos?. Atenci?n al formato de codificaci?n del corpus (comando file de Linux).
// Para la correcci?n de la pr?ctica se utilizar? el formato actual (ISO-8859).
void
Tokenizador::PasarAminuscSinAcentos (const bool& nuevoPasarAminuscSinAcentos)
{
    this->pasarAminuscSinAcentos= nuevoPasarAminuscSinAcentos;
}

// Devuelve el contenido de la variable privada ?pasarAminuscSinAcentos?
bool
Tokenizador::PasarAminuscSinAcentos () const
{
    return pasarAminuscSinAcentos;
}
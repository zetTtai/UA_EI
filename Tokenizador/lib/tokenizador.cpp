#include "tokenizador.h"

ostream& operator<<(ostream &os, const Tokenizador &tokenizador) 
{
    os << "DELIMITADORES: " << tokenizador.delimiters << " TRATA CASOS ESPECIALES: ";
    os << tokenizador.casosEspeciales << " PASAR A MINUSCULAS Y SIN ACENTOS: " << tokenizador.pasarAminuscSinAcentos;
    return os;
}

//Función auxiliar para copiar Tokenizadores
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

//Transforma las mayúsculas a minúsculas y quita los acentos
void
Tokenizador::Filtrar(string& str) const
{
    string mayus= "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    string acentos="ÀàÁáÈèÉéÌìÍíÒòÓóÙùÚúÑ";//Todos los acentos incluyendo Ñ

    string::size_type mayuscula= str.find_first_of(mayus,0);
    string::size_type acento= str.find_first_of(acentos,0);

    while(mayuscula != string::npos || acento != string::npos )// Acaba cuando no encuentra ni mayúsculas ni acentos
    {
        if(mayuscula != string::npos)// Encontramos una mayúscula
        {
            str[mayuscula] += 32;// Pasamos a minúscula (sin acento)
            mayuscula= str.find_first_of(mayus, mayuscula);// Buscamos la siguiente
        }
        if(acento != string::npos)
        {
            switch(str[acento])
            {
                case 'À':
                case 'à':
                case 'Á':
                case 'á':
                    str[acento] = 'a';
                    break;
                case 'È':
                case 'è':
                case 'É':
                case 'é':
                    str[acento] = 'e';
                    break;
                case 'Ì':
                case 'ì':
                case 'Í':
                case 'í':
                    str[acento] = 'i';
                    break;
                case 'Ò':
                case 'ò':
                case 'Ó':
                case 'ó':
                    str[acento] = 'o';
                    break;
                case 'Ù':
                case 'ù':
                case 'Ú':
                case 'ú':
                    str[acento] = 'u';
                    break;
                case 'Ñ':
                    str[acento] = 'ñ';
                default:
                    break;
            }
            acento= str.find_first_of(acentos, acento); //Buscamos el siguiente
        }
    }
}

//Función auxiliar: TRUE si ninguno de sus char adyacentes son o delimitadores o espacios en blanco
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

//Función auxiliar para eliminar los delimitadores especiales de delimiters de cada caso
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

//Heurística aplicada a Tokenizar una URL
bool
Tokenizador::TokenizarURL(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos, list<string>& tokens) const
{
    size_t dospuntos;
    try//Comprobamos que después de los ':' no hay espacio en blanco
    {
        if(str.at(pos) != ':')
        {
            dospuntos= str.find_first_of(':', lastPos);
            if(dospuntos == string::npos)
                return false;
        }
        else
            dospuntos= pos;
        if(str.at(dospuntos + 1) == ' ' || str.at(dospuntos + 1) == '\n')//':' seguido de ' ' o salto de línea
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
    pos = str.find_first_of(delimitadores, lastPos);//lastPos Inicio de URL y pos está en el primer delimitador que no ignora URL
    
    tokens.push_back(str.substr(lastPos, pos - lastPos));
    //Actualizamos lastPos y pos
    lastPos = str.find_first_not_of(delimiters, pos);
    pos = str.find_first_of(delimiters, lastPos);
    return true;
}

//Función auxiliar: Devuelve true si se trata de un Numero
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
                    if(delimiters.find(str.at(aux + 1)) == string::npos)//NO está seguido por un delimiter
                        return false;
                    else
                    {
                        numero.pop_back();//Quitamos el (%,$) del token
                        fin -= 1;//Quitamos uno a fin para poder añadir el (%,$) después
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

//Heurística aplicada a Tokenizar un Numero
bool
Tokenizador::TokenizarNumero(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos,list<string>& tokens) const
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
    if(numero.find_first_not_of("0123456789$%.,", 0) == string::npos)//NO ha encontrado letras en el número
    {
        if(Numero(str, numero, delimiters, pos, lastPos, fin))
        {
            if(numero.at(0) == '.' || numero.at(0) == ',')
                numero= "0" + numero;
            tokens.push_back(numero);
            //Actualizamos lastPos y pos
            lastPos = str.find_first_not_of(delimiters, fin);
            pos = str.find_first_of(delimiters, lastPos);
            return true;
        }
    }
    lastPos= aux;
    return false;
}

//Función auxiliar para determinar si el string email cumple los requisitos 
bool
Tokenizador::Email(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos, const string &especiales, const string &email, const string::size_type &fin) const
{
    string::size_type aux= pos;//Guardamos la pos original
    bool arroba= false;
    bool especial= false;
    string::size_type delimitador= email.find_first_of(delimiters, email.find_first_of('@', 0) + 1);//Encontramos el primer delimitador después de @
    //Derecha del @
    while(delimitador != string::npos)
    {
        if(especiales.find(email[delimitador]) != string::npos)//El delimitador encontrado está entre los especiales
        {
            if(!checkAdyacentes(email, delimiters, delimitador))//False si Delimitador: ".-_" + rodeado de delimitadores
            {
                pos= lastPos + delimitador;//Actualizamos pos porque hemos encontrado un delimitador
                especial= true;
                if(str[pos-1] == '@')
                    pos -= 1;//En caso de que el delimitador esté adyacente derecha del primer arroba -> restamos 1 a pos porque ese @ es delimitador
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

//Heurística aplicada a Tokenizar un Email
bool
Tokenizador::TokenizarEmail(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos,list<string>& tokens) const
{
    string especiales= ".-_@";
    string delimitadores= EliminaEspeciales(delimiters, especiales);//delimitadores se usa para conservar solo los delimitadores que no ignora el caso especial
    string::size_type fin = str.find_first_of(delimitadores, lastPos);
    especiales.pop_back();
    //string::size_type arroba; //Posición del arroba, se usará para dividir el e-mail en dos
    string email= str.substr(lastPos, fin - lastPos);
    if(Email(str, delimiters, pos, lastPos, especiales, email, fin))
    {
        tokens.push_back(str.substr(lastPos, pos - lastPos));//pos se ha modificado en Email hasta el fin del email
        //Actualizamos lastPos y pos
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
        return true;
    }
    return false;
}

//Función auxiliar: Devuelve true si se trata de un Acrónimo
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

//Heurística aplicada a Tokenizar un Acronimo
bool
Tokenizador::TokenizarAcronimo(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos,list<string>& tokens) const
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

            tokens.push_back(acronimo);//Tokenización normal
            //Actualizamos lastPos y pos
            lastPos = str.find_first_not_of(delimiters, pos);
            pos = str.find_first_of(delimiters, lastPos);
            return true;
        }
    return false;
}

//Heurística aplicada a Tokenizar una multipalabra
bool
Tokenizador::TokenizarMultipalabra(const string& str, const string& delimiters, string::size_type& pos, string::size_type& lastPos,list<string>& tokens) const
{
    if(checkAdyacentes(str, delimiters, pos))
    {
        string delimitadores= delimiters;//delimitadores se usa para conservar solo los delimitadores que no ignora el caso especial
        delimitadores.erase(delimitadores.begin() + delimitadores.find('-'));//Eliminamos '-'
        //string::size_type aux= lastPos;
        pos= str.find_first_of(delimitadores, lastPos);//Final de la supuesta multipalabra
        if(pos > str.size())
            pos= str.size();//Evitamos violación de segmento

        string multipalabra= str.substr(lastPos, pos - lastPos);
        string::size_type guion= multipalabra.find_first_of('-', lastPos);
        while(guion != string::npos)
        {
            if(!checkAdyacentes(multipalabra, delimiters, guion))
                break;
            guion= multipalabra.find_first_of('-', guion + 1);
        }

        if(lastPos > str.size())
            lastPos= str.size();//Evitamos violación de segmento
        if(guion != string::npos)//Hemos encontrado un delimitador
            pos= guion + lastPos;
        tokens.push_back(str.substr(lastPos, pos - lastPos));//Añadimos el token
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
    list<string> tokens;
    struct stat dir;
    int err= stat(NomFichEntr.c_str(), &dir);
    // Comprobamos la existencia del directorio
    if(err == -1 || !S_ISDIR(dir.st_mode)) 
    {
        fEntrada.open (NomFichEntr.c_str());
        if(fEntrada)
        {
            list<string> aux;
            int length;
            char * buffer;
            //Obtenemos la longitud del fichero
            fEntrada.seekg(0, ios::end);
            length = fEntrada.tellg();
            fEntrada.seekg(0, ios::beg);
            // Reservamos memoria
            buffer = new char [length];
            // Leemos los datos como si fueran un bloque, todos a la vez
            fEntrada.read(buffer,length);

            string cadena= buffer;//Conversión a string
            delete[] buffer;// Liberamos memoria
            fEntrada.close();

            if(!cadena.empty())
            {
                Tokenizar(cadena, aux);
                tokens.splice(tokens.end(), aux);
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
        
    for(const auto& v : tokens) 
        fSalida << v << '\n' ;

    fSalida.close();
    return true;
}

// Inicializa delimiters=",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@" | casosEspeciales a true; pasarAminuscSinAcentos a false
Tokenizador::Tokenizador ()
{
    this->delimiters= ",;:.-/+*\\ '\"{}[]()<>¡!¿?&#=\t\n\r@";
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
// La lista tokens se vaciará antes de almacenar el resultado de la tokenización.
void
Tokenizador::Tokenizar (const string& s, list<string>& tokens) const
{
    tokens.clear();
    string str= s;
    if(pasarAminuscSinAcentos)
        Filtrar(str);
    if(!casosEspeciales)
    {
        string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        string::size_type pos = str.find_first_of(delimiters, lastPos);//El primer delimitador
        while(string::npos != pos || string::npos != lastPos)
        {
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            lastPos = str.find_first_not_of(delimiters, pos);
            pos = str.find_first_of(delimiters, lastPos);
        }
    }
    else
    {
        string especiales= "_:/.?&-=#@,";
        string delimitadores= this->delimiters;//Necesario para añadir ' ' y \n en caso de que haga falta (no podemos modificar delimiters)

        if(delimiters.find(' ') == string::npos)//No está ' '
            delimitadores += ' ';
        if(delimiters.find('\n') == string::npos)//No está '\n'
            delimitadores += '\n';
        
        string::size_type lastPos = str.find_first_not_of(delimitadores, 0);
        string::size_type pos = str.find_first_of(delimitadores, lastPos);//El primer delimitado
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
                        case '.'://Caso más usado
                            if(!TokenizarNumero(str, delimitadores,pos, lastPos, tokens))
                            {
                                if(!TokenizarAcronimo(str, delimitadores,pos, lastPos, tokens))
                                    especial= false;
                            }
                            else
                                numero= false;
                            break;
                        case ',':
                            if(!TokenizarNumero(str, delimitadores, pos, lastPos, tokens))
                                especial= false;
                            else
                                numero= false;
                            break;
                        case '-':
                            if(!TokenizarMultipalabra(str, delimitadores, pos, lastPos, tokens))
                                especial= false;
                            break;
                        case '@'://Si el primer delimitador NO es @ entonces nunca puede ser Email
                            if(!TokenizarEmail(str, delimitadores, pos, lastPos, tokens))
                                especial= false;
                            break;
                        default:
                            if(!TokenizarURL(str, delimitadores,pos, lastPos, tokens))
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
                    if(TokenizarNumero(str, delimitadores,pos, lastPos, tokens))
                        especial= true;
            if(!especial)
            {
                tokens.push_back(str.substr(lastPos, pos - lastPos));//Tokenización normal
                lastPos = str.find_first_not_of(delimitadores, pos);
                pos = str.find_first_of(delimitadores, lastPos);
            }
        }
    }
}

// Tokeniza el fichero i guardando la salida en un fichero de nombre i + contendrÃ¡ una palabra en cada lÃ­nea del fichero.
// añadiéndole extensión .tk (sin eliminar previamente la extensión de i por ejemplo, del archivo pp.txt se generaría el resultado en pp.txt.tk)
// TRUE si se realiza la tokenización de forma correcta.
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

// Tokeniza el fichero i que contiene un nombre de fichero por línea guardando la salida en ficheros (uno por cada lí­nea de i)
// Cuyo nombre será el leído en i añadiéndole extensión .tk, y que contendrá una palabra en cada línea del fichero leído en i
// TRUE si se realiza la tokenización de forma correcta de todos los archivos que contiene i
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
// Cuyo nombre será el leí­do en i añadiéndole extensión .tk, y que contendrá una palabra en cada lí­nea del fichero leí­do en i
// TRUE si se realiza la tokenización de forma correcta de todos los archivos
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

// Añade al final de ?delimiters? los nuevos delimitadores que aparezcan en ?nuevoDelimiters? (no se almacenarán caracteres repetidos)
// this->delimiters= this->delimiters + FiltrarDelimitadores(nuevoDelimiters);
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

// Cambia la variable privada ?pasarAminuscSinAcentos?. Atención al formato de codificación del corpus (comando file de Linux).
// Para la corrección de la práctica se utilizará el formato actual (ISO-8859).
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
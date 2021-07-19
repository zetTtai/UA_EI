#include "indexadorInformacion.h"

//-------------------------------------------------------------------------------------------------------------------------------------------------
// InfTermDoc
//-------------------------------------------------------------------------------------------------------------------------------------------------

//Funci�n auxiliar para copiar
void
InfTermDoc::Copiar(const InfTermDoc& info)
{
    this->ft= info.ft;
    this->posTerm= info.posTerm;
}
// Inicializa ft = 0
InfTermDoc::InfTermDoc ()
{
    this->ft= 0;
}
InfTermDoc::InfTermDoc(const int& ft, const list<int>& posTerm)
{
    this->ft= ft;
    this->posTerm= posTerm;
    // this->posTerm.sort(std::less<int>());
}
InfTermDoc::InfTermDoc(const int& ft)
{
    this->ft= ft;
    this->posTerm.clear();
}
// Pone ft = 0
InfTermDoc::~InfTermDoc ()
{
    this->ft= 0;
    this->posTerm.clear();
}
// Constructor de copia
InfTermDoc::InfTermDoc (const InfTermDoc &info)
{
    Copiar(info);
}
InfTermDoc& 
InfTermDoc::operator= (const InfTermDoc &info)
{
    if (this != &info)
    {
        (*this).~InfTermDoc();//Destructor
        this->Copiar(info);
    }
    return *this;
}
void 
InfTermDoc::setPosTerm(const list<int>& posTerm)
{
    this->posTerm= posTerm;
    this->posTerm.sort(std::less<int>()); // De menor a mayor
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//InformacionTermino
//-------------------------------------------------------------------------------------------------------------------------------------------------



void
InformacionTermino::Copiar(const InformacionTermino& info)
{
    this->ftc= info.ftc;
    this->l_docs= info.l_docs;
}

InformacionTermino::InformacionTermino ()
{
    this->ftc= 0;
}
InformacionTermino::InformacionTermino (const int& ftc, const long int& id, const InfTermDoc& inf)
{
    this->ftc= ftc;
    this->l_docs[id]= inf;
}

InformacionTermino::~InformacionTermino ()
{
    this->ftc= 0;
    if(!this->l_docs.empty())
        this->l_docs.clear();
}

InformacionTermino::InformacionTermino (const InformacionTermino &info)
{
    Copiar(info);
}

InformacionTermino& 
InformacionTermino::operator= (const InformacionTermino &info)
{
    if (this != &info)
    {
        (*this).~InformacionTermino();//Destructor
        this->Copiar(info);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//InfDoc
//-------------------------------------------------------------------------------------------------------------------------------------------------

//Funci�n auxiliar para copiar
void
InfDoc::Copiar(const InfDoc& info)
{
    this->idDoc= info.idDoc;
    this->numPal= info.numPal;
    this->numPalSinParada= info.numPalSinParada;
    this->numPalDiferentes= info.numPalDiferentes;
    this->tamBytes= info.tamBytes;
    this->fechaModificacion= info.fechaModificacion;
}

long int ID= 1; //Variable global

//TODO Preguntar que hacen
InfDoc::InfDoc ()
{
    //ID++;
}
InfDoc::InfDoc (const long int& id, const int& numPal, const int& numPalSinParada, const int& numPalDiferentes, const int& tamBytes)
{
    this->idDoc= id;
    this->numPal= numPal;
    this->numPalSinParada= numPalSinParada;
    this->numPalDiferentes= numPalDiferentes;
    this->tamBytes= tamBytes;
}
InfDoc::InfDoc (const int& numPal, const int& numPalSinParada, const int& numPalDiferentes, const int& tamBytes)
{
    this->idDoc= ID;
    this->numPal= numPal;
    this->numPalSinParada= numPalSinParada;
    this->numPalDiferentes= numPalDiferentes;
    this->tamBytes= tamBytes;
    ID++;
}
InfDoc::~InfDoc ()
{
    this->idDoc= this->numPal= this->numPalDiferentes= this->numPalDiferentes= this->tamBytes= 0;
    this->fechaModificacion.~Fecha();
}
InfDoc::InfDoc (const InfDoc &info)
{
    Copiar(info);
}
InfDoc& 
InfDoc::operator= (const InfDoc &info)
{
    if (this != &info)
    {
        (*this).~InfDoc();//Destructor
        this->Copiar(info);
    }
    return *this;
}
void
InfDoc::setId()
{
    this->idDoc= ID;
    ID++;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//InfColeccionDocs
//-------------------------------------------------------------------------------------------------------------------------------------------------

void
InfColeccionDocs::Copiar(const InfColeccionDocs& info)
{
    this->numDocs= info.numDocs;
    this->numTotalPal= info.numTotalPal;
    this->numTotalPalSinParada= info.numTotalPalSinParada;
    this->numTotalPalDiferentes= info.numTotalPalDiferentes;
    this->tamBytes= info.tamBytes;
}
InfColeccionDocs::InfColeccionDocs ()
{

}
InfColeccionDocs::InfColeccionDocs (const long int& numDocs, const long int& numTotalPal, const long int& numTotalPalSinParada, const long int& numTotalPalDiferentes, const long int& tamBytes)
{
    this->numDocs= numDocs;
    this->numTotalPal= numTotalPal;
    this->numTotalPalSinParada= numTotalPalSinParada;
    this->numTotalPalDiferentes= numTotalPalDiferentes;
    this->tamBytes= tamBytes;
}
InfColeccionDocs::~InfColeccionDocs ()
{
    this->numDocs= this->numTotalPal= this->numTotalPalSinParada= this->numTotalPalDiferentes= this->tamBytes= 0;
}
InfColeccionDocs::InfColeccionDocs (const InfColeccionDocs &info)
{
    Copiar(info);
}
InfColeccionDocs& 
InfColeccionDocs::operator= (const InfColeccionDocs &info)
{
    if (this != &info)
    {
        (*this).~InfColeccionDocs();//Destructor
        this->Copiar(info);
    }
    return *this;
}

void 
InfColeccionDocs::addDoc(const InfDoc& doc)
{
    this->numTotalPal += doc.getNumPal();
    this->numTotalPalSinParada += doc.getNumPalSinParada();
    this->tamBytes += doc.getTamBytes();
    this->numDocs++;
}

void
InfColeccionDocs::deleteDoc(const InfDoc& doc)
{
    this->numTotalPal -= doc.getNumPal();
    this->numTotalPalSinParada -= doc.getNumPalSinParada();
    this->tamBytes -= doc.getTamBytes();
    this->numDocs--;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//InformacionTerminoPregunta
//-------------------------------------------------------------------------------------------------------------------------------------------------

void
InformacionTerminoPregunta::Copiar(const InformacionTerminoPregunta& info)
{
    this->ft= info.ft;
    this->posTerm= info.posTerm;
}
InformacionTerminoPregunta::InformacionTerminoPregunta ()
{

}
InformacionTerminoPregunta::InformacionTerminoPregunta (const int& ft, const list<int>& posTerm)
{
    this->ft= ft;
    this->posTerm= posTerm;
}
InformacionTerminoPregunta::~InformacionTerminoPregunta ()
{
    this->ft= 0;
    this->posTerm.clear();
}
InformacionTerminoPregunta::InformacionTerminoPregunta (const InformacionTerminoPregunta& info)
{
    Copiar(info);
}
InformacionTerminoPregunta& 
InformacionTerminoPregunta::operator= (const InformacionTerminoPregunta& info)
{
    if (this != &info)
    {
        (*this).~InformacionTerminoPregunta();//Destructor
        this->Copiar(info);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//InformacionPregunta
//-------------------------------------------------------------------------------------------------------------------------------------------------

void
InformacionPregunta::Copiar(const InformacionPregunta& info)
{
    this->numTotalPal= info.numTotalPal;
    this->numTotalPalSinParada= info.numTotalPalSinParada;
    this->numTotalPalDiferentes= info.numTotalPalDiferentes;
}
InformacionPregunta::InformacionPregunta ()
{

}
InformacionPregunta::InformacionPregunta (const long int& numTotalPal, const long int& numTotalPalSinParada, const long int& numTotalPalDiferentes)
{
    this->numTotalPal= numTotalPal;
    this->numTotalPalSinParada= numTotalPalSinParada;
    this->numTotalPalDiferentes= numTotalPalDiferentes;
}
InformacionPregunta::~InformacionPregunta ()
{
    this->numTotalPal= this->numTotalPalSinParada= this->numTotalPalDiferentes= 0;
}
InformacionPregunta::InformacionPregunta (const InformacionPregunta& info)
{
    Copiar(info);
}
InformacionPregunta& 
InformacionPregunta::operator= (const InformacionPregunta& info)
{
    if (this != &info)
    {
        (*this).~InformacionPregunta();//Destructor
        this->Copiar(info);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------------------------------------------------------------
//InformacionPregunta
//-------------------------------------------------------------------------------------------------------------------------------------------------

Fecha::Fecha() 
{
    time_t now = time(0);
    tm *ltm = localtime(&now);
    this->year= 1900 + ltm->tm_year;
    this->month= 1 + ltm->tm_mon;
    this->day= ltm->tm_mday;
    string hour;
    hour= to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec);
    this->hour= hour;
}

Fecha&
Fecha::operator= (const Fecha &fecha)
{
    if (this != &fecha)
    {
        (*this).~Fecha();// Destructor
        this->day = fecha.day;
        this->month = fecha.month;
        this->year= fecha.year;
        this->hour = fecha.hour;
    }
    return *this;
}

Fecha::Fecha (const struct tm * timeinfo)
{
    string fecha= asctime(timeinfo); // Mon Apr 12 18:55:11 2021

    istringstream iss(fecha);
    vector<string> vec;
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(vec));
    
    this->day= stoi(vec.at(2));
    this->year= stoi(vec.at(4));
    this->hour= vec.at(3);
    unordered_map<string, int> meses;
    meses["Jan"]= 1; meses["Feb"]= 2; meses["Mar"]= 3;
    meses["Apr"]= 4; meses["May"]= 5; meses["Jun"]= 6;
    meses["Jul"]= 7; meses["Aug"]= 8; meses["Sep"]= 9;
    meses["Oct"]= 10; meses["Nov"]= 11; meses["Dec"]= 12;
    this->month= meses[vec.at(1)];
}

Fecha::Fecha (const string& str) // day month year hour
{
    vector<string> vec;
    istringstream iss(str);
    // Creamos un vector de string a partir de la linea
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(vec));
    this->day= stoi(vec.at(0));
    this->month= stoi(vec.at(1));
    this->year= stoi(vec.at(2));
    this->hour= vec.at(3);
}

Fecha::Fecha(int day, int month, int year) {
    setDay(day);
    setMonth(month);
    setYear(year);
    time_t now = time(0);
    tm *ltm = localtime(&now);
    string hour;
    hour= to_string(ltm->tm_hour) + ":" + to_string(ltm->tm_min) + ":" + to_string(ltm->tm_sec);
    this->hour= hour;
}

Fecha::~Fecha() 
{
    this->day= this->month= this->year= 0;
    this->hour= "";
}

void
Fecha::setDay(int day) 
{
    if ((day <= 0) || (day >= 32)) {
        cerr << "ERROR: Dia debe ser un n�mero entre 1 y 31" << '\n';
        this->day= 0;
        return;
    }
    this->day = day;
}

void
Fecha::setMonth(int month) 
{
    if ((month <= 0) || (month >= 13)) {
        cerr << "ERROR: Mes debe ser un n�mero entre 1 y 12" << '\n';
        this->month= 0;
        return;
    }
    this->month = month;
}

void
Fecha::setYear(int year) 
{
    if ((year <= 1899) || (year >= 2022)) {
        cerr << "ERROR: A�o debe ser un n�mero entre 1900 y 2021" << '\n';
        this->year= 0;
        return;
    }
    this->year = year;
}

bool
Fecha::moreRecentHour(const Fecha &fecha) const
{
    string thishora, hora;
    thishora= this->hour;
    hora= fecha.hour;

    replace( thishora.begin(), thishora.end(), ':', ' ');
    istringstream iss;
    iss= istringstream(thishora);
    vector<string> thisvector;
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(thisvector));

    vector<string> vector;
    replace( hora.begin(), hora.end(), ':', ' ');
    iss= istringstream(hora);
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(vector));
    
    if(stoi(thisvector.at(0)) > stoi(vector.at(0))) // Comprobamos la hora
        return true;
    else
        if(stoi(thisvector.at(0)) == stoi(vector.at(0))) // Misma hora
            if(stoi(thisvector.at(1)) > stoi(vector.at(1))) // Comprobamos minutos
                return true;
            else
                if(stoi(thisvector.at(1)) == stoi(vector.at(1))) // Mismo minuto
                    if(stoi(thisvector.at(2)) >= stoi(vector.at(2))) // Comprobamos segundos
                        return true;
    return false;
}

bool
Fecha::moreRecentThan(const Fecha& fecha) const
{
    if(this->year > fecha.year)
        return true;
    else
        if(this->year == fecha.year) // Mismo a�o
            if(this->month > fecha.month)
                return true;
            else
                if(this->month == fecha.month) // Mismo mes
                    if(this->day > fecha.day)
                        return true;
                    else
                        if(this->day == fecha.day) // Mismo dia
                            return moreRecentHour(fecha);
    return false;
}
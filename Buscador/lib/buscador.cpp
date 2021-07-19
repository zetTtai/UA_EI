#include "buscador.h"

double Buscador::getcputime(void) {
	struct timeval tim;
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	tim=ru.ru_utime;
	double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
	tim=ru.ru_stime;
	t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
	return t;
}
Buscador::Buscador() : IndexadorHash()
{
    formSimilitud= 0;
    c= 2; k1= 1.2; b= 0.75;
}

Buscador::Buscador(const string& directorioIndexacion, const int& f) : IndexadorHash(directorioIndexacion)
{
    formSimilitud= f;
    c= 2; k1= 1.2; b= 0.75;
}

Buscador::Buscador(const Buscador& busc) : IndexadorHash(busc)
{
    formSimilitud = busc.formSimilitud;
    c = busc.c; k1 = busc.k1; b = busc.b;
}

Buscador::~Buscador()
{
    priority_queue<ResultadoRI> empty;
    swap(this->docsOrdenados, empty); // Vaciamos docsOrdenados
}

Buscador&
Buscador::operator= (const Buscador& busc)
{
    if (this != &busc)
    {
        (*this).~Buscador();//Destructor ERROR
        Buscador(busc);
    }
    return *this;
}

double
Buscador::WeightDoc(InformacionTermino infTerm, InfDoc infDoc, const double &ft, const double &ld, const double& avg) const
{
    double lambda= (double)infTerm.getFtc() / (double)DevuelveInfColeccion().getNumDocs();
    double ftd=  ft * log2(1.0 + ((c * avg) / ld));

    double first= log2(1 + lambda) + ((ftd * log2(1 + lambda)) / lambda);
    double last= ((double)infTerm.getFtc() + 1.0) / ((double)infTerm.getDocs().size() * (ftd + 1.0));
    return first * last;
}

double
Buscador::IDF(const InformacionTermino& infTerm) const
{
    double numerador= (double)DevuelveInfColeccion().getNumDocs() - (double)infTerm.getDocs().size() + 0.5;
    double denominador= (double)infTerm.getDocs().size() + 0.5;
    return log2(numerador/denominador);
}

double
Buscador::FrecuencyDoc(const InformacionTermino& infTerm, const InfDoc& infDoc, const double &ft) const
{
    double numerador= ft * (this->k1 + 1.0);
    double avg= (double)DevuelveInfColeccion().getNumTotalPalSinParada()/ (double)DevuelveInfColeccion().getNumDocs();
    double denominador= ft + this->k1 * (1.0 - this->b + (this->b * abs((double)infDoc.getNumPalSinParada()))/avg);
    return numerador/denominador;
}

void
Buscador::DFR(const int& numDocumentos, const int& numPregunta)
{
    double vSimilitud, wiq, ft;
    vSimilitud= wiq= ft= 0.0;
    double lambda, ftd, first, last, wid;
    lambda= ftd= first= last= wid= 0.0;
    double avg= (double) informacionColeccionDocs.getNumTotalPalSinParada() / (double) informacionColeccionDocs.getNumDocs();
    // Iterador que recorre el unordered_map indicePregunta 
    std::unordered_map<std::string, InformacionTerminoPregunta>::iterator infTermPreg;
    // Iterador que recorre el unordered_map indice
    std::unordered_map<std::string, InformacionTermino>::iterator infTermino;
    // Iterador que recorre el unordered_map l_docs de un informacionTermino
    std::unordered_map<long, InfTermDoc>::iterator infTermDoc;
    // Iterador que recorre el unordered_map indiceDocs
    std::unordered_map<string, InfDoc>::iterator infDoc;
    // priority_queue auxiliar para almacenar los documentos que cumplan las condiciones
    priority_queue<pair<double,long>> aux;

    for(infDoc= indiceDocs.begin(); infDoc != indiceDocs.end(); ++infDoc)
    {
        vSimilitud= 0.0;
        for(infTermPreg= indicePregunta.begin(); infTermPreg != indicePregunta.end(); infTermPreg++)
        {
            infTermino= indice.find(infTermPreg->first);
            if(infTermino != indice.end())
            {
                infTermDoc= infTermino->second.getDocs().find(infDoc->second.getId());
                if(infTermDoc != infTermino->second.getDocs().end())
                {
                    ft= (double) infTermino->second.getDocs()[infDoc->second.getId()].getFt();
                    if(ft != 0)
                    {
                        wiq= (double) infTermPreg->second.getFt() / (double) infPregunta.getNumTotalPal();
                        
                        lambda= (double)infTermino->second.getFtc() / (double) informacionColeccionDocs.getNumDocs();
                        ftd=  ft * log2(1.0 + ((c * avg) / (double) informacionColeccionDocs.getNumTotalPalSinParada()));
                        
                        wid= ( log2(1 + lambda) + ((ftd * log2(1 + lambda)) / lambda)) * 
                        (((double)infTermino->second.getFtc() + 1.0) / ((double)infTermino->second.getDocs().size() * (ftd + 1.0)));
                        
                        vSimilitud += wiq * wid;
                    }
                }
            }
        }
        if(vSimilitud != 0.0)
            aux.push({vSimilitud, infDoc->second.getId()});
    }

    for(int i= 0; i < numDocumentos && !aux.empty(); i++)
    {
        docsOrdenados.push(ResultadoRI(aux.top().first, aux.top().second, numPregunta));
        aux.pop();
    }
}
void
Buscador::BM25(const int& numDocumentos, const int& numPregunta)
{
    double idf, avg, ft, fd, vSimilitud;
    idf= ft= fd= vSimilitud= 0.0;
    avg= (double) informacionColeccionDocs.getNumTotalPalSinParada() / (double) informacionColeccionDocs.getNumDocs();

    // Iterador que recorre el unordered_map indicePregunta 
    std::unordered_map<std::string, InformacionTerminoPregunta>::iterator infTermPreg;
    // Iterador que recorre el unordered_map indice
    std::unordered_map<std::string, InformacionTermino>::iterator infTermino;
    // Iterador que recorre el unordered_map l_docs de un informacionTermino
    std::unordered_map<long, InfTermDoc>::iterator infTermDoc;
    // Iterador que recorre el unordered_map indiceDocs
    std::unordered_map<string, InfDoc>::iterator infDoc;
    // priority_queue auxiliar para almacenar los documentos que cumplan las condiciones
    priority_queue<pair<double,long>> aux;

    for(infDoc= indiceDocs.begin(); infDoc != indiceDocs.end(); infDoc++)
    {
        vSimilitud= 0.0;

        for(infTermPreg= indicePregunta.begin(); infTermPreg != indicePregunta.end(); infTermPreg++)
        {
            infTermino= indice.find(infTermPreg->first);
            if(infTermino != indice.end())
            {
                infTermDoc= infTermino->second.getDocs().find(infDoc->second.getId());
                if(infTermDoc != infTermino->second.getDocs().end())
                {
                    ft= (double) infTermino->second.getDocs()[infDoc->second.getId()].getFt();
                    if(ft != 0)
                    {
                        idf= log2(((double) informacionColeccionDocs.getNumDocs() - (double) infTermino->second.getDocs().size() + 0.5) / ((double) infTermino->second.getDocs().size() + 0.5));
                        fd= (ft* (this->k1 + 1.0)) / (ft + this->k1 *(1.0 - this->b + (this->b * abs((double) infDoc->second.getNumPalSinParada()/avg))));
                        vSimilitud += idf * fd;
                    }
                }
            }
        }
        if(vSimilitud != 0.0)
            aux.push({vSimilitud, infDoc->second.getId()});
    }

    for(int i= 0; i < numDocumentos && !aux.empty(); i++)
    {
        docsOrdenados.push(ResultadoRI(aux.top().first, aux.top().second, numPregunta));
        aux.pop();
    }
}

bool
Buscador::GestionarBusqueda(const int& numDocumentos, const int& numPregunta)
{
    if(!DevolverIndicePregunta().empty())
    {
        if(!this->formSimilitud) // DFR
            DFR(numDocumentos, numPregunta);
        else // BM25
            BM25(numDocumentos, numPregunta);
        return true;
    }
    else
        return false;


}

bool
Buscador::Buscar(const int& numDocumentos)
{
    priority_queue<ResultadoRI> empty;
    swap(this->docsOrdenados, empty); // Vaciamos docsOrdenados
    // this->docsOrdenados.clear();
    return GestionarBusqueda(numDocumentos, 0);
}

bool
Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin)
{
    struct stat dir;
    int err= stat(dirPreguntas.c_str(), &dir);
    // Comprobamos la existencia del directorio
    if(err == -1 || !S_ISDIR(dir.st_mode)) // No existe dirPreguntas
        return false; 
    else
    {
        priority_queue<ResultadoRI> empty;
        swap(this->docsOrdenados, empty); // Vaciamos docsOrdenados
        ifstream fPregunta;
        string ruta;
        for(int i= numPregInicio; i <= numPregFin; i++)
        {
            ruta= dirPreguntas.c_str() + to_string(i) + ".txt";
            fPregunta.open(ruta.c_str());
            if(fPregunta)
            {
                string preg;
                getline(fPregunta, preg);
                fPregunta.close();

                IndexarPregunta(preg);

                if(!GestionarBusqueda(numDocumentos, i)) // TODO mejorar
                    return false;
            }
        }
        return true;
    }
}

string
Buscador::getDocName(const long int& id) const
{
    string ruta;
    bool encontrado= false;
    std::unordered_map<string, InfDoc>::const_iterator doc;
    for(doc = indiceDocs.begin(); doc != indiceDocs.end(); ++doc)
        if(doc->second.getId() == id)
        {
            ruta= doc->first;
            encontrado= true;
            break;
        }
    if(encontrado)
    {
        string::size_type lastPos = ruta.find_last_of("/\\");
        lastPos++;
        if(lastPos == string::npos)
            lastPos= 0;
        string::size_type pos= ruta.find_first_of(".", lastPos);
        return ruta.substr(lastPos, pos - lastPos);
    }
    return "";
}

string
Buscador::Imprimir(const int& numDocumentos) const
{
    string result;
    priority_queue<ResultadoRI> docs(this->docsOrdenados);
    int aux;
    while(!docs.empty())
    {
        aux= docs.top().getNumPregunta();
        for(int i=0; i < numDocumentos; ++i) // Tantas posiciones como numDocumentos
        {
            if(docs.top().getNumPregunta() != aux || docs.empty())
                break;
            else
            {
                aux= docs.top().getNumPregunta();
                result +=  to_string(aux) + ' ';
                if(formSimilitud == 0)
                    result += "DFR ";
                else 
                    result += "BM25 ";
                result += getDocName(docs.top().IdDoc()) + ' ' + to_string(i) + ' ' + to_string(docs.top().VSimilitud()) + ' ';
                if(docs.top().getNumPregunta() == 0)
                {
                    string preg;
                    if(DevuelvePregunta(preg))
                        result += preg +'\n';
                    else
                        result += '\n';
                }
                else 
                    result += "ConjuntoDePreguntas\n";
            }
            docs.pop();
        }
    }
    return result;
}

void
Buscador::ImprimirResultadoBusqueda(const int& numDocumentos) const
{
    cout << Imprimir(numDocumentos);
}

bool
Buscador::ImprimirResultadoBusqueda(const int& numDocumentos, const string& nombreFichero) const
{
    ofstream fSalida(nombreFichero.c_str());
    if(fSalida)
    {
        fSalida << Imprimir(numDocumentos);
        fSalida.close();
        return true;
    }
    return false;
}
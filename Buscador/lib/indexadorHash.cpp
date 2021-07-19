#include "indexadorHash.h"
#include <string.h>

// Ruta para la informaci�n de indice almacenada en disco
#define PATH1 "indiceDisco.txt"
// Ruta para la informaci�n de indiceDocs almacenada en disco
#define PATH2 "indiceDocsDisco.txt"

double 
IndexadorHash::getcputime(void) {
    struct timeval tim;
    struct rusage ru;
    getrusage(RUSAGE_SELF, &ru);
    tim=ru.ru_utime;
    double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
    tim=ru.ru_stime;
    t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
    return t;
}

void
IndexadorHash::AddInfTerm(const InformacionTermino& inf, const string& aux)
{
    ofstream file(PATH1, ios::app); //A�adimos ios::app para poder a�adir lineas al final del fichero
    if(file)
    {
        //    \t      Comienza  list      \t                    
        // ftc   id ft pos pos pos pos pos    id2 ft2: pos2 pos2\n
        file << inf.getFtc();
        for(auto doc : inf.getDocs()) // TODO probar
        {
            file << '\t' << doc.first << ' ' << doc.second.getFt();
            for(auto pos: doc.second.getPosTerm())
                file << ' ' << pos;
        }
        file << '\n';
        file.close();
    }
    ifstream inFile(PATH1);
    // Obtenemos la linea en la que hemos escrito el nuevo termino con std::count
    this->indiceDisco[aux]= (count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n') - 1);
    inFile.close();
}

InformacionTermino
IndexadorHash::GetInfTerm(const string& str, const string::size_type& i) const
{
    InformacionTermino inf;
    vector<string> vec;
    istringstream iss;
    long int id= 0;
    int ft= 0;
    vector<int> posTerm;
    string::size_type pos;
    string::size_type lastPos;
    if(this->almacenarEnDisco)
    {
        pos = str.find_first_of('\t', 0);
        lastPos = str.find_first_not_of('\t', pos);
        inf.setFtc(stoi(str.substr(0, pos)));
        pos= str.find_first_of('\t', 0); //Actualizamos pos para obtener el substring de la lista
    }
    else
    {
        lastPos = str.find_first_not_of('\t', i);
        pos = str.find_first_of('\t', lastPos);
    }
    while(string::npos != pos || string::npos != lastPos) // Bucle para obtener la informacion de InfTermDoc
    {
        iss= istringstream(str.substr(lastPos, pos - lastPos));// id ft pos pos pos\tid ft pos pos...
        copy(istream_iterator<string>(iss),
            istream_iterator<string>(),
            back_inserter(vec));
        id= stoi(vec.at(0));
        ft= stoi(vec.at(1));
        if(this->almacenarPosTerm)//pal2 3\t2 2  0 2\t1 1  2\t
        {                         //2 2  0 2\t1 1  2\t
            if(ft != 1)
            {
                vec.erase(vec.begin()); vec.erase(vec.begin()); //Eliminamos el id y el ft
                posTerm.clear();
                for(auto& i : vec)
                    posTerm.push_back(stoi(i));
            }
            else
                posTerm.push_back(stoi(vec.at(2)));
        }
        inf.addDoc(id,InfTermDoc(ft, posTerm));
        lastPos = str.find_first_not_of('\t', pos);
        pos = str.find_first_of('\t', lastPos);
        posTerm.clear();
        vec.clear();
    }
    return inf;
}

InfDoc
IndexadorHash::GetInfDoc(const string& str) const
{
    // doc idDoc numPal numPalSinParada numPalDiferentes tamBytes\tFecha\n
    vector<string> vec;
    // Reseteamos lastPos y pos para la nueva linea
    string::size_type lastPos = str.find_first_not_of('\t', 0);
    string::size_type pos = str.find_first_of('\t', lastPos);

    istringstream iss(str.substr(lastPos, pos - lastPos));// doc idDoc numPal numPalSinParada numPalDiferentes tamBytes
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(vec));
    
    InfDoc inf(stoi(vec[1]), stoi(vec[2]), stoi(vec[3]), stoi(vec[4]), stoi(vec[5]));

    inf.setFecha(Fecha(str.substr(pos, str.size())));
    return inf;
}

string
IndexadorHash::GetInfo(const string& ruta, const long int& num) const
{
    string str= "";
    ifstream file(ruta.c_str());
    if(file)
    {
        file.seekg(std::ios::beg);
        for(int i=0; i < num; ++i)
            file.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        getline(file, str);
        file.close();
    }
    return str;
}

void
IndexadorHash::ModifyLine(const long int& line, const int& ft, const string& newline, const string& path) const
{
    ifstream file(path.c_str());
    ofstream temp("replace.tmp"); // TODO Buscar sobre ficheros binarios
    if(file.is_open() && temp.is_open())
    {
        int count= 0;
        string aux;
        while(getline(file, aux))
        {
            if(count == line)
            {
                if(path == PATH1) //indice
                {
                    if(!newline.empty())
                    {
                        size_t pos= aux.find_first_of('\t');
                        int ftc= stoi(aux.substr(0, pos)) + ft; // Actualizamos ftc
                        aux.erase(0, pos); //\tid ft list<int>\n 
                        aux.erase(std::remove(aux.begin(), aux.end(), '\n'), //eliminamos el '\n'
                                aux.end());
                        temp << ftc << aux << newline;
                    }
                    else
                        temp << ""; // Si newline est� vacio entonces elimina
                }
                else // indiceDocs s�lo se llama a esta funci�n cuando hay que eliminar la informaci�n de un documento anticuado 
                {
                    temp << "";
                }
            }
            else
                temp << aux << '\n';
            count++;
        }
        file.close();
        temp.close();
        remove(path.c_str());
        rename("replace.tmp", path.c_str());
    }
    else
        cerr << "No se ha podido abrir el archivo: " << path << '\n';
}

IndexadorHash::IndexadorHash() : tok()
{
    this->ficheroStopWords= "";
    this->tipoStemmer= 0;
    this->stopWords.clear();

    this->directorioIndice= "./"; // Directorio raiz
    this->almacenarEnDisco= false;
    this->almacenarPosTerm= false;
    
    this->indiceDisco.clear();
    this->indiceDocsDisco.clear();
    this->indice.clear();
    this->indiceDocs.clear();
    this->indicePregunta.clear();
    this->infPregunta.~InformacionPregunta();
    this->informacionColeccionDocs.~InfColeccionDocs();
}

void
IndexadorHash::Copiar(const IndexadorHash& index)
{
    this->indice= index.indice;
    this->indiceDocs= index.indiceDocs;
    this->informacionColeccionDocs= index.informacionColeccionDocs;
    this->pregunta= index.pregunta;
    this->indicePregunta= index.indicePregunta;
    this->infPregunta= index.infPregunta;
    this->stopWords= index.stopWords;
    this->ficheroStopWords= index.ficheroStopWords;
    this->directorioIndice= index.directorioIndice;
    this->tipoStemmer= index.tipoStemmer;
    this->almacenarEnDisco= index.almacenarEnDisco;
    this->almacenarPosTerm= index.almacenarPosTerm;
}

unordered_map<string, vector<int>>
IndexadorHash::ExtractInfo(string& str, InfDoc& doc) const
{
    unordered_map<string, vector<int>> infTerm;
    int pal, sinparada;
    pal= sinparada= 0;
    stemmerPorter stem;
    // if(this->tok.PasarAminuscSinAcentos())
    //     this->tok.Filtrar(str);
    string tokens;
    this->tok.Tokenizar(str, tokens);
    istringstream iss(tokens);
    string token;
    while(iss >> token)
    {
        stem.stemmer(token, this->tipoStemmer); // Stemizamos el token
        if(stopWords.find(token) == stopWords.end()) // NO es palabra de parada
        {
            if(infTerm.find(token) == infTerm.end()) // Nuevo termino
                infTerm[token]= vector<int>{pal};
            else // Termino duplicado
                infTerm[token].push_back(pal);
            sinparada++;
        }
        pal++;
    }
    doc.addPal(pal, sinparada, infTerm.size());
    // char * cstr= new char[str.length() + 1];
    // strcpy(cstr,str.c_str());
    // string delimitadores= this->tok.DelimitadoresPalabra() + " \n";
    // char* token = std::strtok(cstr, delimitadores.c_str());
    // while(token != 0) // TODO Mejorar
    // {
    //     stem.stemmer(token, this->tipoStemmer); // Stemizamos el token
    //     if(stopWords.find(token) == stopWords.end()) // NO es palabra de parada
    //     {
    //         if(infTerm.find(token) == infTerm.end()) // Nuevo termino
    //             infTerm[token]= vector<int>{pal};
    //         else // Termino duplicado
    //             infTerm[token].push_back(pal);
    //         sinparada++;
    //     }
    //     pal++;
    //     token= strtok(NULL, delimitadores.c_str());
    // }
    // delete[] cstr;
    // doc.addPal(pal, sinparada, infTerm.size());
    return infTerm;
}

bool
IndexadorHash::AddTerms(const string& ruta, const unordered_map<string, vector<int>>& infTerm, InfDoc& inf)
{
    if(!ruta.empty()) // Es un documento
    {
        try
        {
            if(!this->almacenarEnDisco)// TODO probar si recorriendo el �ndice es mejor que recorrer todas las palabras del fichero
            {
                // unordered_map<string, InformacionTermino>::const_iterator got;
                for(auto& termino : infTerm)
                {
                    // got= this->indice.find(termino.first);
                    if(indice.find(termino.first) == indice.end()) // El termino NO se encuentra en el indice
                    {
                        if(this->almacenarPosTerm)
                            this->indice[termino.first]= InformacionTermino(termino.second.size(), inf.getId(), InfTermDoc(termino.second.size(), termino.second));
                        else 
                            this->indice[termino.first]= InformacionTermino(termino.second.size(), inf.getId(), InfTermDoc(termino.second.size()));
                    }
                    else // El termino ya esta en el indice
                    {
                        this->indice[termino.first].setFtc(indice[termino.first].getFtc() + termino.second.size()); // Sumamos a la ftc la nueva ft
                        if(this->almacenarPosTerm)
                            this->indice[termino.first].addDoc(inf.getId(), InfTermDoc(termino.second.size(), termino.second));
                        else 
                            this->indice[termino.first].addDoc(inf.getId(), InfTermDoc(termino.second.size()));
                    }
                }
                // inf.addPal(pal, sinparada, infTerm.size()); // A?adimos el n? palabras
                this->indiceDocs[ruta]= inf; // A�adimos el documento con toda su informaci?n a indiceDocs
                this->informacionColeccionDocs.addDoc(inf); // A?adimos su informaci?n a nuestra colecci?n
                this->informacionColeccionDocs.setDiferentes(this->indice.size()); // Actualizamos el num de palabras diferentes
            }
            else // AlmacenarEnDisco == TRUE
            {
                vector<string> info; // pos i corresponde a la linea i del fichero PATH1
                ifstream f(PATH1);
                if(f)
                {
                    string line;
                    while(getline(f, line))
                        info.push_back(line + '\n');
                    f.close();
                }
                // unordered_map<string, long int>::const_iterator got;
                for(auto& termino : infTerm)
                {
                    // got= this->indiceDisco.find(termino.first);
                    if(indiceDisco.find(termino.first) == indiceDisco.end()) // El termino NO se encuentra en el indice
                    {
                        string line;
                        line += to_string(termino.second.size()) + '\t' + to_string(inf.getId()) + ' ' + to_string(termino.second.size()) ;
                        for(auto& pos : termino.second)
                            line += ' ' + to_string(pos);
                        line += '\n';
                        info.push_back(line);
                        this->indiceDisco[termino.first]= (info.size() - 1);
                    }
                    else
                    {
                        string newline = "";
                        newline += '\t' + to_string(inf.getId()) + ' ' + to_string(termino.second.size()) + ' ';
                        for(auto& pos: termino.second)
                            newline += ' ' + to_string(pos);
                        newline += '\n';
                        string aux= info[indiceDisco[termino.first]];

                        size_t pos= aux.find_first_of('\t');
                        int ftc= stoi(aux.substr(0, pos)) + termino.second.size(); // Actualizamos ftc
                        aux.erase(0, pos); //\tid ft list<int>\n 
                        aux.erase(std::remove(aux.begin(), aux.end(), '\n'), //eliminamos el '\n'
                                aux.end());
                        
                        info[indiceDisco[termino.first]]= to_string(ftc) + aux + newline;
                    }
                }
                ofstream file(PATH2, ios::app);
                if(file)
                {
                    // doc idDoc numPal numPalSinParada numPalDiferentes tamBytes   Fecha\n Fecha: dia mes anyo hora
                    file << ruta << ' ' << inf.getId() << ' ' << inf.getNumPal() << ' ' << inf.getNumPalSinParada() << ' ' << infTerm.size() << ' ' << inf.getTamBytes() << '\t';
                    file << inf.getFecha().getDay() << ' ' << inf.getFecha().getMonth() << ' ' << inf.getFecha().getYear() << ' ';
                    file << inf.getFecha().getHour() << '\n';
                    file.close();
                }
                ifstream inFile(PATH2);
                // Obtenemos la linea en la que hemos escrito el nuevo termino con std::count
                this->indiceDocsDisco[ruta]= (count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n') - 1);
                inFile.close();
                this->informacionColeccionDocs.addDoc(inf); // A?adimos su informaci?n a nuestra colecci?n
                this->informacionColeccionDocs.setDiferentes(this->indiceDisco.size()); // Actualizamos el num de palabras diferentes
                remove(PATH1);
                ofstream outFile(PATH1, ios::app);
                if(outFile)
                {   
                    for(auto& data : info)
                        outFile << data;
                    outFile.close();
                }
            }
        }
        catch(const std::bad_alloc& e)
        {
            if(!this->almacenarEnDisco)
            {
                cerr << "ERROR: (Indexar) Se ha alcanzado el limite de memoria.\n";
                cerr << "Ultimo documento indexado: " << ruta << '\t' << "Ultimo termino indexado: " << this->indice.end()->first << '\n';
            }
            else
            {
                cerr << "ERROR: (Indexar) Se ha alcanzado el limite de memoria.\n";
                cerr << "Ultimo documento indexado: " << ruta << '\t' << "Ultimo termino indexado: " << this->indiceDisco.end()->first << '\n';
            }
            return false;
        }
    }
    else // Es una pregunta
    {
        try
        {
            for(auto& termino : infTerm)
            {
                if(this->almacenarPosTerm)
                    this->indicePregunta[termino.first]= InformacionTerminoPregunta(termino.second.size(), termino.second);
                else
                    this->indicePregunta[termino.first]= InformacionTerminoPregunta(termino.second.size()); //Le pasamos una lista vacia
            }
            //                                     Aunque pertenezcan a un infDoc corresponden a la pregunta
            this->infPregunta= InformacionPregunta(inf.getNumPal(), inf.getNumPalSinParada(), this->indicePregunta.size());
        }
        catch(const std::bad_alloc& e)
        {
            cerr << "ERROR: (IndexarPregunta) Se ha alcanzado el l?mite de memoria. ?ltimo termino indexado: " << this->indicePregunta.end()->first << "\n";
            return false;
        }
    }
    return true;
}

void
IndexadorHash::AlmacenarStopWords()
{
    ifstream fEntrada(this->ficheroStopWords);
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
        // buffer = new char [length];
        buffer= (char *) malloc(sizeof(char)*length);
        // Leemos los datos como si fueran un bloque, todos a la vez
        fEntrada.read(buffer,length);
        // string cadena= buffer;//Conversi?n a string
        string cadena(buffer, length);
        // delete[] buffer;// Liberamos memoria
        free(buffer);
        fEntrada.close();
        string tokens, word;
        this->tok.Tokenizar(cadena, tokens); // Tokenizamos las stopWords
        istringstream iss(tokens);
        stemmerPorter stem;
        while(iss >> word)
        {
            if(this->DevolverPasarAminuscSinAcentos())
                this->tok.Filtrar(word);
            stem.stemmer(word, this->tipoStemmer); // Stemizamos el token
            this->stopWords.insert(word); //a?adimos todas las palabras del fichero a la variable privada stopWords
            this->numStopWords += 1;
        }
    }
    else
        cerr << "ERROR: No existe el archivo: " << this->ficheroStopWords << '\n';
}

IndexadorHash::IndexadorHash(const string& fichStopWords, const string& delimitadores, const bool& detectComp, const bool& minuscSinAcentos, 
            const string& dirIndice, const int& tStemmer, const bool& almEnDisco, const bool& almPosTerm) : tok(delimitadores, detectComp, minuscSinAcentos)
{
    this->ficheroStopWords= fichStopWords;
    if(tStemmer < 0 || tStemmer > 2)
    {
        cerr << "ERROR: El tipo de stemmer introducido no es v?lido, debe ser 0, 1 ? 2" << '\n';
        this->tipoStemmer= 0;
    }
    else
        this->tipoStemmer= tStemmer;
    this->numStopWords= 0;
    AlmacenarStopWords();
    if(dirIndice.empty())
        this->directorioIndice= "./"; // Directorio raiz
    else
        this->directorioIndice= dirIndice;
    this->almacenarEnDisco= almEnDisco;
    if(this->almacenarEnDisco)
    {
        remove(PATH1);
        remove(PATH2);
    }
    this->almacenarPosTerm= almPosTerm;
    this->indice.clear();
    this->indiceDocs.clear();
    this->indiceDisco.clear();
    this->indiceDocsDisco.clear();
    this->indicePregunta.clear();
    this->infPregunta.~InformacionPregunta();
    this->informacionColeccionDocs.~InfColeccionDocs();
}

IndexadorHash::IndexadorHash(const string& directorioIndexacion)
{
    this->RecuperarIndexacion(directorioIndexacion);
}

IndexadorHash::~IndexadorHash()
{
    if(this->almacenarEnDisco)
    {
        this->indiceDisco.clear();
        this->indiceDocsDisco.clear();
    }
    this->indice.clear();
    this->indiceDocs.clear();
    this->pregunta= "";
    this->indicePregunta.clear();
    this->stopWords.clear();
    this->ficheroStopWords= "";
    this->tipoStemmer= 0;
    this->almacenarEnDisco= false;
    this->almacenarPosTerm= false;
}

IndexadorHash::IndexadorHash(const IndexadorHash& index) : tok(index.tok) 
{
    Copiar(index);
}

IndexadorHash& 
IndexadorHash::operator= (const IndexadorHash& index)
{
    if (this != &index)
    {
        (*this).~IndexadorHash();//Destructor ERROR
        IndexadorHash(index);
    }
    return *this;
}

void
IndexadorHash::IndexarFichero(const string& ruta, InfDoc& doc, bool& memory)
{
    // ifstream fEntrada((ruta + ".tk").c_str()); //Abrimos el archivo tokenizado
    ifstream fEntrada(ruta.c_str());
    if(fEntrada)
    {
        int length;
        char * buffer;
        // Obtenemos la longitud del fichero
        fEntrada.seekg(0, ios::end);
        length = fEntrada.tellg();
        fEntrada.seekg(0, ios::beg);
        // Reservamos memoria
        // buffer = new char [length];
        buffer= (char *) malloc(sizeof(char)*length);
        // Leemos los datos como si fueran un bloque, todos a la vez
        fEntrada.read(buffer,length);
        string cadena(buffer, length);
        free(buffer);
        unordered_map<string, vector<int>> infTerm;
        fEntrada.close();
        if(!AddTerms(ruta, ExtractInfo(cadena, doc), doc))
            memory= false; // Si addTerms devuelve false entonces no queda memoria
    }
    else
        cerr << "ERROR: No existe el archivo: " << ruta << '\n';
}

bool
IndexadorHash::Indexar(const string& ficheroDocumentos)
{
    ifstream fEntrada(ficheroDocumentos.c_str()); //Abrimos el archivo tokenizado
    if(fEntrada)
    {
        int length;
        char * buffer;
        // Obtenemos la longitud del fichero
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

        istringstream iss(cadena);
        string ruta;
        bool memory= true;
        struct stat t_stat;
        while(getline(iss, ruta))
        {
            if(!ruta.empty())
            {
                InfDoc doc;
                stat(ruta.c_str(), &t_stat);
                doc.setTamBytes((int) *&t_stat.st_size);
                Fecha fecha(localtime(&t_stat.st_ctime)); // Obtenemos la fecha de modificacion
                if(!this->almacenarEnDisco)
                {
                    // reindexar= this->indiceDocs.find(ruta);
                    if(indiceDocs.find(ruta) != indiceDocs.end()) // Documento repetido
                    {
                        // Comprobamos fechas
                        if(fecha.moreRecentThan(indiceDocs[ruta].getFecha())) // El nuevo es mas reciente que el ya indexado
                        {
                            doc.setFecha(fecha);
                            doc.setId(indiceDocs[ruta].getId()); // Mantiene el id del anterior
                            this->indiceDocs.erase(ruta); // Eliminamos el que ya estaba indexado
                            IndexarFichero(ruta, doc, memory);
                            if(!memory) // Booleana que indica cuando se ha quedado sin memoria 
                                return false;
                        }
                    }
                    else
                    {
                        doc.setFecha(fecha);
                        doc.setId(); //Le asignamos la id correspondiente
                        IndexarFichero(ruta, doc, memory);
                        if(!memory) // Booleana que indica cuando se ha quedado sin memoria 
                            return false;
                    }

                }
                else
                {
                    // reindexarDisco= this->indiceDocsDisco.find(ruta);
                    if(indiceDocsDisco.find(ruta) != indiceDocsDisco.end()) // Documento repetido
                    {
                        string str= GetInfo(PATH2, indiceDocsDisco[ruta]);
                        InfDoc auxDoc= GetInfDoc(str);
                        if(fecha.moreRecentThan(auxDoc.getFecha())) // El nuevo es mas reciente que el ya indexado
                        {
                            doc.setFecha(fecha);
                            doc.setId(auxDoc.getId()); // Mantiene el id del anterior
                            ModifyLine(indiceDocsDisco[ruta], 0, "", PATH2); // Eliminamos sus datos del fichero PATH2
                            for(auto& termino : indiceDocsDisco)
                                if(termino.second > this->indiceDocsDisco[ruta])
                                    this->indiceDocsDisco[termino.first] -= 1; // Tenemos que restar uno a la linea en la que se encuentra la informaci�n
                            this->indiceDocsDisco.erase(ruta); // Eliminamos el que estaba indexado (erase by key)            
                            IndexarFichero(ruta, doc, memory);
                            if(!memory) // Booleana que indica cuando se ha quedado sin memoria 
                                return false;
                        }
                    }
                    else
                        doc.setFecha(fecha);
                        doc.setId(); //Le asignamos la id correspondiente
                        IndexarFichero(ruta, doc, memory);
                        if(!memory) // Booleana que indica cuando se ha quedado sin memoria 
                            return false;
                }
            }
        }
        return true;
    }
    else
    {
        cerr << "ERROR: No existe el archivo: " << ficheroDocumentos << '\n';
        return false;
    }
}

bool 
IndexadorHash::IndexarDirectorio(const string& dirAIndexar)
{
    struct stat dir;
    int err= stat(dirAIndexar.c_str(), &dir);
    // Comprobamos la existencia del directorio
    if(err == -1 || !S_ISDIR(dir.st_mode)) 
        return false;
    else
    {
        // Hago una lista en un fichero con find>fich
        string cmd="find ";
        cmd += dirAIndexar;
        cmd += " -type f -follow |sort > .lista_fich";
        system(cmd.c_str());
        return Indexar(".lista_fich");
    }
}

void
IndexadorHash::ExportarIndices() const
{
    ofstream indices("");
    indices.open(this->directorioIndice + "/indices.txt");
    //             \t      Comienza  list      \t                    \t
    // termino ftc   id ft pos pos pos pos pos    id2 ft2: pos2 pos2    ... \n
    if(!this->almacenarEnDisco)
    {
        for(auto& termino: this->indice)
        {
            indices << termino.first << ' ' << termino.second.getFtc() << '\t';
            for(auto& doc : termino.second.getDocs())
            {
                indices << doc.first << ' ' << doc.second.getFt();
                for(auto& pos : doc.second.getPosTerm())
                    indices << ' ' << pos;
                indices << '\t';
            }
            indices << '\n';
        }
    }
    else
    {
        string str;
        InformacionTermino inf;
        for(auto& termino : this->indiceDisco) // Respetamos el mismo formato que en el de arriba
        {
            str= GetInfo(PATH1, termino.second);
            inf= GetInfTerm(str, 0);
            indices << termino.first << ' ' << inf.getFtc() << '\t';
            for(auto& doc : inf.getDocs())
            {
                indices << doc.first << ' ' << doc.second.getFt();
                for(auto& pos : doc.second.getPosTerm())
                    indices << ' ' << pos;
                indices << '\t';
            }
        }
    }
    indices << "*\n";// Separa los indices
    // doc idDoc numPal numPalSinParada numPalDiferentes tamBytes   Fecha\n Fecha: dia mes anyo hora
    if(!this->almacenarEnDisco)
    {
        for(auto& doc: this->indiceDocs)
        {
            indices << doc.first << ' ' << doc.second.getId() << ' ' << doc.second.getNumPal() << ' ';
            indices << doc.second.getNumPalSinParada() << ' ' << doc.second.getNumPalDiferentes() << ' ' << doc.second.getTamBytes() << '\t';
            indices << doc.second.getFecha().getDay() << ' ' << doc.second.getFecha().getMonth() << ' ' << doc.second.getFecha().getYear() << ' ';
            indices << doc.second.getFecha().getHour() << '\n';
        }
    }
    else
    {
        string str;
        InfDoc inf;
        for(auto& doc : this->indiceDocsDisco) // Respetamos el mismo formato que en el de arriba
        {
            str= GetInfo(PATH2, doc.second);
            inf= GetInfDoc(str);
            indices << doc.first << ' ' << inf.getId() << ' ' << inf.getNumPal() << ' ';
            indices << inf.getNumPalSinParada() << ' ' << inf.getNumPalDiferentes() << ' ' << inf.getTamBytes() << '\t';
            indices << inf.getFecha().getDay() << ' ' << inf.getFecha().getMonth() << ' ' << inf.getFecha().getYear() << ' ';
            indices << inf.getFecha().getHour() << '\n';
        }
    }
    if(!this->pregunta.empty())
    {
        indices << "+\n"; // Este s?mbolo indica que si que hay pregunta indexada
        // termino ft pos pos pos pos \n
        for(auto& preg : this->indicePregunta)
        {
            indices << preg.first << ' ' << preg.second.getFt() << ' ';
            for(auto& inf : preg.second.getPosTerm())
            {
                indices << ' ' << inf;
            }
            indices << '\n';
        }
    }
    else
        indices << "-\n"; //Este s?mbolo indica que NO que hay pregunta indexada
    indices.close();
}

bool
IndexadorHash::GuardarIndexacion() const
{
    struct stat dir;
    int err= stat(this->directorioIndice.c_str(), &dir);
    // Comprobamos la existencia del directorio
    if(err == -1 || !S_ISDIR(dir.st_mode)) // Si no existe lo creamos
    {
        string cmd = "mkdir ";
        cmd += this->directorioIndice;
        system(cmd.c_str());
    }
    try
    {
        ExportarIndices();
        ofstream fSalida("");
        fSalida.open(this->directorioIndice + "/stopWords.indx");
        for(auto& stopword : this->stopWords)
            fSalida << stopword << '\n';
        fSalida.close();
        // Copiamos el fichero con las palabras de parada originales en el directorio
        string cmd= "cp " + this->ficheroStopWords + ' ' + this->directorioIndice + "/stopWords.txt";
        system(cmd.c_str());

        fSalida.open(this->directorioIndice + "/indexador.txt");
        // numDocs numTotalPal numTotalPalSinParada numTotalPalDiferentes tamBytes\n
        fSalida << this->informacionColeccionDocs.getNumDocs() << ' ' << this->informacionColeccionDocs.getNumTotalPal() << ' ';
        fSalida << this->informacionColeccionDocs.getNumTotalPalSinParada() << ' '<< this->informacionColeccionDocs.getNumPalTotalDiferentes() << ' ';
        fSalida << this-> informacionColeccionDocs.getTamBytes() << '\n';
        // delimitadores casosEspeciales minuscAcentos\n
        fSalida << this->tok.DelimitadoresPalabra() << ' ' << this->tok.CasosEspeciales() << ' ' << this->tok.PasarAminuscSinAcentos() << '\n';
        // directorioIndice tipoStemmer almacenarDisco almacenarPosTerm\n
        fSalida << this->directorioIndice << ' ' << this->tipoStemmer << ' ' << this->almacenarEnDisco << ' ' << this->almacenarPosTerm << '\n';
        // pregunta numTotalPal numTotalPalSinParada numTotalPalDiferentes
        if(!pregunta.empty())
            fSalida << pregunta << ' ' << this->infPregunta.getNumTotalPal() << ' ' << this->infPregunta.getNumTotalPalSinParada() << ' ' << this->infPregunta.getNumTotalPalDiferentes() << '\n';
        else
            fSalida << "-\n"; //Indica que no hay palabra indexada en el indexador guardado
        
        fSalida.close();
    }
    catch(const std::bad_alloc& e)
    {
        cerr << "ERROR: (GuardarIndexacion) Se ha alcanzado el limite de memoria.\n";
        return false;
    }
    return true;
}

void
IndexadorHash::Importar(const string& ruta)
{
    ifstream fEntrada(ruta);// Mostrar el fichero stopWords
    if(fEntrada)
    {
        string linea;
        int i= 0;
        vector<string> vec;
        while (getline(fEntrada, linea)) 
        {
            istringstream iss(linea);
            // Creamos un vector de string a partir de la linea
            copy(istream_iterator<string>(iss),
                istream_iterator<string>(),
                back_inserter(vec));
            bool minus, esp;
            switch(i) // ". ,: 0 0" Eliminar los 4 ultimos chars
            {
                case 0: // numDocs numTotalPal numTotalPalSinParada numTotalPalDiferentes tamBytes\n
                    this->informacionColeccionDocs= InfColeccionDocs(stol(vec.at(0)), stol(vec.at(1)), stol(vec.at(2)), stol(vec.at(3)), stol(vec.at(4))); 
                    break; 
                case 1: // delimitadores casosEspeciales minuscAcentos\n
                    minus= stoi(vec.back());
                    vec.pop_back();
                    esp= stoi(vec.back());
                    vec.pop_back(); // Ahora s?lo quedan delimitadores en vec
                    this->tok.DelimitadoresPalabra(linea.substr(0, linea.size() - 4));
                    this->tok.CasosEspeciales(esp);
                    this->tok.PasarAminuscSinAcentos(minus);
                    break;
                case 2: // directorioIndice tipoStemmer almacenarDisco almacenarPosTerm\n
                    this->directorioIndice= vec.at(0);
                    this->tipoStemmer= stoi(vec.at(1));
                    this->almacenarEnDisco= stoi(vec.at(2));
                    this->almacenarPosTerm= stoi(vec.at(3));
                    break;
                default: // pregunta numTotalPal numTotalPalSinParada numTotalPalDiferentes
                    if(linea != "-")
                    {
                        this->pregunta= vec.at(0);
                        this->infPregunta= InformacionPregunta(stol(vec.at(1)), stol(vec.at(2)), stol(vec.at(3)));
                    }
                    else
                    {
                        this->pregunta= "";
                        this->infPregunta.~InformacionPregunta();
                    }
                    break;
            }
            i++;
            vec.clear();
        }
    }
}

void
IndexadorHash::ImportarIndices(const string& ruta)
{
    ifstream fEntrada(ruta);//Mostrar el fichero stopWords
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
        // buffer = new char [length];
        buffer= (char *) malloc(sizeof(char)*length);
        // Leemos los datos como si fueran un bloque, todos a la vez
        fEntrada.read(buffer,length);

        string str(buffer, length);
        // delete[] buffer;// Liberamos memoria
        free(buffer);
        fEntrada.close();

        string::size_type inicio = str.find_first_not_of("\n\0", 0);
        string::size_type fin = str.find_first_of("\n\0", inicio);

        istringstream iss;
        string linea= str.substr(inicio, fin - inicio);
        vector<string> vec;
        string::size_type lastPos;
        string::size_type pos;
        while(linea != "*") // indice
        {
            //             \t      Comienza  list      \t                   \t
            // termino ftc   id ft pos pos pos pos pos    id2 ft2 pos2 pos2    ... \n
            vec.clear();
            // Reseteamos lastPos y pos para la nueva linea
            lastPos = linea.find_first_not_of('\t', 0);
            pos = linea.find_first_of('\t', lastPos);
            iss= istringstream(linea.substr(lastPos, pos - lastPos));// termino ftc
            copy(istream_iterator<string>(iss),
                istream_iterator<string>(),
                back_inserter(vec));

            InformacionTermino inf;
            string termino= vec.at(0);
            int ftc= (stoi(vec.at(1)));
            vec.clear();

            inf= GetInfTerm(linea, pos);
            inf.setFtc(ftc);

            if(!this->almacenarEnDisco)
                this->indice.insert({termino, inf}); //A�adimos el termino al indice
            else
                AddInfTerm(inf, termino);

            inicio= str.find_first_not_of("\n\0", fin);
            fin= str.find_first_of("\n\0", inicio);
            linea= str.substr(inicio, fin - inicio);
        }
        inicio= str.find_first_not_of("\n\0", fin); 
        fin= str.find_first_of("\n\0", inicio);
        linea= str.substr(inicio, fin - inicio); // Siguiente linea
        while(linea != "+" && linea != "-") // indice docs
        {
            // doc idDoc numPal numPalSinParada numPalDiferentes tamBytes\tFecha\n
            vec.clear();
            // Reseteamos lastPos y pos para la nueva linea
            lastPos = linea.find_first_not_of('\t', 0);
            pos = linea.find_first_of('\t', lastPos);
            iss= istringstream(linea.substr(lastPos, pos - lastPos));// doc idDoc numPal numPalSinParada numPalDiferentes tamBytes
            copy(istream_iterator<string>(iss),
                istream_iterator<string>(),
                back_inserter(vec));
            
            InfDoc inf(stoi(vec.at(1)), stoi(vec.at(2)), stoi(vec.at(3)), stoi(vec.at(4)), stoi(vec.at(5)));

            inf.setFecha(Fecha(linea.substr(pos, linea.size())));
            if(!this->almacenarEnDisco)
                this->indiceDocs.insert({vec.at(0), inf});
            else
            {
                ofstream file(PATH2, ios::app);
                if(file)
                {
                    // doc idDoc numPal numPalSinParada numPalDiferentes tamBytes   Fecha\n Fecha: dia mes anyo hora
                    file << ruta << ' ' << inf.getId() << ' ' << inf.getNumPal() << ' ' << inf.getNumPalSinParada() << ' ' << inf.getNumPalDiferentes() << ' ' << inf.getTamBytes() << '\t';
                    file << inf.getFecha().getDay() << ' ' << inf.getFecha().getMonth() << ' ' << inf.getFecha().getYear() << ' ';
                    file << inf.getFecha().getHour() << '\n';
                    file.close();
                }
                ifstream inFile(PATH2);
                // Obtenemos la linea en la que hemos escrito el nuevo termino con std::count
                this->indiceDocsDisco[ruta]= (count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n') - 1);
                inFile.close();
            }
            inicio= str.find_first_not_of("\n\0", fin);
            fin= str.find_first_of("\n\0", inicio);
            linea= str.substr(inicio, fin - inicio);
        }
        if(linea == "+")
        {
            inicio= str.find_first_not_of("\n\0", fin); 
            fin= str.find_first_of("\n\0", inicio);
            linea= str.substr(inicio, fin - inicio); // Siguiente linea
            while(inicio == string::npos || fin == string::npos) // indice pregunta
            {
                vec.clear();
                iss= istringstream(str.substr(lastPos, pos - lastPos));// termino ft pos pos pos pos \n
                copy(istream_iterator<string>(iss),
                    istream_iterator<string>(),
                    back_inserter(vec));

                string termino= vec.at(0);
                int ft= stoi(vec.at(1));
                vec.erase(vec.begin()); vec.erase(vec.begin());
                vector<int> posTerm;
                for(auto& pos : vec)
                    posTerm.push_back(stoi(pos));
                this->indicePregunta.insert({termino, InformacionTerminoPregunta(ft, posTerm)});
                
                inicio= str.find_first_not_of("\n\0", fin);
                fin= str.find_first_of("\n\0", inicio);
                linea= str.substr(inicio, fin - inicio);
            }
        }
        else
            this->indicePregunta.clear();
    }
}

void
IndexadorHash::ImportarStopWords(const string& ruta)
{
    ifstream fEntrada(ruta);//Mostrar el fichero stopWords
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
        // buffer = new char [length];
        buffer= (char *) malloc(sizeof(char)*length);
        // Leemos los datos como si fueran un bloque, todos a la vez
        fEntrada.read(buffer,length);

        string cadena(buffer, length);
        // delete[] buffer;// Liberamos memoria
        free(buffer);
        fEntrada.close();  
        istringstream iss(cadena);
        string word;
        while(iss >> word) {
            this->stopWords.insert(word); //a?adimos todas las palabras del fichero a la variable privada stopWords
        }
    }
}

void
IndexadorHash::importarIndexador(const string& ruta, const int& linea)
{
    switch(linea)
    {
        case 0: // ./indexador.txt
            Importar(ruta);
            break;
        case 1: // ./indices.txt
            ImportarIndices(ruta);
            break; 
        case 2: // ./stopWords.indx
            ImportarStopWords(ruta);
            break;
        case 3: // ./stopWords.txt
            this->ficheroStopWords= ruta;
            break;
        default:
            break;
    }
}

bool
IndexadorHash::RecuperarIndexacion (const string& directorioIndexacion)
{
    // Vaciamos la indexacion actual
    this->indice.clear();
    this->indiceDocs.clear();
    this->indicePregunta.clear();
    this->infPregunta.~InformacionPregunta();
    this->informacionColeccionDocs.~InfColeccionDocs();

    string cmd="find ";
    cmd += directorioIndexacion;
    cmd += " -type f -follow |sort > .indexador";
    system(cmd.c_str());
    ifstream lista("");
    lista.open(".indexador");
    if(lista)
    {
        string ruta;
        int i= 0;
        while(getline(lista, ruta))
        {
            importarIndexador(ruta, i);
            i++;
        }
        lista.close();
    }
    return false;
}

bool
IndexadorHash::IndexarPregunta(const string& preg)
{
    if(!preg.empty())
    {
        this->indicePregunta.clear();
        this->infPregunta.~InformacionPregunta();

        this->pregunta= preg;
        string pregAux= preg;
        InfDoc inf;
        if(!AddTerms("", ExtractInfo(pregAux, inf), inf)) // inf no se usa
            return false; // No queda memoria
        return true;
    }
    else
    {
        cerr << "ERROR: (En IndexarPregunta) no se puede indexar una cadena vac?a\n";
        return false;
    }
}

bool
IndexadorHash::DevuelvePregunta(string& preg) const
{
    if(!this->pregunta.empty())
    {
        if(!this->indicePregunta.empty())
        {
            preg= this->pregunta;
            return true;
        }
    }
    return false;
}

bool
IndexadorHash::DevuelvePregunta(const string& word, InformacionTerminoPregunta& inf) const
{
    stemmerPorter stem;
    string aux= word;
    if(this->DevolverPasarAminuscSinAcentos())
        this->tok.Filtrar(aux);
    stem.stemmer(aux, this->tipoStemmer); // Stemming a word
    if(!this->pregunta.empty())
    {
        unordered_map<string,InformacionTerminoPregunta>::const_iterator got= this->indicePregunta.find(aux); // Buscar en indice
        if(got != this->indicePregunta.end())
        {
            inf= got->second;
            return true;
        }
    }
    inf.~InformacionTerminoPregunta();
    return false;
}

bool
IndexadorHash::DevuelvePregunta(InformacionPregunta& inf) const
{
    if(!this->pregunta.empty())
    {
        inf= this->infPregunta;
        return true;
    }
    inf.~InformacionPregunta();
    return false;
}

void
IndexadorHash::ImprimirIndexacionPregunta() const
{

    cout << "Pregunta indexada: " << pregunta << endl;
    cout << "Terminos indexados en la pregunta: " << endl;
    // A continuaci?n aparecer? un listado del contenido de ?indicePregunta? donde para cada t?rmino indexado se imprimir?: 
    // cout << termino << ?\t? << InformacionTerminoPregunta << endl;
    if(!pregunta.empty())
    {
        for(auto& termino: this->indicePregunta)
            cout << termino.first << '\t' << termino.second << '\n';
    }
    cout << "Informacion de la pregunta: " << infPregunta << '\n';    
}

bool
IndexadorHash::Devuelve(const string& word, InformacionTermino& inf) const
{
    stemmerPorter stem;
    string aux= word;
    if(this->DevolverPasarAminuscSinAcentos())
        this->tok.Filtrar(aux);
    stem.stemmer(aux, this->tipoStemmer); // Stemming a word
    if(!this->almacenarEnDisco)
    {
        // unordered_map<string,InformacionTermino>::const_iterator got= this->indice.find(aux); // Buscar en indice
        if(this->indice.find(aux) != this->indice.end())
        {
            inf= this->indice.at(aux); //Guardamos la informacion del termino deseado en inf
            return true;
        }
    }
    else
    {
        unordered_map<string,long int>::const_iterator got= this->indiceDisco.find(aux); // Buscar en indice
        if(got != this->indiceDisco.end())
        {
            string str= GetInfo(PATH1, this->indiceDisco.at(aux)); // Obtenemos la linea con la informaci�n
            inf= GetInfTerm(str, 0);
            return true;
        }
    }
    // inf.~InformacionTermino();//Lo vaciamos porque no lo hemos encontrado
    return false;
}

bool
IndexadorHash::Devuelve(const string& word, const string& nomDoc, InfTermDoc& inf) const
{
    stemmerPorter stem;
    string aux= word;
    if(this->DevolverPasarAminuscSinAcentos())
        this->tok.Filtrar(aux);
    stem.stemmer(aux, this->tipoStemmer); // Stemming a word   
    if(!this->almacenarEnDisco)
    {
        unordered_map<string,InfDoc>::const_iterator got= this->indiceDocs.find(nomDoc);
        if(got != this->indiceDocs.end())// Documento indexado encontrado
        {
            // long int id= got->second.getId();// Guardamos el id del documento indexado
            long int id= this->indiceDocs.at(nomDoc).getId();// Guardamos el id del documento indexado
            // InformacionTermino tiene una l_docs, hace falta el if de arriba?
            unordered_map<string,InformacionTermino>::const_iterator palabra= this->indice.find(aux); // Buscar en indice
            if(palabra != this->indice.end())// Palabra indexada encontrada
            {
                unordered_map<long int, InfTermDoc>::const_iterator doc= palabra->second.getDocs().find(id);
                if(doc != palabra->second.getDocs().end())//Encontrado doc
                    inf= palabra->second.getDocs().at(id);
                else 
                    inf.~InfTermDoc();//Vaciamos al no cumplir las condiciones
                return true;
            }
            else
            {
                inf.~InfTermDoc();//Vaciamos al no cumplir las condiciones
                return false;
            }
        }
        else
        {
            inf.~InfTermDoc();//Vaciamos al no cumplir las condiciones
            return false;
        }
    }
    else
    {
        unordered_map<string,long int>::const_iterator got= this->indiceDocsDisco.find(nomDoc);
        if(got != this->indiceDocsDisco.end())// Documento indexado encontrado
        {
            string str= GetInfo(PATH2, indiceDocsDisco.at(nomDoc));
            InfDoc infDoc= GetInfDoc(str);
            unordered_map<string,long int>::const_iterator palabra= this->indiceDisco.find(aux); // Buscar en indice
            if(palabra != this->indiceDisco.end())// Palabra indexada encontrada
            {
                string s= GetInfo(PATH1, indiceDisco.at(aux));
                InformacionTermino infTerm= GetInfTerm(s, 0);
                unordered_map<long int, InfTermDoc>::const_iterator doc= infTerm.getDocs().find(infDoc.getId());
                if(doc != infTerm.getDocs().end())// Encontrado doc
                    inf= infTerm.getDocs().at(infDoc.getId());
                else 
                    inf.~InfTermDoc();//Vaciamos al no cumplir las condiciones
                return true;
            }
        }
        inf.~InfTermDoc();//Vaciamos al no cumplir las condiciones
        return false;
    }
}

bool
IndexadorHash::Existe(const string& word) const
{
    stemmerPorter stem;
    string aux= word;
    if(this->DevolverPasarAminuscSinAcentos())
        this->tok.Filtrar(aux);
    stem.stemmer(aux, this->tipoStemmer); // Stemming a word
    if(!this->almacenarEnDisco)
    {
        unordered_map<string,InformacionTermino>::const_iterator got= this->indice.find(aux); // Buscar en indice
        if(got != this->indice.end())
            return true;
    }
    else
    {
        unordered_map<string,long int>::const_iterator got= this->indiceDisco.find(aux); // Buscar en indice
        if(got != this->indiceDisco.end())
            return true;
    }
    return false;
}

bool
IndexadorHash::Borra(const string& word)
{
    stemmerPorter stem;
    string aux= word;
    if(this->DevolverPasarAminuscSinAcentos())
        this->tok.Filtrar(aux);
    stem.stemmer(aux, this->tipoStemmer); // Stemming a word
    if(!this->almacenarEnDisco)
    {
        unordered_map<string,InformacionTermino>::const_iterator got= this->indice.find(aux); // Buscar en indice
        if(got != this->indice.end())
        {
            this->indice.erase(aux);
            return true;
        }
    }
    else
    {
        unordered_map<string,long int>::const_iterator got= this->indiceDisco.find(aux); // Buscar en indice
        if(got != this->indiceDisco.end())
        {
            ModifyLine(this->indiceDisco.at(aux), 0, "", PATH1); //Eliminamos su contenido 
            for(auto& termino : indiceDisco) // TODO Probar
                if(termino.second > this->indiceDisco.at(aux))
                    this->indiceDisco[termino.first] -= 1; // Tenemos que restar uno a la linea en la que se encuentra la informaci�n
            this->indiceDisco.erase(aux);
            return true;
        }
    }
    return false;
}

bool
IndexadorHash::BorraDoc(const string& nomDoc)
{
    if(!this->almacenarEnDisco)
    {
        unordered_map<string,InfDoc>::const_iterator got= this->indiceDocs.find(nomDoc);
        if(got != this->indiceDocs.end())// Documento indexado encontrado
        {
            vector<string> vec;
            for(auto& termino : this->indice)
            {
                for(auto& doc : termino.second.getDocs())
                {
                    if(doc.first == got->second.getId())
                    {
                        this->indice[termino.first].setFtc(termino.second.getFtc() - termino.second.getDocs()[got->second.getId()].getFt());
                        termino.second.deleteDoc(got->second.getId());
                    }
                }
                if(termino.second.getDocs().empty()) //Si la l_docs del termino se queda vacia -> hay que eliminarlo
                    vec.push_back(termino.first);
            }
            for(auto& key: vec) // Bucle para eliminar terminos
                this->indice.erase(key);

            this->informacionColeccionDocs.deleteDoc(got->second);
            this->informacionColeccionDocs.setDiferentes(this->indice.size()); // Actualizamos diferentes
            this->indiceDocs.erase(nomDoc);
            return true;
        }
    }
    else
    {
        unordered_map<string,long int>::const_iterator got= this->indiceDocsDisco.find(nomDoc);
        if(got != this->indiceDocsDisco.end())// Documento indexado encontrado
        {
            string str, aux;
            // TODO Probar
            InformacionTermino inf;
            aux= GetInfo(PATH2, this->indiceDocsDisco.at(nomDoc));
            InfDoc infDoc= GetInfDoc(aux);
            vector<string> vec;
            for(auto& termino : this->indiceDisco)
            {
                str= GetInfo(PATH1, termino.second);
                inf= GetInfTerm(str, 0);
                for(auto& doc : inf.getDocs())
                {
                    if(doc.first == infDoc.getId())
                    {
                        inf.setFtc(inf.getFtc() - inf.getDocs()[infDoc.getId()].getFt());
                        inf.deleteDoc(infDoc.getId());
                        ModifyLine(termino.second, 0, "", PATH1);
                        for(auto& termino : indiceDisco) // TODO Probar
                            if(termino.second > this->indiceDisco.at(aux))
                                this->indiceDisco[termino.first] -= 1; // Tenemos que restar uno a la linea en la que se encuentra la informaci�n
                        if(!inf.getDocs().empty()) // Como ft != 0, hay que reescribir la informacion en el documento con la nueva
                            AddInfTerm(inf, aux); // A�ade y modifica el long int asociado a la linea del termino aux
                        else
                            this->indiceDisco.erase(aux);
                    }
                }
            }
            this->informacionColeccionDocs.deleteDoc(infDoc);
            this->informacionColeccionDocs.setDiferentes(this->indiceDocsDisco.size()); // Actualizamos diferentes
            ModifyLine(this->indiceDocsDisco[nomDoc], 0, "", PATH2); // Eliminamos el contenido del fichero
            for(auto& termino : indiceDocsDisco) // TODO Probar
                if(termino.second > this->indiceDocsDisco[nomDoc])
                    this->indiceDocsDisco[termino.first] -= 1; // Tenemos que restar uno a la linea en la que se encuentra la informaci�n

            this->indiceDocsDisco.erase(nomDoc);
            return true;
        }
    }
    return false;
}

bool
IndexadorHash::Actualiza(const string& word, const InformacionTermino& inf)
{
    stemmerPorter stem;
    string aux= word;
    if(this->DevolverPasarAminuscSinAcentos())
        this->tok.Filtrar(aux);
    stem.stemmer(aux, this->tipoStemmer); // Stemming a word
    if(!this->almacenarEnDisco)
    {
        unordered_map<string,InformacionTermino>::const_iterator got= this->indice.find(aux); // Buscar en indice
        if(got != this->indice.end())
        {
            this->indice.at(aux)= inf;
            return true;
        }
    }
    else
    {
        unordered_map<string,long int>::const_iterator got= this->indiceDisco.find(aux); // Buscar en indice
        if(got != this->indiceDisco.end())
        {
            ModifyLine(this->indiceDisco.at(aux), 0, "", PATH1); //Eliminamos la informaci�n antigua
            for(auto& termino : indiceDisco) // TODO Probar
                if(termino.second > this->indiceDisco.at(aux))
                    this->indiceDisco[termino.first] -= 1; // Tenemos que restar uno a la linea en la que se encuentra la informaci�n
            AddInfTerm(inf, aux);
            return true;
        }
    }
    return false;
}

bool
IndexadorHash::Inserta(const string& word, const InformacionTermino& inf)
{
    stemmerPorter stem;
    string aux= word;
    if(this->DevolverPasarAminuscSinAcentos())
        this->tok.Filtrar(aux);
    stem.stemmer(aux, this->tipoStemmer); // Stemming a word
    if(!this->almacenarEnDisco)
    {
        unordered_map<string,InformacionTermino>::const_iterator got= this->indice.find(aux); // Buscar en indice
        if(got == this->indice.end())// NO encontrado
        {
            this->indice[aux]= inf; //Insertamos sabiendo que no estaba previamente
            return true;
        }
    }
    else
    {
        unordered_map<string,long int>::const_iterator got= this->indiceDisco.find(aux); // Buscar en indice
        if(got == this->indiceDisco.end())// NO encontrado
            AddInfTerm(inf, aux);
    }
    return false;
}



//-------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------
// GETTERS Y SETTERS (Los que no estan aqui estan en el .h)
//-------------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------------



void
IndexadorHash::ListarPalParada() const 
{
    ifstream fEntrada(this->ficheroStopWords);//Mostrar el fichero stopWords
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
        // buffer = new char [length];
        buffer= (char *) malloc(sizeof(char)*length);
        // Leemos los datos como si fueran un bloque, todos a la vez
        fEntrada.read(buffer,length);

        string cadena= buffer;//Conversi?n a string
        if(cadena[length] != '\n')
            cadena += '\n'; //Nos aseguramos que el ?ltimo t?rmino siempre tenga un salto de linea
        cout << cadena;
        free(buffer);

        fEntrada.close();
    }
}

void
IndexadorHash::ListarInfColeccDocs() const
{
    cout << this->informacionColeccionDocs << '\n';
}

void
IndexadorHash::ListarTerminos() const
{
    if(!this->almacenarEnDisco)
        for (auto& termino: this->indice) 
            cout << termino.first << '\t' << termino.second << '\n';
    else
    {
        string str;
        InformacionTermino inf;
        for(auto& termino : this->indiceDisco)
        {
            str= GetInfo(PATH1, termino.second);
            inf= GetInfTerm(str, 0);
            cout << termino.first << '\t' << inf << '\n';
        }
    }
}

bool
IndexadorHash::ListarTerminos(const string& nomDoc) const
{
    if(!this->almacenarEnDisco)     
    {
        unordered_map<string,InfDoc>::const_iterator got= this->indiceDocs.find(nomDoc);
        if(got != this->indiceDocs.end())// Documento indexado encontrado
        {
            for (auto& termino: this->indice) 
            {
                // Bucamos en la l_docs de termino la ID del documento "nomDoc"
                unordered_map<long int, InfTermDoc>::const_iterator encontrado= termino.second.getDocs().find(got->second.getId());
                if(encontrado != termino.second.getDocs().end()) //El termino pertenece al documento
                    cout << termino.first << '\t' << termino.second << '\n';
            }
            return true;
        }
    }
    else
    {
        unordered_map<string,long int>::const_iterator got= this->indiceDocsDisco.find(nomDoc);
        if(got != this->indiceDocsDisco.end())// Documento indexado encontrado
        {
            string str;
            str= GetInfo(PATH2, this->indiceDocsDisco.at(nomDoc));
            InfDoc infDoc= GetInfDoc(str);
            InformacionTermino inf;
            for (auto& termino: this->indiceDisco) 
            {
                str= GetInfo(PATH1, termino.second);
                inf= GetInfTerm(str, 0);
                // Bucamos en la l_docs de termino la ID del documento "nomDoc"
                unordered_map<long int, InfTermDoc>::const_iterator encontrado= inf.getDocs().find(infDoc.getId());
                if(encontrado != inf.getDocs().end()) //El termino pertenece al documento
                    cout << termino.first << '\t' << inf << '\n';
            }
            return true;
        }
    }
    return false;
}

void
IndexadorHash::ListarDocs() const
{
    if(!this->almacenarEnDisco)
        for(auto& doc : this->indiceDocs)
            cout << doc.first << "\t" << doc.second << '\n';
    else
    {
        string str;
        InfDoc inf;
        for(auto& doc : this->indiceDocsDisco) //here
        {
            str= GetInfo(PATH2, doc.second);
            inf= GetInfDoc(str);
            cout << doc.first << "\t" << inf << '\n';
        }
        
    }
}

// Devuelve TRUE si nomDoc existe en la coleccion y muestra por pantalla el contenido del campo privado ?indiceDocs? 
// para el documento con nombre ?nomDoc?: cout << nomDoc << ?\t? << InfDoc << endl; . Si no existe no se muestra nada
bool
IndexadorHash::ListarDocs(const string& nomDoc) const
{
    if(!this->almacenarEnDisco)
    {
        unordered_map<string,InfDoc>::const_iterator got= this->indiceDocs.find(nomDoc);
        if(got != this->indiceDocs.end())// Documento indexado encontrado
        {
            cout << nomDoc << "\t" << got->second << '\n';
            return true;
        }
    }
    else
    {
        unordered_map<string,long int>::const_iterator got= this->indiceDocsDisco.find(nomDoc);
        if(got != this->indiceDocsDisco.end())// Documento indexado encontrado
        {
            string str= GetInfo(PATH2, indiceDocsDisco.at(nomDoc));
            InfDoc inf= GetInfDoc(str);
            cout << nomDoc << "\t" << inf << '\n';
            return true;
        }
    }
    return false;
}
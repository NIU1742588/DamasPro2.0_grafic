#include "tauler.hpp"

void Tauler::inicialitza(const string& nomFitxer)
{

    // Inicialitzar tot el tauler a caselles buides
    for (int fila = 0; fila < N_FILES; fila++)
    {
        for (int col = 0; col < N_COLUMNES; col++) 
        {
            Posicio pos(fila, col);  // Cada casella té la seva posició correcta
            
            m_tauler[fila][col] = Fitxa(COLOR_EMPTY, TIPUS_EMPTY, pos);
        }
    }

    ifstream fitxer(nomFitxer);
    if (fitxer.is_open())
    {
        char tipus;
        string posicioStr; 
        Fitxa fitxa;

        while (fitxer >> tipus >> posicioStr)
        {
            Fitxa fitxa = Fitxa().llegeixFitxa(tipus, posicioStr);
            int fila = fitxa.getPosicio().getFila();
            int columna = fitxa.getPosicio().getColumna();
            m_tauler[fila][columna] = fitxa;
        }

        fitxer.close();
    }
    else
    {
        for (int i = 0; i < N_FILES; ++i) {
            for (int j = 0; j < N_COLUMNES; ++j) {
                bool posicio_valida = (i % 2 == 0 && j % 2 != 0) || (i % 2 != 0 && j % 2 == 0);

                if (posicio_valida) 
                {
                    Posicio pos(i, j);

                    if (i < 3) 
                    {
                        m_tauler[i][j] = Fitxa(COLOR_NEGRE, TIPUS_NORMAL, pos);
                    }
                    else if (i > 4) 
                    {
                        m_tauler[i][j] = Fitxa(COLOR_BLANC, TIPUS_NORMAL, pos);
                    }
                }
            }
        }
    }
}

void Tauler::actualitzaMovimentsValids() 
{
    for (int fil = 0; fil < N_FILES; fil++) 
    {
        for (int col = 0; col < N_COLUMNES; col++) 
        {
            if (m_tauler[fil][col].getTipus() != TIPUS_EMPTY) 
            {
                m_tauler[fil][col].actualitzaMoviments(m_tauler);
            }
        }
    }
}

void Tauler::getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[]) 
{
    nPosicions = 0;
    int fila = origen.getFila();
    int col = origen.getColumna();

    if (fila >= 0 && fila < N_FILES && col >= 0 && col < N_COLUMNES) 
    {
        const Moviment& movs = m_tauler[fila][col].getMovsValids();
        nPosicions = movs.getNumMovs();
        for (int i = 0; i < nPosicions; i++) 
        {
            posicionsPossibles[i] = movs.getMoviment(i);
        }
    }
}

string Tauler::toString() const 
{
    string result;

    // Files del tauler (8 a 1)
    for (int fil = 0; fil < N_FILES; fil++) 
    {
        result += to_string(N_FILES - fil) + ": ";

        for (int col = 0; col < N_COLUMNES; col++) 
        {
            char simbol = '_';  
            const Fitxa& fitxa = m_tauler[fil][col];

            if (fitxa.getTipus() != TIPUS_EMPTY) 
            {
                if (fitxa.getTipus() == TIPUS_NORMAL) 
                {
                    simbol = (fitxa.getColor() == COLOR_BLANC) ? 'O' : 'X';
                }
                else 
                {
                    simbol = (fitxa.getColor() == COLOR_BLANC) ? 'D' : 'R';
                }
            }
            result += simbol;
            result += ' ';
        }
        result += '\n';
    }

    // Lletres de columnes (a h)
    result += "   ";
    for (int col = 0; col < N_COLUMNES; col++) 
    {
        result += ('a' + col);  
        result += ' ';
    }
    result += '\n';

    return result;
}


void Tauler::stringToPosicio(const string& posicio, int& fila, int& columna)
{} //No Necessaria

void Tauler::visualitza(const vector<Posicio>& movimentsValids) const {
    // Dibuixa els moviments vàlids primer
    for (const auto& pos : movimentsValids) {
        int x = POS_X_TAULER + CASELLA_INICIAL_X + pos.getColumna() * AMPLADA_CASELLA;
        int y = POS_Y_TAULER + CASELLA_INICIAL_Y + pos.getFila() * ALCADA_CASELLA;
        GraphicManager::getInstance()->drawSprite(GRAFIC_POSICIO_VALIDA, x, y);
    }

    // Dibuixa totes les fitxes
    for (int fila = 0; fila < N_FILES; fila++) {
        for (int col = 0; col < N_COLUMNES; col++) {
            const Fitxa& fitxa = m_tauler[fila][col];
            if (fitxa.getTipus() != TIPUS_EMPTY) {
                int x = POS_X_TAULER + CASELLA_INICIAL_X + col * AMPLADA_CASELLA;
                int y = POS_Y_TAULER + CASELLA_INICIAL_Y + fila * ALCADA_CASELLA;
                fitxa.visualitza(x, y);
            }
        }
    }
}


bool Tauler::mouFitxa(const Posicio& origen, const Posicio& desti)
{
    // Valida les posicions
    if (!origen.EsPosicioValida() || !desti.EsPosicioValida()) {
        cout << "Posició no vàlida: fora del tauler.\n";
        return false;
    }
    int filaOrigen = origen.getFila(), colOrigen = origen.getColumna();
    int filaDesti = desti.getFila(), colDesti = desti.getColumna();

    Fitxa& fitxaOrigen = m_tauler[filaOrigen][colOrigen];
    Fitxa& fitxaDesti = m_tauler[filaDesti][colDesti];

    // Origen ha de tenir fitxa i destí ha d'estar buit
    if (fitxaOrigen.getTipus() == TIPUS_EMPTY || fitxaDesti.getTipus() != TIPUS_EMPTY) {
        cout << "Moviment no vàlid: origen buit o destí ocupat.\n";
        return false;
    }

    // Recupera els moviments valids abans de moure la fitxa:
    //    - maxCap: Quantes captures té el millor moviment
    //    - capsTriades: Quantes captures fa la jugada escollida
    const Moviment& movsOrigen = fitxaOrigen.getMovsValids();
    int  maxCap = 0;
    bool movValid = false;
    vector<Posicio> capsTriades;

    for (int i = 0; i < movsOrigen.getNumMovs(); ++i) {
        int c_movs = (int)movsOrigen.getMovimentsCaptura(i).size();
        if (c_movs > maxCap) maxCap = c_movs;
        const Posicio& landing = movsOrigen.getMoviment(i);
        if (landing == desti) {
            movValid    = true;
            capsTriades = movsOrigen.getMovimentsCaptura(i);
        }
    }
    bool fitxaTeCaptura = !capsTriades.empty();
    if (!movValid) {
        cout << "Moviment no vàlid: no pot anar a " << desti.toString() << ".\n";
        return false;
    }

    // Comprovar si hi ha altres fitxes que podien capturar
    bool altresCapt = false;
    for (int row = 0; row < N_FILES && !altresCapt; ++row) {
        for (int col = 0; col < N_COLUMNES && !altresCapt; ++col) {
            const Fitxa& f2 = m_tauler[row][col];
            if (f2.getTipus() != TIPUS_EMPTY &&
                f2.getColor() == fitxaOrigen.getColor() &&
                !(row==filaOrigen && col==colOrigen) &&
                f2.getMovsValids().esCaptura())
            {
                altresCapt = true;
            }
        }
    }

    cout << fitxaOrigen.getPosicio().toString() << " -> " << fitxaDesti.getPosicio().toString() << endl;

    // Moure i eliminar captures inicials
    fitxaDesti = fitxaOrigen;
    fitxaDesti.setPosicio(desti);
    fitxaOrigen.eliminaFitxa();
    for (const Posicio& p : capsTriades) {
        if (m_tauler[p.getFila()][p.getColumna()].getTipus() != TIPUS_EMPTY)
            cout << "Eliminacio -> " << p.toString() << endl;

        m_tauler[p.getFila()][p.getColumna()].eliminaFitxa();
    }

    // Coronació si toca
    if (fitxaDesti.getTipus() == TIPUS_NORMAL &&
       ((fitxaDesti.getColor()==COLOR_BLANC && filaDesti==0) ||
        (fitxaDesti.getColor()==COLOR_NEGRE && filaDesti==N_FILES-1)))
    {
        fitxaDesti.coronacio();
        cout << "Coronacio -> " << fitxaDesti.getPosicio().toString() << endl;
    }

    // Lògica de bufar:
    //    Si podia capturar (maxCap>0) però:
    //      · no ho fa -> bufa
    //      · tampoc encadena més des del destí -> bufa
    //      · no tria la millor seqüència -> bufa
    //    o (altresCapt) -> bufa
    bool bufaFitxa = false;
    if (maxCap > 0) {
        // Pot caputrar i no ho fa
        if (!fitxaTeCaptura) {
            bufaFitxa = true;
        }
        else {
            // recalculem moviments un cop ja hem mogut per comprovar encadenament
            actualitzaMovimentsValids();
            const Moviment& movsD = m_tauler[filaDesti][colDesti].getMovsValids();
            // No encadena més del del destí
            if (movsD.esCaptura()) {
                bufaFitxa = true;
            }
            // no tria la millor secuencia
            else if ((int)capsTriades.size() < maxCap) {
                bufaFitxa = true;
            }
        }
    }
    else if (altresCapt) {
        bufaFitxa = true;
    }
    
    // Eliminar la peça corresponent
    if (bufaFitxa) {
        bool eliminada = false;
        for (int row = 0; row < N_FILES && !eliminada; ++row) {
            for (int col = 0; col < N_COLUMNES && !eliminada; ++col) {
                Fitxa& fitxa = m_tauler[row][col];
                if (fitxa.getTipus()!=TIPUS_EMPTY &&
                    fitxa.getColor()==fitxaDesti.getColor() &&
                    !(row==filaDesti && col==colDesti) &&
                    fitxa.getMovsValids().esCaptura())
                {
                    cout << "Bufada -> " << fitxa.getPosicio().toString() << endl;
                    fitxa.eliminaFitxa();
                    eliminada = true;
                }
            }
        }
        if (!eliminada && fitxaDesti.getTipus()==TIPUS_NORMAL) {
            cout << "Bufada -> " << fitxaOrigen.getPosicio().toString() << endl;
            fitxaDesti.eliminaFitxa();
        }
    }




    actualitzaMovimentsValids();

    return (int)capsTriades.size() >= maxCap;
}




bool Tauler::esCapturaMaxima(const Posicio& origen, const Posicio& desti) const
{

    int filaOrigen = origen.getFila();
    int colOrigen = origen.getColumna();

    const Fitxa& fitxa = m_tauler[filaOrigen][colOrigen];
	const Moviment& movs = fitxa.getMovsValids();

    // Buscar capturas en el movimiento actual
    int capturesActuals = 0;
    for (int i = 0; i < movs.getNumMovs(); i++) {
        if (movs.getMoviment(i) == desti) {
            capturesActuals = (int)movs.getMovimentsCaptura(i).size();
            break;
        }
    }

    // Troba el máxim nombre de captures possibles
    int maxCaptures = 0;
    for (int i = 0; i < movs.getNumMovs(); i++) {
        int numCaptures = (int)movs.getMovimentsCaptura(i).size();
        if (numCaptures > maxCaptures) {
            maxCaptures = numCaptures;
        }
    }

    return (capturesActuals >= maxCaptures);
}
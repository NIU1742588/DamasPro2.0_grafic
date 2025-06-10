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
    // Dibujar movimientos válidos primero (como fondo)
    for (const auto& pos : movimentsValids) {
        int x = POS_X_TAULER + CASELLA_INICIAL_X + pos.getColumna() * AMPLADA_CASELLA;
        int y = POS_Y_TAULER + CASELLA_INICIAL_Y + pos.getFila() * ALCADA_CASELLA;
        GraphicManager::getInstance()->drawSprite(GRAFIC_POSICIO_VALIDA, x, y);
    }

    // Dibujar todas las piezas
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

    // Comprova que origen o destí siguin posicions vàlides
    if (!origen.EsPosicioValida() || !desti.EsPosicioValida())
    {
		cout << "Posició no vàlida: origen o destí fora del tauler." << endl;
        return false;
    }

    int filaOrigen = origen.getFila();
    int colOrigen = origen.getColumna();
    int filaDesti = desti.getFila();
    int colDesti = desti.getColumna();

    Fitxa& fitxaOrigen = m_tauler[filaOrigen][colOrigen];
    Fitxa& fitxaDesti = m_tauler[filaDesti][colDesti];


    // Comprova que l'origen no estigui buit o que al destí hi hagi fitxa
    if (fitxaOrigen.getTipus() == TIPUS_EMPTY || fitxaDesti.getTipus() != TIPUS_EMPTY)
    {
		cout << "Moviment no vàlid: origen buit o destí ocupat." << endl;
        return false;
    }

    // Obtenció dels moviments vàlids de la fitxa
    const Moviment& movsValidsFitxa = fitxaOrigen.getMovsValids();


    bool movValid = false;
    bool fitxaTeCaptura = false;
    int indexMoviment = -1;
	vector<Posicio> movimentsCaptura;

    // Per cada moviment vàlid
    for (int i = 0; i < movsValidsFitxa.getNumMovs(); i++) {
        // Si el destí està dins els moviments vàlids
        // Marca que si es vàlid i hi ha captura
		const Posicio& mov = movsValidsFitxa.getMoviment(i);

		cout << "Comprovant moviment vàlid: " << mov.toString() << endl;

        if (mov.getFila() == filaDesti && mov.getColumna() == colDesti)
        {
            movValid = true;
			indexMoviment = i;  // Guarda l'índex del moviment vàlid

			movimentsCaptura = movsValidsFitxa.getMovimentsCaptura(i);
            fitxaTeCaptura = !movimentsCaptura.empty();
			break;  // No cal continuar comprovant altres moviments
        }
    }

    if (!movValid) 
    {
		cout << "Moviment no valid: la fitxa no pot moures a aquesta posicio." << endl;
        return false;
    }

    bool altresFitxesCaptura = false;
    for (int fil = 0; fil < N_FILES && !altresFitxesCaptura; fil++) {
        for (int col = 0; col < N_COLUMNES && !altresFitxesCaptura; col++) {
            
            if (m_tauler[fil][col].getTipus() != TIPUS_EMPTY &&
                m_tauler[fil][col].getColor() == fitxaOrigen.getColor() &&
                !(fil == filaOrigen && col == colOrigen))
            {

                const Moviment& movs = m_tauler[fil][col].getMovsValids();
                if (movs.esCaptura() && !fitxaTeCaptura) {
                    altresFitxesCaptura = true;
                }
            }
        }
    }


    fitxaDesti = fitxaOrigen;
	fitxaDesti.setPosicio(desti);
    fitxaOrigen.eliminaFitxa();

	cout << "Te captura: " << (fitxaTeCaptura ? "Sí" : "No") << endl;
    if (fitxaTeCaptura) {
        // Eliminar todas las fichas en las posiciones de captura
        for (const Posicio& posCaptura : movimentsCaptura)
        {
            if (posCaptura.EsPosicioValida())
            {
                int filaCaptura = posCaptura.getFila();
                int colCaptura = posCaptura.getColumna();

                m_tauler[filaCaptura][colCaptura].eliminaFitxa();
            }
        }
    }

    // En cas que la fitxa es trobi a l'extrem contrari, la transforma en dama
	if (fitxaDesti.getTipus() == TIPUS_NORMAL &&
        (fitxaDesti.getColor() == COLOR_BLANC && filaDesti == 0) ||
        (fitxaDesti.getColor() == COLOR_NEGRE && filaDesti == N_FILES - 1)) {
        fitxaDesti.coronacio();
    }

    bool bufaFitxa = false;

    if (movsValidsFitxa.esCaptura())
    {

        // La fitxa pot fer una captura pero el moviment no inclou captura
        if (!fitxaTeCaptura) {

		    cout << "Moviment vàlid però no captura: " << origen.toString() << " -> " << desti.toString() << endl;
            bufaFitxa = true;
        }
        // Hi ha caputres disponibles per altres fitxes i no és dama
        else if (!esCapturaMaxima(origen, desti)) {
            cout << "Hi ha captures disponibles per altres fitxes: " << origen.toString() << " -> " << desti.toString() << endl;
			bufaFitxa = true;
        }

    }
    // La fitxa por fer una captura pero no captura el nombre màxim de fitxes
    else if (altresFitxesCaptura)
    {
		cout << "Hi ha altres fitxes que poden capturar: " << origen.toString() << " -> " << desti.toString() << endl;
        bufaFitxa = true;
    }


    if (bufaFitxa) {
        bool fitxaEliminada = false;

        // Es recorre el taulell
        for (int fil = 0; fil < N_FILES && !fitxaEliminada; fil++) {
            for (int col = 0; col < N_COLUMNES && !fitxaEliminada; col++) {

                // Si hi ha fitxa, és del mateix color, no és la posició destí i té captura
                if (m_tauler[fil][col].getTipus() != TIPUS_EMPTY &&
                    m_tauler[fil][col].getColor() == fitxaDesti.getColor() &&
                    !(fil == filaDesti && col == colDesti) &&
                    m_tauler[fil][col].getMovsValids().esCaptura()) {

                    m_tauler[fil][col].eliminaFitxa();
                    fitxaEliminada = true;
                }
            }
        }

        // Si no ha bufat cap fitxa i la moguda no és dama
        if (!fitxaEliminada && fitxaDesti.getTipus() == TIPUS_NORMAL) {
            fitxaDesti.eliminaFitxa();
        }
    }

    // Torna a actualitzar tots els moviments
    actualitzaMovimentsValids();

    return true;
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
            capturesActuals = movs.getMovimentsCaptura(i).size();
            break;
        }
    }

    // Encontrar máximo de capturas posibles
    int maxCaptures = 0;
    for (int i = 0; i < movs.getNumMovs(); i++) {
        int numCaptures = movs.getMovimentsCaptura(i).size();
        if (numCaptures > maxCaptures) {
            maxCaptures = numCaptures;
        }
    }

    return (capturesActuals >= maxCaptures);
}
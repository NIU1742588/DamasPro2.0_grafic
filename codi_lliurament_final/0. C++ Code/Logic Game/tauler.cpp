#include "tauler.hpp"

void Tauler::inicialitza(const string& nomFitxer)
{

    // Inicialitzar tot el tauler a caselles buides
    for (int fila = 0; fila < N_FILES; fila++)
    {
        for (int col = 0; col < N_COLUMNES; col++) 
        {
            Posicio pos(fila, col);  // Cada casella t� la seva posici� correcta
            
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
    // Dibujar movimientos v�lidos primero (como fondo)
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

    // Comprova que origen o dest� siguin posicions v�lides
    if (!origen.EsPosicioValida() || !desti.EsPosicioValida())
    {
		cout << "Posici� no v�lida: origen o dest� fora del tauler." << endl;
        return false;
    }

    int filaOrigen = origen.getFila();
    int colOrigen = origen.getColumna();
    int filaDesti = desti.getFila();
    int colDesti = desti.getColumna();

    Fitxa& fitxaOrigen = m_tauler[filaOrigen][colOrigen];
    Fitxa& fitxaDesti = m_tauler[filaDesti][colDesti];


    // Comprova que l'origen no estigui buit o que al dest� hi hagi fitxa
    if (fitxaOrigen.getTipus() == TIPUS_EMPTY || fitxaDesti.getTipus() != TIPUS_EMPTY)
    {
		cout << "Moviment no v�lid: origen buit o dest� ocupat." << endl;
        return false;
    }

    // Obtenci� dels moviments v�lids de la fitxa
    const Moviment& movsValidsFitxa = fitxaOrigen.getMovsValids();

    bool movValid = false;
    bool fitxaTeCaptura = false;
    int indexMoviment = -1;

    // Per cada moviment v�lid
    for (int i = 0; i < movsValidsFitxa.getNumMovs(); i++) {
        // Si el dest� est� dins els moviments v�lids
        // Marca que si es v�lid i hi ha captura
		const Posicio& mov = movsValidsFitxa.getMoviment(i);

		cout << "Comprovant moviment v�lid: " << mov.toString() << endl;

        if (mov.getFila() == filaDesti && mov.getColumna() == colDesti)
        {
            movValid = true;
            fitxaTeCaptura = movsValidsFitxa.esCaptura();
			indexMoviment = i;  // Guarda l'�ndex del moviment v�lid
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

	cout << "Te captura: " << (fitxaTeCaptura ? "S�" : "No") << endl;
    if (fitxaTeCaptura) {

        int distFila = filaDesti - filaOrigen;
        int distCol = colDesti - colOrigen;

        // Calcular posici� de la fitxa capturada
        int filaCaptura = filaOrigen + (distFila > 0 ? 1 : -1);
        int colCaptura = colOrigen + (distCol > 0 ? 1 : -1);

        // Per a moviments llargs (dames)
        if (abs(distFila) > 2 || abs(distCol) > 2) {
            // Calcular direcci�
            int dirFila = (distFila > 0) ? 1 : -1;
            int dirCol = (distCol > 0) ? 1 : -1;

            // Rec�rrer diagonal eliminant fitxes enemigues
            int filaActual = filaOrigen + dirFila;
            int colActual = colOrigen + dirCol;

            while (filaActual != filaDesti && colActual != colDesti) {
                if (m_tauler[filaActual][colActual].getTipus() != TIPUS_EMPTY) {
                    m_tauler[filaActual][colActual].eliminaFitxa();
                }
                filaActual += dirFila;
                colActual += dirCol;
            }
        }
        else {  // Captura simple
            m_tauler[filaCaptura][colCaptura].eliminaFitxa();
        }
    }

    // En cas que la fitxa es trobi a l'extrem contrari, la transforma en dama
	if (fitxaDesti.getTipus() == TIPUS_NORMAL &&
        (fitxaDesti.getColor() == COLOR_BLANC && filaDesti == 0) ||
        (fitxaDesti.getColor() == COLOR_NEGRE && filaDesti == N_FILES - 1)) {
        fitxaDesti.coronacio();
    }

    bool bufaFitxa = false;

    // La fitxa pot fer una captura pero el moviment no inclou captura
    if (movsValidsFitxa.esCaptura() && !fitxaTeCaptura)
    {
		cout << "Moviment v�lid per� no captura: " << origen.toString() << " -> " << desti.toString() << endl;
        bufaFitxa = true;
    }
    // La fitxa por fer una captura pero no captura el nombre m�xim de fitxes
    else if (altresFitxesCaptura)
    {
		cout << "Hi ha altres fitxes que poden capturar: " << origen.toString() << " -> " << desti.toString() << endl;
        bufaFitxa = true;
    }
    // Hi ha caputres disponibles per altres fitxes i no �s dama
    else if (fitxaTeCaptura && !esCapturaMaxima(origen, desti))
    {
		cout << "Hi ha captures disponibles per altres fitxes: " << origen.toString() << " -> " << desti.toString() << endl;
        bufaFitxa = true;
    }


    if (bufaFitxa) {
        bool fitxaEliminada = false;

        // Es recorre el taulell
        for (int fil = 0; fil < N_FILES && !fitxaEliminada; fil++) {
            for (int col = 0; col < N_COLUMNES && !fitxaEliminada; col++) {

                // Si hi ha fitxa, �s del mateix color, no �s la posici� dest� i t� captura
                if (m_tauler[fil][col].getTipus() != TIPUS_EMPTY &&
                    m_tauler[fil][col].getColor() == fitxaDesti.getColor() &&
                    !(fil == filaDesti && col == colDesti) &&
                    m_tauler[fil][col].getMovsValids().esCaptura()) {

                    m_tauler[fil][col].eliminaFitxa();
                    fitxaEliminada = true;
                }
            }
        }

        // Si no ha bufat cap fitxa i la moguda no �s dama
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
    int filaDesti = desti.getFila();
    int colDesti = desti.getColumna();

    const Fitxa& fitxa = m_tauler[origen.getFila()][origen.getColumna()];
	const Moviment& movs = fitxa.getMovsValids();

    // Clacul de la dist�ncia
    int capturesActuals = 0;
    int distFila = filaDesti - filaOrigen;
    int distCol = colDesti - colOrigen;
    //int dirFila = (distFila > 0) ? 1 : -1;
    //int dirCol = (distCol > 0) ? 1 : -1;
    int dirFila = (distFila != 0) ? (distFila > 0 ? 1 : -1) : 0;
    int dirCol = (distCol != 0) ? (distCol > 0 ? 1 : -1) : 0;

    // Solo procesar si es movimiento diagonal v�lido
    if (abs(distFila) != abs(distCol)) return false;

    // Calcula la quantitat de captures que fa en el moviment original
    int filaActual = filaOrigen + dirFila;
    int colActual = colOrigen + dirCol;

    // Es recorre la diagonal
    while ((filaActual != filaDesti || colActual != colDesti) &&
        filaActual >= 0 && filaActual < N_FILES &&
        colActual >= 0 && colActual < N_COLUMNES)
    {
        // Si la cela no est� buida
        if (m_tauler[filaActual][colActual].getTipus() != TIPUS_EMPTY &&
            m_tauler[filaActual][colActual].getColor() != fitxa.getColor()) 
        {
			capturesActuals++;
        }

        filaActual += dirFila;
        colActual += dirCol;
    }

    // Trobar el m�xim de captures possibles
    int maxCaptures = 0;
    for (int i = 0; i < movs.getNumMovs(); i++) {
        const Posicio& destiPossible = movs.getMoviment(i);

        int distFilaPossible = destiPossible.getFila() - filaOrigen;
        int distColPossible = destiPossible.getColumna() - colOrigen;
        int dirFilaPossible = (distFilaPossible != 0) ? (distFilaPossible > 0 ? 1 : -1) : 0;
        int dirColPossible = (distColPossible != 0) ? (distColPossible > 0 ? 1 : -1) : 0;

        // Solo procesar movimientos diagonales v�lidos
        if (abs(distFilaPossible) != abs(distColPossible)) continue;

        int capturesPossibles = 0;
        int filaActualPossible = filaOrigen + dirFilaPossible;
        int colActualPossible = colOrigen + dirColPossible;

        // Verificar l�mites en cada iteraci�n
        while ((filaActualPossible != destiPossible.getFila() ||
            colActualPossible != destiPossible.getColumna()) &&
            filaActualPossible >= 0 && filaActualPossible < N_FILES &&
            colActualPossible >= 0 && colActualPossible < N_COLUMNES)
        {
            if (m_tauler[filaActualPossible][colActualPossible].getTipus() != TIPUS_EMPTY &&
                m_tauler[filaActualPossible][colActualPossible].getColor() != fitxa.getColor() &&
                (filaActualPossible != filaDesti || colActualPossible != colDesti))
            {
                capturesPossibles++;
            }
            filaActualPossible += dirFilaPossible;
            colActualPossible += dirColPossible;
        }

        if (capturesPossibles > maxCaptures) {
            maxCaptures = capturesPossibles;
        }
    }

    return (capturesActuals >= maxCaptures);
}
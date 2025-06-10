#include "fitxa.hpp"

Fitxa Fitxa::llegeixFitxa(char& tipus, const string& posicioStr)
{
    Posicio posicio(posicioStr);

    // Crear la fitxa corresponent
    ColorFitxa color;
    TipusFitxa tipusFitxa;

    switch (tipus)
    {
    case 'O':
        color = COLOR_BLANC;
        tipusFitxa = TIPUS_NORMAL;
        break;
    case 'X':
        color = COLOR_NEGRE;
        tipusFitxa = TIPUS_NORMAL;
        break;
    case 'D':
        color = COLOR_BLANC;
        tipusFitxa = TIPUS_DAMA;
        break;
    case 'R':
        color = COLOR_NEGRE;
        tipusFitxa = TIPUS_DAMA;
        break;
    default:
        color = COLOR_BLANC;
        tipusFitxa = TIPUS_EMPTY;
        break;
    }

    return Fitxa(color, tipusFitxa, posicio);
}


void escriureFitxa(const string& nomFitxer, char tipusFitxa, const Posicio& posicio) 
{
	ofstream fitxer(nomFitxer);
	fitxer << tipusFitxa << ' ';
	fitxer << posicio;
	fitxer.close();
}

void Fitxa::actualitzaMoviments(const Fitxa tauler[][N_COLUMNES]) 
{

    m_movsValids.clear();

    // Moviment de les fitxes normals
    if (m_tipus == TIPUS_NORMAL) 
    {

        int dirFil = (m_color == COLOR_BLANC) ? -1 : 1; // Blanc (Amunt) Negre (Avall)

        // Moviemnts simples
        for (int dirCol : {-1, 1})
        {
            int novaFil = m_pos.getFila() + dirFil;
            int novaCol = m_pos.getColumna() + dirCol;

            // Si la posició és vàlida i està la cela buida
            // Afegeix el moviment
            if (Posicio(novaFil, novaCol).EsPosicioValida() && tauler[novaFil][novaCol].getTipus() == TIPUS_EMPTY && !m_movsValids.contePosicio(Posicio(novaFil, novaCol)))
            {
                m_movsValids.afegeixMoviment(Posicio(novaFil, novaCol));
            }
            
        }

        bool potContinuar = true;
        int filInicial = m_pos.getFila();
        int colInicial = m_pos.getColumna();

        vector<Posicio> posActuals = { m_pos };      // Posicions en aquesta iteració
        vector<Posicio> novesPosicions;  // Posicions per a la següents iteració
        vector<Posicio> posVisitades = { m_pos };    // Posicions ja processades 


        while (potContinuar && !posActuals.empty()) 
        {
            potContinuar = false;

            for (const Posicio& posActual : posActuals) 
            {
                int filActual = posActual.getFila(); 
                int colActual = posActual.getColumna();

                //for (int dirFil : {-1, 1}) 
                //{
                    for (int dirCol : {-1, 1}) 
                    {
                        int filInter = filActual + dirFil;
                        int colInter = colActual + dirCol;
                        int filFinal = filInter + dirFil;
                        int colFinal = colInter + dirCol;

                        if (!Posicio(filInter, colInter).EsPosicioValida()) continue; 
                        if (!Posicio(filFinal, colFinal).EsPosicioValida()) continue;

                        if (tauler[filInter][colInter].getTipus() != TIPUS_EMPTY &&
                            tauler[filInter][colInter].getColor() != m_color &&
                            tauler[filFinal][colFinal].getTipus() == TIPUS_EMPTY)
                        {
                            Posicio novaPos(filFinal, colFinal);


                            // Verifiquem si la nova posició ja ha estat visitada
                            bool jaVisitada = false;
                            int j = 0;

							// Busquem si la nova posició ja ha estat visitada
                            while (j < posVisitades.size() && !jaVisitada)
                            {
                                jaVisitada = (posVisitades[j] == novaPos);
                                j++;
                            }

							// Si no ha estat visitada, afegim el moviment
                            if (!jaVisitada) 
                            {

								Posicio captura = tauler[filInter][colInter].getPosicio();
                                

                               /* if (tauler[captura.getFila()][captura.getColumna()].getTipus() != TIPUS_EMPTY) {

									cout << "Error: Captura no valida: " << captura.toString() << endl;
                                    continue;

                                }*/
                                    m_movsValids.afegeixMoviment(novaPos);
                                    m_movsValids.marcaComCaptura();

                                    if (novesPosicions.size() < 20) {
                                
								        // Afegim la nova posició a les noves posicions
                                        novesPosicions.push_back(novaPos);

                                    }
                                    posVisitades.push_back(novaPos); // Marquem com a visitada
                                    potContinuar = true;

                            }
                        }
                    }
                //}
            }
            
            posActuals = move(novesPosicions);
        }

        vector<Posicio> movimentsOk;
        Posicio previa(-1, -1);  // Posición previa inválida


		//cout << "Moviments possibles: " << m_movsValids.getMoviments().size() << endl;
		for (const Posicio& pos : m_movsValids.getMoviments())
		{
			TipusFitxa tipus = tauler[pos.getFila()][pos.getColumna()].getTipus();

            // Elimina posicions on ja hi ha peces
            if (tipus != TIPUS_EMPTY ) {
                m_movsValids.eliminaMoviment(pos); // Elimina moviments que no són vàlids
            }

		}


    }
    // Movimients de les dames
    else if (m_tipus == TIPUS_DAMA) 
    {
        vector<Posicio> posActuals = { m_pos };      // Posicions en aquesta iteració
        vector<Posicio> novesPosicions;  // Posicions per a la següents iteració
        vector<Posicio> posVisitades = { m_pos };    // Posicions ja processades 
        vector<Posicio> movimentsSimples;

        bool hiHaCaptures = false;

        // Recorre les diagonals
        for (int dirFil : {-1, 1}) 
        {
            for (int dirCol : {-1, 1}) 
            {
                int distancia = 1;
                bool trobadaFitxaOponent = false;
                int filInter = -1, colInter = -1;

                // Fins que trobi una fitxa
                while (!trobadaFitxaOponent && Posicio(m_pos.getFila() + dirFil * distancia, 
                    m_pos.getColumna() + dirCol * distancia).EsPosicioValida())
                {
                    int novaFil = m_pos.getFila() + dirFil * distancia;
                    int novaCol = m_pos.getColumna() + dirCol * distancia;

                    if (!Posicio(novaFil, novaCol).EsPosicioValida()) break;

                    // Si troba una fitxa i és de tipus diferent a buit
                    if (tauler[novaFil][novaCol].getTipus() != TIPUS_EMPTY) 
                    {
                        // Si la fitxa és de color diferent i encara no s'havia trobat cap oponent
                        if (tauler[novaFil][novaCol].getColor() != m_color && !trobadaFitxaOponent) 
                        {
                            trobadaFitxaOponent = true;
                            filInter = novaFil;
                            colInter = novaCol;
                        }
                    }
                    // Si no s'ha trobat cap fitxa oponent, s'afegeix la posició com a moviment vàlid
                    else if (!trobadaFitxaOponent) 
                    {
						movimentsSimples.push_back(Posicio(novaFil, novaCol));
                        //m_movsValids.afegeixMoviment(Posicio(novaFil, novaCol));
                    }

                    distancia++;
                }

                // Si es troba una fitxa
                // Actualitza la posició final
                if (trobadaFitxaOponent) 
                {
                    int filFinal = filInter + dirFil;
                    int colFinal = colInter + dirCol;

                    // Si la posició és vàlida i està buida
                    // Indica que hi ha una captura
                    if (Posicio(filFinal, colFinal).EsPosicioValida() && tauler[filFinal][colFinal].getTipus() == TIPUS_EMPTY)
                        hiHaCaptures = true;
                }
            }
        }

        // Si hi ha captures
        if (hiHaCaptures) 
        {

            // Mentre hi hagi posicions
            while (!posActuals.empty()) 
            {

                novesPosicions.clear();
                                
                // Per cada posicio
                //for (const Posicio posActual : posActuals)
                for (size_t i = 0; i < posActuals.size(); i++)
                {
                    const Posicio& posActual = posActuals[i];

                    for (int dirFil : {-1, 1}) 
                    {
                        for (int dirCol : {-1, 1}) 
                        {
                            int distancia = 1;
                            bool trobadaFitxaOponent = false;
                            int filInter = -1, colInter = -1;

                            // Fins que trobi una fitxa
                            while (!trobadaFitxaOponent && Posicio(posActual.getFila() + dirFil * distancia,
                                posActual.getColumna() + dirCol * distancia).EsPosicioValida())
                            {
                                int filTemp = posActual.getFila() + dirFil * distancia;
                                int colTemp = posActual.getColumna() + dirCol * distancia;

                                // Si troba una fitxa i no està buida
                                //if (tauler[filTemp][colTemp].getTipus() != TIPUS_EMPTY) 
                                if (tauler[filTemp][colTemp].getTipus() != TIPUS_EMPTY) 
                                {
                                    // Si la fitxa és de color diferent i encara no s'ha trobat cap oponent
                                    if (tauler[filTemp][colTemp].getColor() != m_color && !trobadaFitxaOponent) 
                                    {
                                        trobadaFitxaOponent = true;
                                        filInter = filTemp;
                                        colInter = colTemp;
                                    }
                                }
                                distancia++;
                            }

                            // Si es troba una captura
                            if (trobadaFitxaOponent) 
                            {
                                int filFinal = filInter + dirFil;
                                int colFinal = colInter + dirCol;

                                if (Posicio(filFinal, colFinal).EsPosicioValida() && tauler[filFinal][colFinal].getTipus() == TIPUS_EMPTY) 
                                {

                                    Posicio novaPos(filFinal, colFinal);
                                    bool jaVisitada = false;

                                    // Es verifica si ja està visitada
                                    for (int j = 0; j < posVisitades.size(); j++)
                                    {
                                        if (posVisitades[j] == novaPos && !jaVisitada)
                                            jaVisitada = true;
                                    }

                                    // Si no està visitada
                                    // Afegeix el moviment, marca la caputra, l'afegeix a noves posicions i a posicions visitades
                                    if (!jaVisitada) 
                                    {
                                        bool afegida = false;

                                        // Es recorre tots els moviments ja afegits
                                        for (int i = 0; i < m_movsValids.getNumMovs(); i++)
                                        {
                                            // Si el moviment ja es troba dins
                                            if (m_movsValids.getMoviment(i) == novaPos)
                                                afegida = true;
                                        }

                                        // Si no ha estat afegit
                                        if (!afegida) 
                                        {
                                            m_movsValids.afegeixMoviment(novaPos);
                                            m_movsValids.marcaComCaptura();
                                        }

										novesPosicions.push_back(novaPos);
										posVisitades.push_back(novaPos);
                                    }
                                }
                            }
                        }
                    }
                }

                // Per cada una de les noves posicions s'afegeix a les actuals
                posActuals = move(novesPosicions); // Intercanvia amb les noves posicions
            }
        }
        else
        {
			// Si no hi ha captures, s'afegeixen els moviments simples
			for (const Posicio& pos : movimentsSimples)
			{
				m_movsValids.afegeixMoviment(pos);
			}
        }

    }


}

void Fitxa::visualitza(int posX, int posY) const {

    IMAGE_NAME sprite = GRAFIC_NUM_MAX;

    switch (m_tipus) {
        case TIPUS_NORMAL:
            sprite = (m_color == COLOR_BLANC) ? GRAFIC_FITXA_BLANCA : GRAFIC_FITXA_NEGRA;
            break;
        case TIPUS_DAMA:
            sprite = (m_color == COLOR_BLANC) ? GRAFIC_DAMA_BLANCA : GRAFIC_DAMA_NEGRA;
            break;
    default:
        break;
    }

    GraphicManager::getInstance()->drawSprite(sprite, posX, posY);
}
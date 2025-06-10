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

        struct State
        {
            Posicio pos;
            vector<Posicio> captures;
        };

        vector<State> posActuals = { {m_pos, {}} };      // Posicions en aquesta iteració
        vector<State> novesPosicions;  // Posicions per a la següents iteració
        vector<Posicio> posVisitades = { m_pos };    // Posicions ja processades 

        bool potContinuar = true;
        while (potContinuar && !posActuals.empty())
        {
            potContinuar = false;
            novesPosicions.clear(); // Reinicialitza les noves posicions per a la següent iteració

            for (const State& estat : posActuals)
            {
                int filActual = estat.pos.getFila();
                int colActual = estat.pos.getColumna();

                for (int dirCol : {-1, 1})
                {
                    int filInter = filActual + dirFil;
                    int colInter = colActual + dirCol;
                    int filFinal = filInter + dirFil;
                    int colFinal = colInter + dirCol;

                    if (!Posicio(filInter, colInter).EsPosicioValida()) continue;
                    if (!Posicio(filFinal, colFinal).EsPosicioValida()) continue;


                    // Comprova si la peca intermitja es capturable
                    bool jaCapturat = false;
                    for (const Posicio& captura : estat.captures) {
                        if (captura == Posicio(filInter, colInter)) {
                            jaCapturat = true;
                            break;
                        }
                    }

                    if (!jaCapturat &&
                        tauler[filInter][colInter].getTipus() != TIPUS_EMPTY &&
                        tauler[filInter][colInter].getColor() != m_color &&
                        tauler[filFinal][colFinal].getTipus() == TIPUS_EMPTY)
                    {
                        Posicio novaPos(filFinal, colFinal);
                        bool jaVisitada = false;

                        for (const Posicio& visitada : posVisitades) {
                            if (visitada == novaPos) {
                                jaVisitada = true;
                                break;
                            }
                        }

                        if (!jaVisitada)
                        {
                            vector<Posicio> novesCaptures = estat.captures;
                            novesCaptures.push_back(Posicio(filInter, colInter));

                            // Registrar movimiento con capturas
                            m_movsValids.afegeixMoviment(novaPos, novesCaptures);
                            m_movsValids.marcaComCaptura();

                            novesPosicions.push_back({ novaPos, novesCaptures });
                            posVisitades.push_back(novaPos);
                            potContinuar = true;
                        }
                    }
                }
            }

            posActuals = move(novesPosicions);
        }

        vector<Posicio> movimentsOk;
        Posicio previa(-1, -1);  // Posición previa inválida

    }
    // Movimients de les dames
    else if (m_tipus == TIPUS_DAMA)
    {

        struct State {
            Posicio pos;
            vector<Posicio> captures;
            vector<Posicio> visited;
        };

        vector<State> posActuals = { {m_pos, {}, {m_pos}} };      // Posicions en aquesta iteració
        vector<State> novesPosicions;  // Posicions per a la següents iteració
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
                Posicio posInter;

                // Fins que trobi una fitxa
                while (true) {
                    int filTemp = m_pos.getFila() + dirFil * distancia;
                    int colTemp = m_pos.getColumna() + dirCol * distancia;
                    Posicio posActual(filTemp, colTemp);

                    if (!posActual.EsPosicioValida()) break;

                    // Si encontramos una ficha
                    if (tauler[filTemp][colTemp].getTipus() != TIPUS_EMPTY) {
                        // Si es del oponente y no hemos encontrado aún una ficha en esta dirección
                        if (tauler[filTemp][colTemp].getColor() != m_color && !trobadaFitxaOponent) {
                            trobadaFitxaOponent = true;
                            posInter = posActual;
                        }
                        else {
                            // Si ya encontramos una ficha y hay otra, rompemos
                            break;
                        }
                    }
                    else {
                        // Si hay espacio vacío después de una ficha oponente, hay captura posible
                        if (trobadaFitxaOponent) {
                            hiHaCaptures = true;
                            break;
                        }
                        else {
                            // Si no hay capturas, registramos movimiento simple
                            movimentsSimples.push_back(posActual);
                        }
                    }
                    distancia++;
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
                for (const State& estat: posActuals)
                {
                    for (int dirFil : {-1, 1})
                    {
                        for (int dirCol : {-1, 1})
                        {
                            int distancia = 1;
                            bool trobadaFitxaOponent = false;
                            Posicio posInter;
                            vector<Posicio> capturesInDirection;

                            // Fins que trobi una fitxa
                            while (true) {
                                int filTemp = estat.pos.getFila() + dirFil * distancia;
                                int colTemp = estat.pos.getColumna() + dirCol * distancia;
                                Posicio posActual(filTemp, colTemp);

                                if (!posActual.EsPosicioValida()) break;

                                // Si la casilla no está vacía
                                if (tauler[filTemp][colTemp].getTipus() != TIPUS_EMPTY) {
                                    // Verificamos si es capturable
                                    if (tauler[filTemp][colTemp].getColor() != m_color && !trobadaFitxaOponent) {
                                        // Comprobamos que no haya sido capturada ya en este camino
                                        bool jaCapturat = false;
                                        for (const Posicio& captura : estat.captures) {
                                            if (captura == posActual) {
                                                jaCapturat = true;
                                                break;
                                            }
                                        }

                                        if (!jaCapturat) {
                                            trobadaFitxaOponent = true;
                                            posInter = posActual;
                                            capturesInDirection.push_back(posInter);
                                        }
                                        else {
                                            // Ya capturada, pero seguimos buscando más allá
                                        }
                                    }
                                    else {
                                        // Es una ficha del mismo color o ya encontramos una ficha oponente en esta dirección
                                        break;
                                    }
                                }
                                else {
                                    // Casilla vacía
                                    if (trobadaFitxaOponent) {
                                        // Este es un movimiento de captura válido
                                        // Comprobar que no hemos visitado esta posición en el mismo camino
                                        bool jaVisitada = false;
                                        for (const Posicio& visitada : posVisitades) {
                                            if (visitada == posActual) {
                                                jaVisitada = true;
                                                break;
                                            }
                                        }

                                        if (!jaVisitada) {
                                            // Añadimos las capturas de este salto
                                            vector<Posicio> novesCaptures = estat.captures;
                                            novesCaptures.insert(novesCaptures.end(), capturesInDirection.begin(), capturesInDirection.end());

                                            // Añadimos el movimiento
                                            m_movsValids.afegeixMoviment(posActual, novesCaptures);
                                            m_movsValids.marcaComCaptura();

                                            // Preparamos el nuevo estado para continuar saltando
                                            State nouEstat;
                                            nouEstat.pos = posActual;
                                            nouEstat.captures = novesCaptures;
                                            novesPosicions.push_back(nouEstat);
                                            posVisitades.push_back(posActual);
                                        }
                                    }
                                }
                                
                                distancia++;
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

        // Eliminar posiciones donde ya hay piezas (solo para movimientos simples)
        if (!hiHaCaptures) {
            for (int i = m_movsValids.getNumMovs() - 1; i >= 0; i--) {
                Posicio pos = m_movsValids.getMoviment(i);
                if (tauler[pos.getFila()][pos.getColumna()].getTipus() != TIPUS_EMPTY) {
                    m_movsValids.eliminaMoviment(pos);
                }
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
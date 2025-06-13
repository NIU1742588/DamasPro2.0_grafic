//
//  CurrentGame.cpp
//  LearnChess
//
//  Created by Enric Vergara on 21/2/22.
//

#include "joc.hpp"
#include <iostream>
#include <fstream>
#include "GraphicManager.h"

using namespace std;

Joc::Joc()
	: m_mode(MODE_JOC_NONE), m_tornActual(C_BLANC), m_fiPartida(false),
	m_guanyador(NO_COLOR), m_teSeleccio(false), m_finalitzaCridat(false) {
}
/**/
void Joc::inicialitza(ModeJoc mode, const string& nomFitxerTauler, const string& nomFitxerMoviments)
{
	/*
	▪ Inicialitza el tauler llegint les posicions inicials de les fitxes del fitxer indicat al paràmetre nomFitxerTauler.
	▪ Si el mode és MODE_JOC_NORMAL :
		▪ El fitxer indicat al paràmetre nomFitxerMoviments s’haurà d’utilitzar per guardar, al final de la partida, tots
		els moviments que s’hagin fet durant el desenvolupament de la partida.
	▪ Si el mode és MODE_JOC_REPLAY :
		▪ S’haurà d’inicialitzar la cua de moviments a reproduir amb la informació del fitxer indicat al paràmetre
		nomFitxerMoviments
	*/

	m_mode = mode;
    cout << "Mode: " << m_mode << endl;
	carregaTaulerInicial(nomFitxerTauler);

    if (mode == MODE_JOC_NORMAL) {
        m_cuaMoviments.esbuida();  // Netejar cua existent 
        m_nomFitxerMoviments = nomFitxerMoviments;  // Guardar el nom per finalitzar
        m_esperantClickReplay = false;
    }
    else if (mode == MODE_JOC_REPLAY) {
        m_cuaMoviments.carregaDeFitxer(nomFitxerMoviments);
        m_esperantClickReplay = true;

    }
}

bool Joc::actualitza(int mousePosX, int mousePosY, bool mouseStatus)
{
    // Si la partida ha acabat, mostrar resultat y sortir
    if (m_fiPartida) {
        GraphicManager::getInstance()->drawSprite(GRAFIC_FONS, 0, 0);

        string guanyador = (m_guanyador == C_BLANC) ? "BLANQUES" : "NEGRES";

        GraphicManager::getInstance()->drawFont(FONT_WHITE_30, (TAMANY_PANTALLA_X-500) / 2, 100, 1.25, "GUANYEN LES " + guanyador + "!!");

        GraphicManager::getInstance()->drawSprite(GRAFIC_POTATO_GOOD, (TAMANY_PANTALLA_X - 450) / 2, TAMANY_PANTALLA_Y - 500);

        GraphicManager::getInstance()->drawFont(FONT_RED_30, 10, TAMANY_PANTALLA_Y - 30, 0.5, "Premi ESC per sortir");

        return true;
    }

    GraphicManager::getInstance()->drawSprite(GRAFIC_FONS, 0, 0);
    GraphicManager::getInstance()->drawSprite(GRAFIC_TAULER, POS_X_TAULER, POS_Y_TAULER);

    vector<Posicio> movimentsPerMostrar;
    if (m_teSeleccio) {
        movimentsPerMostrar = m_movimentsValids;
	}

	// Dibuixar el tauler amb les fitxes
    m_tauler.visualitza(movimentsPerMostrar);

    static bool clickAnterior_out = false;
    Posicio posActual = conversorCoordenades(mousePosX, mousePosY);

    if (m_mode == MODE_JOC_REPLAY && !m_cuaMoviments.buida() && mouseStatus && !clickAnterior_out) {
        if (m_esperantClickReplay)
        {
            // Primer click per iniciar el replay
            m_esperantClickReplay = false;
        }
        else
        {
            m_tauler.actualitzaMovimentsValids();
            Moviment mov = m_cuaMoviments.treu();
            m_tauler.mouFitxa(mov.getInici(), mov.getFi());
            // canviar turn y comprobar fi de partida…
            m_tornActual = (m_tornActual == C_BLANC ? C_NEGRE : C_BLANC);
            comprovaFiPartida();
        }

    } else if (m_mode == MODE_JOC_NORMAL && mouseStatus) {
        
        // ------ MODE NORMAL ----- // 
	    // Posa l'efecte se selecció a la fitxa seleccionada
        if (m_teSeleccio) {

		    TipusFitxa tipusFitxa = m_tauler.getCela(m_fitxaSeleccionada.getFila(), m_fitxaSeleccionada.getColumna()).getTipus();

            if (tipusFitxa != TIPUS_EMPTY) {

                int x = POS_X_TAULER + CASELLA_INICIAL_X + m_fitxaSeleccionada.getColumna() * AMPLADA_CASELLA;
                int y = POS_Y_TAULER + CASELLA_INICIAL_Y + m_fitxaSeleccionada.getFila() * ALCADA_CASELLA;

                GraphicManager::getInstance()->drawSprite(GRAFIC_POSICIO_VALIDA, x, y);

                Fitxa fitxa = m_tauler.getCela(m_fitxaSeleccionada.getFila(), m_fitxaSeleccionada.getColumna());
                fitxa.visualitza(x, y);
        
            }
        }

        // Sistema de selecció y movimient
        static bool clickAnterior = false;
        Posicio posActual = conversorCoordenades(mousePosX, mousePosY);

        if (mouseStatus)
        {
            if (posActual.getFila() != -1 && posActual.getColumna() != -1)
            {
                if (!m_teSeleccio)
                {
                    // Seleccionar fitxa si es del color del torn actual
                    ColorFitxa colorFitxa = m_tauler.getCela(posActual.getFila(), posActual.getColumna()).getColor();
                    if (colorFitxa == m_tornActual)
                    {
                        m_fitxaSeleccionada = posActual;
                        m_teSeleccio = true;

                        m_tauler.actualitzaMovimentsValids();
                        m_movimentsValids = m_tauler.getCela(posActual).getMovsValids().getMoviments();


                    }
                }
                else
                {
				
                    // Intentar moure la fitxa seleccionada
                    bool movimentValid = false;

                    // Verificar si el movimient és valid
                    for (const auto& mov : m_movimentsValids)
                    {

					    //cout << "Verificant movimient: " << m_fitxaSeleccionada.toString() <<  " -> " << mov.toString() << endl;
                        if (mov == posActual)
                        {
                            movimentValid = true;
                            break;
                        }
                    }

                    if (movimentValid)
                    {
                        // Realitzar el movimento
                        m_tauler.mouFitxa(m_fitxaSeleccionada, posActual);

						// Guardar el movimient a la cua
                        guardaMoviment(m_fitxaSeleccionada, posActual);

                        // Cambiar torn y resetejar la seleccio
                        m_tornActual = (m_tornActual == C_BLANC) ? C_NEGRE : C_BLANC;
                        m_teSeleccio = false;
                        m_movimentsValids.clear();

                        // Comprobar si la partida ha acabat
                        comprovaFiPartida();
                    }
                    else
                    {
                        // Si es fa click en una altre fitxa del mateix color, canviar selecció
                        ColorFitxa colorNovaFitxa = m_tauler.getCela(posActual.getFila(), posActual.getColumna()).getColor();
                        if (colorNovaFitxa == m_tornActual)
                        {
                            m_fitxaSeleccionada = posActual;
                        
                            m_tauler.actualitzaMovimentsValids();

                            m_movimentsValids = m_tauler.getCela(posActual).getMovsValids().getMoviments();
                        
                        }
                        else
                        {
                            // Click en posició invàlida, cancelar selecció
                            m_teSeleccio = false;
                            m_movimentsValids.clear();
                        }
                    }
                }
           
            }
        }
        clickAnterior = mouseStatus;
    }

	clickAnterior_out = mouseStatus;
    


    string torn = (m_tornActual == COLOR_BLANC) ? "Blanques" : "Negres";
    string mode = (m_mode == MODE_JOC_NORMAL) ? "Normal" : "Replay";

    GraphicManager::getInstance()->drawFont(FONT_WHITE_30, 600, 2, 0.75, torn);
    GraphicManager::getInstance()->drawFont(FONT_WHITE_30, 40, 2, 0.75, mode);
    GraphicManager::getInstance()->drawFont(FONT_WHITE_30, 500, 720, 1.0,
        "Mouse: " + to_string(mousePosX) + ", " + to_string(mousePosY));

    return m_fiPartida;
}

void Joc::finalitza()
{
	/*
	▪ Si el mode és MODE_JOC_NORMAL:
		▪ S’haurà de guardar la cua de moviments al fitxer que s’ha indicat al paràmetre nomFitxerMoviments
		quan s’ha fet la crida al mètode inicialitza.
	▪ Si el mode és MODE_JOC_REPLAY, no s’ha de fer res.
	*/

    if (!m_finalitzaCridat && m_mode == MODE_JOC_NORMAL) {
        m_cuaMoviments.guardaAFitxer(m_nomFitxerMoviments);
        m_finalitzaCridat = true;
    }
}

void Joc::comprovaFiPartida() {
    // Comtar fitxas de cada jugador
    int blanques = 0, negres = 0;

    for (int fila = 0; fila < NUM_FILES_TAULER; fila++) {
        for (int col = 0; col < NUM_COLS_TAULER; col++) {

            ColorFitxa color = m_tauler.getCela(fila, col).getColor();
            
            if (color == COLOR_BLANC) blanques++;
            
            else if (color == COLOR_NEGRE) negres++;
        }
    }

    // Comprobar condicions de fi de joc
    if (blanques == 0 || negres == 0) {
        m_fiPartida = true;
        m_guanyador = (blanques > 0) ? C_BLANC : C_NEGRE;

        // Mostrar resultat en consola
        string guanyador = (m_guanyador == C_BLANC) ? "BLANQUES"  : "NEGRES";
        cout << "PARTIDA FINALITZADA! Guanyen les " << guanyador << endl;
    }
}

void Joc::guardaMoviment(const Posicio& origen, const Posicio& desti) {
    // Nomes guardar movimientos en modo normal
    if (m_mode == MODE_JOC_NORMAL) {
        m_cuaMoviments.afegeix(Moviment(origen, desti));
    }
}

void Joc::carregaTaulerInicial(const string& nomFitxer) {
    cout << "Fitxer: " << nomFitxer << endl;
    ifstream fitxer(nomFitxer);
    if (fitxer.is_open()) {
        cout << "Tauler inicialitzat" << endl;
        m_tauler.inicialitza(nomFitxer);
        fitxer.close();
    }
    else {
        cerr << "Error: No s'ha pogut obrir el fitxer " << nomFitxer << endl;
    }
}

Posicio Joc::conversorCoordenades(int x, int y) const {
    // Calcular posició relativa al tauler
    int relX = x - (POS_X_TAULER + CASELLA_INICIAL_X);
    int relY = y - (POS_Y_TAULER + CASELLA_INICIAL_Y);

    // Verificar límits
    if (relX < 0 || relY < 0 ||
        relX >= NUM_COLS_TAULER * AMPLADA_CASELLA ||
        relY >= NUM_FILES_TAULER * ALCADA_CASELLA) {
        return Posicio(-1, -1); // Posició invàlida
    }

    // Calcular fila i columna
    int columna = relX / AMPLADA_CASELLA;
    int fila = relY / ALCADA_CASELLA;

    return Posicio(fila, columna);
}

void Joc::processaSeleccio(const Posicio& pos) {
    if (m_tauler.getCela(pos.getFila(), pos.getColumna()).getColor() == m_tornActual) {
        m_fitxaSeleccionada = pos;
        m_teSeleccio = true;
        //m_movimentsValids = m_tauler.movimentsPossibles(pos);
    }
}

void Joc::processaMoviment(const Posicio& desti) {
    bool movimentValid = false;

    // Comprobar si el desti està en movimients vàlids
    for (const auto& pos : m_movimentsValids) {
        if (pos == desti) {
            movimentValid = true;
            break;
        }
    }

    if (movimentValid) {
        // Realitzar moviment
        Moviment mov(m_fitxaSeleccionada, desti);
        //m_tauler.mou(mov);

        // Guardar moviment (mode normal)
        if (m_mode == MODE_JOC_NORMAL) {
            guardaMoviment(m_fitxaSeleccionada, desti);
        }
        canviaTorn();

        // Comprobar fi de partida
        comprovaFiPartida();
    }

    // Resetejar selecció
    m_teSeleccio = false;
    m_movimentsValids.clear();
}

void Joc::canviaTorn() {
    m_tornActual = (m_tornActual == C_BLANC) ? C_NEGRE : C_BLANC;
}
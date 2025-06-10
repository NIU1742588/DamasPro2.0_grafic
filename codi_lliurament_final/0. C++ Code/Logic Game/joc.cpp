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
        m_cuaMoviments.esbuida();  // Limpiar cola existente
        m_nomFitxerMoviments = nomFitxerMoviments;  // Guardar nombre para finalizar
    }
    else if (mode == MODE_JOC_REPLAY) {
        m_cuaMoviments.carregaDeFitxer(nomFitxerMoviments);

    }
}

bool Joc::actualitza(int mousePosX, int mousePosY, bool mouseStatus)
{
    // Si la partida ha terminado, mostrar resultado y salir
    if (m_fiPartida) {
        GraphicManager::getInstance()->drawSprite(GRAFIC_FONS, 0, 0);

        string guanyador = (m_guanyador == C_BLANC) ? "EUROPEUS" : "NIGERIANS";

        GraphicManager::getInstance()->drawFont(FONT_WHITE_30,
            (TAMANY_PANTALLA_X-500) / 2,
            100,
            1.25, "GUANYEN ELS " + guanyador + "!!");

        GraphicManager::getInstance()->drawSprite(GRAFIC_POTATO_GOOD, (TAMANY_PANTALLA_X - 450) / 2, TAMANY_PANTALLA_Y - 500);

        GraphicManager::getInstance()->drawFont(FONT_RED_30,
            10,
            TAMANY_PANTALLA_Y - 30,
            0.5, "Premi ESC per sortir");
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
            m_tauler.actualitzaMovimentsValids();
            Moviment mov = m_cuaMoviments.treu();
            bool out = m_tauler.mouFitxa(mov.getInici(), mov.getFi());

            cout << "fa mov -> " << out << endl;

            // Cambiar turno después de cada movimiento en replay
            m_tornActual = (m_tornActual == C_BLANC) ? C_NEGRE : C_BLANC;

            // Comprobar si la partida ha terminado después del movimiento
            comprovaFiPartida();

    } else if (m_mode == MODE_JOC_NORMAL && mouseStatus) {
        
        // ------ MODE NORMAL ----- // 
	    // Posa l'efecte se selecció a la fitxa seleccionada
        if (m_teSeleccio) {

		    TipusFitxa tipusFitxa = m_tauler.getCela(m_fitxaSeleccionada.getFila(), m_fitxaSeleccionada.getColumna()).getTipus();

            if (tipusFitxa != TIPUS_EMPTY) {

                int x = POS_X_TAULER + CASELLA_INICIAL_X + m_fitxaSeleccionada.getColumna() * AMPLADA_CASELLA;
                int y = POS_Y_TAULER + CASELLA_INICIAL_Y + m_fitxaSeleccionada.getFila() * ALCADA_CASELLA;

                // Usamos el gráfico de posición válida para resaltar la selección
                GraphicManager::getInstance()->drawSprite(GRAFIC_POSICIO_VALIDA, x, y);

                // Volver a dibujar la ficha encima del resaltado
                Fitxa fitxa = m_tauler.getCela(m_fitxaSeleccionada.getFila(), m_fitxaSeleccionada.getColumna());
                fitxa.visualitza(x, y);
        
            }
        }

        // Sistema de selección y movimiento
        static bool clickAnterior = false;
        Posicio posActual = conversorCoordenades(mousePosX, mousePosY);

        if (mouseStatus)
        {
            if (posActual.getFila() != -1 && posActual.getColumna() != -1)
            {
                if (!m_teSeleccio)
                {
                    // Seleccionar ficha si es del color del turno actual
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
				
                    // Intentar mover la ficha seleccionada
                    bool movimentValid = false;

                    // Verificar si el movimiento es válido
                    for (const auto& mov : m_movimentsValids)
                    {

					    //cout << "Verificando movimiento: " << m_fitxaSeleccionada.toString() <<  " -> " << mov.toString() << endl;
                        if (mov == posActual)
                        {
                            movimentValid = true;
                            break;
                        }
                    }

                    if (movimentValid)
                    {
                        // Realizar el movimiento
                        m_tauler.mouFitxa(m_fitxaSeleccionada, posActual);

						// Guardar el movimiento en la cola
                        guardaMoviment(m_fitxaSeleccionada, posActual);

                        // Cambiar turno y resetear selección
                        m_tornActual = (m_tornActual == C_BLANC) ? C_NEGRE : C_BLANC;
                        m_teSeleccio = false;
                        m_movimentsValids.clear();

                        // Comprobar si la partida ha terminado
                        comprovaFiPartida();
                    }
                    else
                    {
                        // Si se hace click en otra ficha del mismo color, cambiar selección
                        ColorFitxa colorNovaFitxa = m_tauler.getCela(posActual.getFila(), posActual.getColumna()).getColor();
                        if (colorNovaFitxa == m_tornActual)
                        {
                            m_fitxaSeleccionada = posActual;
                        
                            m_tauler.actualitzaMovimentsValids();

                            m_movimentsValids = m_tauler.getCela(posActual).getMovsValids().getMoviments();
                        
                        }
                        else
                        {
                            // Click en posición inválida, cancelar selección
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

    // Solo guardar una vez al finalizar la partida
    if (!m_finalitzaCridat && m_mode == MODE_JOC_NORMAL) {
        m_cuaMoviments.guardaAFitxer(m_nomFitxerMoviments);
        m_finalitzaCridat = true;
    }
}

void Joc::comprovaFiPartida() {
    // Contar fichas de cada jugador
    int blanques = 0, negres = 0;

    for (int fila = 0; fila < NUM_FILES_TAULER; fila++) {
        for (int col = 0; col < NUM_COLS_TAULER; col++) {

            ColorFitxa color = m_tauler.getCela(fila, col).getColor();
            
            if (color == COLOR_BLANC) blanques++;
            
            else if (color == COLOR_NEGRE) negres++;
        }
    }

    // Comprobar condiciones de fin de juego
    if (blanques == 0 || negres == 0) {
        m_fiPartida = true;
        m_guanyador = (blanques > 0) ? C_BLANC : C_NEGRE;

        // Mostrar resultado en consola
        string guanyador = (m_guanyador == C_BLANC) ? "EUROPEUS" : "NIGERIANS";
        cout << "PARTIDA FINALITZADA! Guanyen els " << guanyador << endl;
    }
}

void Joc::guardaMoviment(const Posicio& origen, const Posicio& desti) {
    // Solo guardar movimientos en modo normal
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

void Joc::carregaMovimentsReplay(const string& nomFitxer) {

    cout << "Fitxer: " << nomFitxer << endl;

    ifstream fitxer(nomFitxer);
    if (fitxer.is_open()) {
        cout << "Llegint moviments" << endl;
        string posInici, posFinal;

        while (fitxer >> posInici >> posFinal) {

            cout << "Data: " << posInici << " " << posFinal << endl;

			Posicio posicioInici(posInici);
			Posicio posicioFinal(posFinal);

        }

        fitxer.close();
    }
    else {
        cerr << "Error: No s'ha pogut obrir el fitxer " << nomFitxer << endl;
    }
}

Posicio Joc::conversorCoordenades(int x, int y) const {
    // Calcular posición relativa al tablero
    int relX = x - (POS_X_TAULER + CASELLA_INICIAL_X);
    int relY = y - (POS_Y_TAULER + CASELLA_INICIAL_Y);

    // Verificar límites
    if (relX < 0 || relY < 0 ||
        relX >= NUM_COLS_TAULER * AMPLADA_CASELLA ||
        relY >= NUM_FILES_TAULER * ALCADA_CASELLA) {
        return Posicio(-1, -1); // Posición inválida
    }

    // Calcular fila y columna
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

    // Comprobar si el destino está en movimientos válidos
    for (const auto& pos : m_movimentsValids) {
        if (pos == desti) {
            movimentValid = true;
            break;
        }
    }

    if (movimentValid) {
        // Realizar movimiento
        Moviment mov(m_fitxaSeleccionada, desti);
        //m_tauler.mou(mov);

        // Guardar movimiento (modo normal)
        if (m_mode == MODE_JOC_NORMAL) {
            guardaMoviment(m_fitxaSeleccionada, desti);
        }

        // Cambiar turno
        canviaTorn();

        // Comprobar fin de partida
        comprovaFiPartida();
    }

    // Resetear selección
    m_teSeleccio = false;
    m_movimentsValids.clear();
}

void Joc::canviaTorn() {
    m_tornActual = (m_tornActual == C_BLANC) ? C_NEGRE : C_BLANC;
}
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
	: m_mode(MODE_JOC_NORMAL), m_tornActual(C_BLANC), m_fiPartida(false),
	m_guanyador(NO_COLOR), m_teSeleccio(false), m_indexReplay(0) {

    m_tauler.inicialitza("None");
	m_tauler.actualitzaMovimentsValids();
}

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
	carregaTaulerInicial(nomFitxerTauler);

	if (mode == MODE_JOC_NORMAL) {
		//m_cuaMoviments = CuaMoviments();
        m_cuaMoviments.esbuida();
	}
	else if (mode == MODE_JOC_REPLAY) {
		carregaMovimentsReplay(nomFitxerMoviments);
	}
}

bool Joc::actualitza(int mousePosX, int mousePosY, bool mouseStatus)
{
    //TODO 1: Interactuar amb la crida per dibuixar gràfics (sprites).
    // 	      Dibuixar a pantalla el fons i el gràfic amb el tauler buit.
    //------------------------------------------------------------------

    //TODO 1.1 Afegir l'include de GraphicManager --> #include "GraphicManager.h"
    //TODO 1.2 Fer la crida de dibuixar un sprite --> GraphicManager::getInstance()->drawSprite(image, posX, posY);
    //	    Per començar podem cridar el drawSprite amb els params --> 
    //          (GRAFIC_FONS,0,0) i 
    //          (GRAFIC_TAULER, POS_X_TAULER, POS_Y_TAULER)


    GraphicManager::getInstance()->drawSprite(GRAFIC_FONS, 0, 0);
    GraphicManager::getInstance()->drawSprite(GRAFIC_TAULER, POS_X_TAULER, POS_Y_TAULER);

    vector<Posicio> movimentsPerMostrar;
    if (m_teSeleccio) {
        movimentsPerMostrar = m_movimentsValids;
	}

	// Dibuixar el tauler amb les fitxes
    m_tauler.visualitza(movimentsPerMostrar);

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


    //TODO 1.3: Dibuixar a pantalla el gràfic amb el tauler i una fitxa blanca a la posició (fila, columna ) del tauler


    //TODO 2: Interactuar amb el ratolí.
    //------------------------------------------------------------------
    //TODO 2.1: Dibuixar la fitxa blanca al tauler només si estem pressionant el botó del ratolí

    /*
    if (mouseStatus) {
        // TODO 2.2: Solo dentro de los límites del tablero
        Posicio pos = conversorCoordenades(mousePosX, mousePosY);
        if (pos.getFila() != -1 && pos.getColumna() != -1) {
            // TODO 2.3: Dibujar en la casilla del ratón
            int fitxaX = POS_X_TAULER + CASELLA_INICIAL_X + pos.getColumna() * AMPLADA_CASELLA;
            int fitxaY = POS_Y_TAULER + CASELLA_INICIAL_Y + pos.getFila() * ALCADA_CASELLA;

            // TODO 2.4: Mantener selección después de soltar
            static bool fitxaDibuixada = false;
            static Posicio ultimaPosicio;

            if (!fitxaDibuixada) {
                GraphicManager::getInstance()->drawSprite(GRAFIC_FITXA_BLANCA, fitxaX, fitxaY);
                ultimaPosicio = pos;
                fitxaDibuixada = true;
            }
            else {
                // Solo cambiar posición si es nueva casilla
                if (!(ultimaPosicio == pos)) {
                    GraphicManager::getInstance()->drawSprite(GRAFIC_FITXA_BLANCA, fitxaX, fitxaY);
                    ultimaPosicio = pos;
                }
            }
        }
    }
    */


    // Sistema de selección y movimiento
    static bool clickAnterior = false;
    Posicio posActual = conversorCoordenades(mousePosX, mousePosY);

    if (mouseStatus && !clickAnterior)
    {
        if (posActual.getFila() != -1 && posActual.getColumna() != -1)
        {
            if (!m_teSeleccio)
            {
                // Seleccionar ficha si es del color del turno actual
                ColorFitxa colorFitxa = m_tauler.getCela(posActual.getFila(), posActual.getColumna()).getColor();
				cout << endl << "Color de la fitxa seleccionada: " << colorFitxa << " - Color jugador: " << m_tornActual << endl;
                if (colorFitxa == m_tornActual)
                {
                    m_fitxaSeleccionada = posActual;
                    cout << "Ficha seleccionada: " << m_fitxaSeleccionada.toString() << endl;
                    m_teSeleccio = true;

                    m_tauler.actualitzaMovimentsValids();
                    m_movimentsValids = m_tauler.getCela(posActual).getMovsValids().getMoviments();

                    cout << "Movimientos validos: ";
                    for (const auto& mov : m_movimentsValids) {
                        cout << mov.toString() << " ";
                    }
                    cout << endl;
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

                    // Cambiar turno y resetear selección
                    m_tornActual = (m_tornActual == C_BLANC) ? C_NEGRE : C_BLANC;
                    m_teSeleccio = false;
                    m_movimentsValids.clear();
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
                        
                        cout << "Movimientos validos: ";
                        for (const auto& mov : m_movimentsValids) {
                            cout << mov.toString() << " ";
                        }
                        cout << endl;
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
    

    
    //TODO 2.2: Dibuixar la fitxa blanca al tauler només si estem pressionant el botó del ratolí i el ratolí
    // està dins del límits del tauler



    //TODO 2.3: Dibuixar la fitxa blanca al tauler només si estem pressionant el botó del ratolí i el ratolí
    // està dins del límits del tauler. Dibuixa la fitxa a la casella on està el ratolí



    //TODO 2.4: Dibuixar la fitxa blanca a la casella on cliquem al ratolí. La fitxa s'ha de mantenir dibuixada
    // a la casella quan deixem de clicar amb el ratolí. Quan cliquem a una altra casella, la fitxa canvia de posició
    // a la nova casella

    
    /*
    Posicio pos = conversorCoordenades(mousePosX, mousePosY);
    static bool clickAnterior = false;

    if (mouseStatus && !clickAnterior && pos.getFila() != -1 && pos.getColumna() != -1) {
        if (!m_teSeleccio) {
            processaSeleccio(pos);
        }
        else {
            processaMoviment(pos);
        }
    }
    clickAnterior = mouseStatus;

    */

    // TODO 3: Imprimir text per pantalla
    //------------------------------------------
    // TODO 3.1: Mostrar la posició actual del ratolí a sota del     

    string torn = (m_tornActual == COLOR_BLANC) ? "Blanques" : "Negres";
    string mode = (m_mode == MODE_JOC_NORMAL) ? "Normal" : "Replay";

    GraphicManager::getInstance()->drawFont(FONT_WHITE_30, 20, 700, 1.0, "Torn: " + torn);
    GraphicManager::getInstance()->drawFont(FONT_WHITE_30, 20, 730, 1.0, "Mode: " + mode);
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

	if (m_mode == MODE_JOC_NORMAL) {
		// Guardar movimientos en archivo
		ofstream fitxer("partida.txt");
		while (!m_cuaMoviments.buida()) {
			Moviment m = m_cuaMoviments.treu();
			fitxer << m.getInici().getFila() << " "
				<< m.getInici().getColumna() << " "
				<< m.getFi().getFila() << " "
				<< m.getFi().getColumna() << endl;
		}
		fitxer.close();
	}
}

void Joc::comprovaFiPartida()
{
}

//void Joc::comprovaFiPartida() {
//    // Contar fichas de cada jugador
//    int blanques = 0, negres = 0;
//
//    for (int fila = 0; fila < 8; fila++) {
//        for (int col = 0; col < 8; col++) {
//            Color color = m_tauler.getColor(Posicio(fila, col));
//            if (color == COLOR_BLANC) blanques++;
//            else if (color == COLOR_NEGRE) negres++;
//        }
//    }
//
//    // Comprobar condiciones de fin de juego
//    if (blanques == 0 || negres == 0) {
//        m_fiPartida = true;
//        m_guanyador = (blanques > 0) ? COLOR_BLANC : COLOR_NEGRE;
//    }
//}

void Joc::guardaMoviment(const Posicio& origen, const Posicio& desti) {
    m_cuaMoviments.afegeix(Moviment(origen, desti));
}

void Joc::carregaTaulerInicial(const string& nomFitxer) {
    ifstream fitxer(nomFitxer);
    if (fitxer.is_open()) {
        m_tauler.inicialitza(nomFitxer);
        fitxer.close();
    }
    else {
        cerr << "Error: No s'ha pogut obrir el fitxer " << nomFitxer << endl;
    }
}

void Joc::carregaMovimentsReplay(const string& nomFitxer) {
    ifstream fitxer(nomFitxer);
    if (fitxer.is_open()) {
        int filaInici, colInici, filaFi, colFi;

        while (fitxer >> filaInici >> colInici >> filaFi >> colFi) {
            Posicio inici(filaInici, colInici);
            Posicio fi(filaFi, colFi);
            m_movimentsReplay.push_back(Moviment(inici, fi));
        }

        fitxer.close();
        m_indexReplay = 0;
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
//
//  main.cpp
//
//  Copyright � 2018 Compiled Creations Limited. All rights reserved.
//

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined  (_WIN64)

#include <iostream>
//Definicio necesaria per poder incloure la llibreria i que trobi el main
#define SDL_MAIN_HANDLED
#include <windows.h>
//Llibreria grafica
#include "../Graphic Lib/libreria.h"
#include "../Graphic Lib/NFont/NFont.h"
#include <conio.h>      /* getch */ 

#elif __APPLE__
//Llibreria grafica
#include "../Graphic Lib/libreria.h"
#include "../Graphic Lib/NFont/NFont.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdocumentation"
#include <SDL2/SDL.h>
#pragma clang diagnostic pop

#endif

#include "./joc.hpp"
#include "./info_joc.hpp"

const int AMPLADA_BOTO = 200;
const int ALCADA_BOTO = 50;
const int POS_X_BOTO = (TAMANY_PANTALLA_X - AMPLADA_BOTO) / 2;
const int POS_Y_BOTO_JUGAR = 300;
const int POS_Y_BOTO_REPLAY = 400;
const int MARGIN_BOTO = 2; 
const int POS_Y_IMATGE = 100; 

bool puntEnBoto(int x, int y, int botoX, int botoY) {
    return (x >= botoX && x <= botoX + AMPLADA_BOTO &&
        y >= botoY && y <= botoY + ALCADA_BOTO);
}

int main(int argc, const char* argv[])
{
    // Instruccions SDL
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    // Pantalla
    Screen pantalla(TAMANY_PANTALLA_X, TAMANY_PANTALLA_Y);
    pantalla.show();

    bool sortir = false;
    ModeJoc modeSeleccionat = MODE_JOC_NONE;
    std::string fitxerTauler = "./data/Games/tauler_inicial.txt";
    std::string fitxerMoviments = "./data/Games/moviments.txt";

    // --- Menú principal ---
    while (!sortir && modeSeleccionat == MODE_JOC_NONE)
    {
        pantalla.processEvents();
        bool mouseStatus = Mouse_getBtnLeft();
        int mouseX = Mouse_getX();
        int mouseY = Mouse_getY();

        // Dibuixa fons i títol
        GraphicManager::getInstance()->drawSprite(GRAFIC_FONS, 0, 0);
        GraphicManager::getInstance()->drawSprite(GRAFIC_DAMA_NEGRA, 90, POS_Y_IMATGE);
        GraphicManager::getInstance()->drawSprite(GRAFIC_DAMA_NEGRA,
            TAMANY_PANTALLA_X - 90 * 1.75, POS_Y_IMATGE);
        GraphicManager::getInstance()->drawFont(FONT_WHITE_30,
            180.0f, 80.0f, 2.0f, "DamasPro 2.0");

        // Botons
        bool hoverJugar = puntEnBoto(mouseX, mouseY, POS_X_BOTO, POS_Y_BOTO_JUGAR);
        bool hoverReplay = puntEnBoto(mouseX, mouseY, POS_X_BOTO, POS_Y_BOTO_REPLAY);

        // “JUGAR”
        GraphicManager::getInstance()->drawFont(
            hoverJugar ? FONT_RED_30 : FONT_WHITE_30,
            POS_X_BOTO + (AMPLADA_BOTO - 100) / 2,
            POS_Y_BOTO_JUGAR + (ALCADA_BOTO - 50) / 2,
            true, "JUGAR");

        // “REPETICIÓ”
        GraphicManager::getInstance()->drawFont(
            hoverReplay ? FONT_RED_30 : FONT_WHITE_30,
            POS_X_BOTO + (AMPLADA_BOTO - 150) / 2,
            POS_Y_BOTO_REPLAY + (ALCADA_BOTO - 50) / 2,
            true, "REPETICIO");

        // Altres gràfics
        GraphicManager::getInstance()->drawSprite(GRAFIC_WILL_GAMBIT,
            TAMANY_PANTALLA_X - 300, TAMANY_PANTALLA_Y - 400);
        GraphicManager::getInstance()->drawSprite(GRAFIC_REI_ENIGMA,
            10, TAMANY_PANTALLA_Y - 380);

        // Selecció de mode
        if (mouseStatus) {
            if (hoverJugar)
                modeSeleccionat = MODE_JOC_NORMAL;
            else if (hoverReplay)
                modeSeleccionat = MODE_JOC_REPLAY;
        }

        // Esc per sortir
        if (Keyboard_GetKeyTrg(KEYBOARD_ESCAPE))
            sortir = true;

        pantalla.update();
    }

    // --- Inici del joc si s'ha triat mode ---
    if (!sortir && modeSeleccionat != MODE_JOC_NONE)
    {
        Joc joc;
        joc.inicialitza(modeSeleccionat, fitxerTauler, fitxerMoviments);

        // Juga fins a fi de partida o ESC
        bool gameOver = false;
        do {
            pantalla.processEvents();

            bool mouseStatus = Mouse_getBtnLeft();
            int mouseX = Mouse_getX();
            int mouseY = Mouse_getY();

            // actualitza() retorna true quan la partida ha acabat
            gameOver = joc.actualitza(mouseX, mouseY, mouseStatus);

            pantalla.update();
        } while (!gameOver && !Keyboard_GetKeyTrg(KEYBOARD_ESCAPE));

        // Si ha acabat per victòria, seguim mostrant el menú final fins a ESC
        if (gameOver) {
            while (!Keyboard_GetKeyTrg(KEYBOARD_ESCAPE)) {
                pantalla.processEvents();
                // amb (0,0,false) només redibuixem la pantalla de resultat
                joc.actualitza(0, 0, false);
                pantalla.update();
            }
        }

        // Guarda moviments si és mode NORMAL
        joc.finalitza();
    }

    // Neteja SDL
    SDL_Quit();
    return 0;
}


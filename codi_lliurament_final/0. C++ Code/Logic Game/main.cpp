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
const int MARGIN_BOTO = 2; // Grosor del borde
const int POS_Y_IMATGE = 100; // Posición vertical de la imagen

bool puntEnBoto(int x, int y, int botoX, int botoY) {
    return (x >= botoX && x <= botoX + AMPLADA_BOTO &&
        y >= botoY && y <= botoY + ALCADA_BOTO);
}

int main(int argc, const char* argv[])
{
    //Instruccions necesaries per poder incloure la llibreria i que trobi el main
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_VIDEO);

    //Inicialitza un objecte de la classe Screen que s'utilitza per gestionar la finestra grafica
    Screen pantalla(TAMANY_PANTALLA_X, TAMANY_PANTALLA_Y);
    //Mostrem la finestra grafica
    pantalla.show();

    bool sortir = false;
    ModeJoc modeSeleccionat = MODE_JOC_NONE;
    string fitxerTauler = "./data/Games/tauler_inicial.txt";
    string fitxerMoviments = "./data/Games/moviments.txt";

    // Bucle del menú principal
    while (!sortir && modeSeleccionat == MODE_JOC_NONE)
    {
        // Captura events
        pantalla.processEvents();
        bool mouseStatus = Mouse_getBtnLeft();
        int mouseX = Mouse_getX();
        int mouseY = Mouse_getY();

        // Dibuixa fons del menú
        GraphicManager::getInstance()->drawSprite(GRAFIC_FONS, 0, 0);

        // Dibuixa la Dama blanca al lado del título
        int posXDama = 90; // A la izquierda del título
        GraphicManager::getInstance()->drawSprite(GRAFIC_DAMA_NEGRA, posXDama, POS_Y_IMATGE);
        GraphicManager::getInstance()->drawSprite(GRAFIC_DAMA_NEGRA, TAMANY_PANTALLA_X - posXDama*1.75, POS_Y_IMATGE);

        // Dibuixa títol
        GraphicManager::getInstance()->drawFont(FONT_WHITE_30,
            180.0f, 80.0f, 2.0f, "DamasPro 2.0");

        // Comprova hover sobre botons
        bool hoverJugar = puntEnBoto(mouseX, mouseY, POS_X_BOTO, POS_Y_BOTO_JUGAR);
        bool hoverReplay = puntEnBoto(mouseX, mouseY, POS_X_BOTO, POS_Y_BOTO_REPLAY);


        // Dibuixa botó JUGAR
        FONT_NAME colorJugar = hoverJugar ? FONT_RED_30 : FONT_WHITE_30;
        GraphicManager::getInstance()->drawFont(colorJugar,
            POS_X_BOTO + (AMPLADA_BOTO-100) / 2,
            POS_Y_BOTO_JUGAR + (ALCADA_BOTO-50) / 2,
            true, "JUGAR");

        // Dibuixa botó REPETICIÓ
        FONT_NAME colorReplay = hoverReplay ? FONT_RED_30 : FONT_WHITE_30;
        GraphicManager::getInstance()->drawFont(colorReplay,
            POS_X_BOTO + (AMPLADA_BOTO - 150) / 2,
            POS_Y_BOTO_REPLAY + (ALCADA_BOTO - 50) / 2,
            true, "REPETICIO");

        GraphicManager::getInstance()->drawSprite(GRAFIC_WILL_GAMBIT, (TAMANY_PANTALLA_X - 300), TAMANY_PANTALLA_Y - 400);
        GraphicManager::getInstance()->drawSprite(GRAFIC_REI_ENIGMA, 10, TAMANY_PANTALLA_Y - 380);

        // Comprova clics
        if (mouseStatus)
        {
            if (hoverJugar) {
                modeSeleccionat = MODE_JOC_NORMAL;
            }
            else if (hoverReplay) {
                modeSeleccionat = MODE_JOC_REPLAY;
            }
        }

        // Comprova ESC per sortir
        if (Keyboard_GetKeyTrg(KEYBOARD_ESCAPE)) {
            sortir = true;
        }

        pantalla.update();
    }

    // Si s'ha seleccionat un mode, inicia el joc
    if (modeSeleccionat != MODE_JOC_NONE && !sortir)
    {
        Joc joc;
        joc.inicialitza(modeSeleccionat, fitxerTauler, fitxerMoviments);


        do
        {
            // Captura tots els events de ratolí i teclat de l'ultim cicle
            pantalla.processEvents();

            bool mouseStatus = Mouse_getBtnLeft();
            int mousePosX = Mouse_getX();
            int mousePosY = Mouse_getY();
            bool final = joc.actualitza(mousePosX, mousePosY, mouseStatus);

            // Actualitza la pantalla
            pantalla.update();

        } while (!Keyboard_GetKeyTrg(KEYBOARD_ESCAPE));
        // Sortim del bucle si pressionem ESC
        joc.finalitza();

    }
    //Instruccio necesaria per alliberar els recursos de la llibreria 
    SDL_Quit();
    return 0;
}


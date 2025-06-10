#ifndef JOC_H
#define JOC_H

#include <string>
#include <vector>
#include "Tauler.hpp"
#include "CuaMoviments.hpp"
#include "Posicio.hpp"
#include "Moviment.hpp"
#include "Info_joc.hpp"

using namespace std;
 
//typedef enum {
//    MODE_JOC_NORMAL,
//    MODE_JOC_REPLAY,
//    MODE_JOC_NONE
//} ModeJoc;

typedef enum {
    C_NEGRE,
    C_BLANC,
    NO_COLOR
} ColorJugador;

class Joc {
public:
    Joc();

    void inicialitza(ModeJoc mode, const string& nomFitxerTauler, const string& nomFitxerMoviments = "");
    bool actualitza(int mousePosX, int mousePosY, bool mouseStatus);
    void finalitza();

    IMAGE_NAME getSpriteFromTipus(TipusFitxa tipus, ColorJugador color);

    // Funciones para renderizado (opcional)
    void render() const;

private:
    Tauler m_tauler;
    CuaMoviments m_cuaMoviments;
    ModeJoc m_mode;
    ColorJugador m_tornActual;
    bool m_fiPartida;
    ColorJugador m_guanyador;
    Posicio m_fitxaSeleccionada;
    bool m_teSeleccio;
    vector<Posicio> m_movimentsValids;
    bool m_finalitzaCridat;

	string m_nomFitxerMoviments;  // Nom del fitxer per guardar moviments en mode normal


    // Métodos auxiliares
    void processaSeleccio(const Posicio& pos);
    void processaMoviment(const Posicio& desti);
    void canviaTorn();
    void comprovaFiPartida();
    void guardaMoviment(const Posicio& origen, const Posicio& desti);
    void carregaTaulerInicial(const string& nomFitxer);
    void carregaMovimentsReplay(const string& nomFitxer);
    Posicio conversorCoordenades(int x, int y) const;
};

#endif
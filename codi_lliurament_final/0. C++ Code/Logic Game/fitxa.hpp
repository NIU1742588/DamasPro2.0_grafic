#pragma once

#include "posicio.hpp"
#include "moviment.hpp"
#include "GraphicManager.h"

using namespace std;

typedef enum
{
	TIPUS_NORMAL,
	TIPUS_DAMA,
	TIPUS_EMPTY
} TipusFitxa;

typedef enum
{
	COLOR_NEGRE,
	COLOR_BLANC,
} ColorFitxa;

class Fitxa {
private:
	ColorFitxa m_color;
	TipusFitxa m_tipus;
	Posicio m_pos;
	Moviment m_movsValids;

public:
	Fitxa() { m_tipus = TIPUS_EMPTY; m_color = COLOR_BLANC;};
	Fitxa(ColorFitxa color, TipusFitxa tipus, Posicio pos) : m_color(color), m_tipus(tipus), m_pos(pos) {};

	ColorFitxa getColor() const { return m_color; };
	TipusFitxa getTipus() const { return m_tipus; };
	Posicio getPosicio() const { return m_pos; };
	Moviment getMovsValids() const { return m_movsValids; };

	void setPosicio(const Posicio& pos) { m_pos = pos; } // --> cridarem a tauler.cpp per mouFitxa

	// Comprova els moviments vàlids
	void actualitzaMoviments(const Fitxa tauler[][N_COLUMNES]); //Acutualitza els moviements d'una fitxa --> actualitzaMovimentsValids();

	//Elimina una fitxa si l'han matat
	void eliminaFitxa() {m_tipus = TIPUS_EMPTY;}

	// Converteix la fitxa en una Damisela
	void coronacio() {m_tipus = TIPUS_DAMA;}

	Fitxa llegeixFitxa(char& tipus, const string& posicioStr);

	void visualitza(int posX, int posY) const;
};
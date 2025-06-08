#ifndef TAULER_HPP
# define TAULER_HPP

#include "fitxa.hpp"
#include "posicio.hpp"
#include <string>
#include <iostream>
#include "info_joc.hpp"

using namespace std;

class Tauler {

private:
	Fitxa m_tauler[N_FILES][N_COLUMNES];

public:
	
	void inicialitza(const string& nomFitxer); // inicialitza el tauler a partir de la info  d'un fitxer 

	Fitxa getCela(const int& fila, const int& columna) const { return m_tauler[fila][columna]; }
	Fitxa getCela(const Posicio& pos) const { return m_tauler[pos.getFila()][pos.getColumna()]; }
	
	void actualitzaMovimentsValids(); // Actualitzar tots els moviments vàlids que pot qualsevol de les peces del tauler
	
	// posicions on es podria moure la peça que ocupa la posició origen
	void getPosicionsPossibles(const Posicio& origen, int& nPosicions, Posicio posicionsPossibles[]); 
	
	bool esCapturaMaxima(const Posicio& origen, const Posicio& desti) const;

	bool mouFitxa(const Posicio& origen, const Posicio& desti);
	
	// Genera un string amb l’estat actual del tauler 
	string toString() const;
	void stringToPosicio(const string& posicio, int& fila, int& columna); //utilitzat en inicialitza, passar d'un string a fila i col

	void visualitza(const vector<Posicio>& movimentsValids = {}) const;

};	

#endif
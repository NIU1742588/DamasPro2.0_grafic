#pragma once

#include <string>
#include <fstream>
using namespace std;

const int N_FILES = 8;
const int N_COLUMNES = 8;

class Posicio {
public:
	// Constructors
	Posicio() : m_fila(0), m_columna(0) {};
	Posicio(int fila, int columna) : m_fila(fila), m_columna(columna) {}
	Posicio(const string& posicio) { fromString(posicio); };
	Posicio(const char posicio[3]);

	// Getters
	int getFila() const { return m_fila; };
	int getColumna() const { return m_columna; };

	// Setters
	void setFila(int fila) { m_fila = fila; }
	void setColumna(int columna) { m_columna = columna; }

	// String --> poscio -- posicio --> String
	string toString() const;
	void fromString(const string& pos);

	// Ens diu si una posicio esta dins del tauler
	bool EsPosicioValida() const; 

	bool operator==(const Posicio& posicio) const { return (m_fila == posicio.m_fila && m_columna == posicio.m_columna); };

private:
	int m_fila, m_columna;
};

ifstream& operator>>(ifstream& fitxer, Posicio& posicio);
ostream& operator<<(ostream& fitxer, const Posicio& posicio);


#include <iostream>
#include <fstream>
#include "posicio.hpp"

using namespace std;

bool Posicio::EsPosicioValida() const
{
	return m_fila >= 0 && m_fila < N_FILES &&
		m_columna >= 0 && m_columna < N_COLUMNES;
}

Posicio::Posicio(const char posicio[3]) 
{
	m_columna = tolower(posicio[0]) - 'a';
	m_fila = (N_FILES - 1) - (posicio[1] - '1');
}

void Posicio::fromString(const string& posicio)
{
	m_fila = (N_FILES - 1) - (posicio[1] - '1');
	m_columna = tolower(posicio[0]) - 'a';
}

string Posicio::toString() const
{
	string posicio = "a1";
	posicio[0] = 'a' + m_columna;
	posicio[1] = '1' + (N_FILES - 1) - m_fila;

	return posicio;
}

ifstream& operator>>(ifstream& fitxer, Posicio& posicio) 
{
	string pos;
	fitxer >> pos;
	posicio.fromString(pos);
	return fitxer;
}

ostream& operator<<(ostream& fitxer, const Posicio& posicio) 
{
	fitxer << posicio.toString();
	return fitxer;
}

#include "moviment.hpp"


Moviment::Moviment()
    : m_inici(Posicio()), m_fi(Posicio()), m_numMovs(0), m_esCaptura(false) {
}

// Constructor con parámetros
Moviment::Moviment(const Posicio& inici, const Posicio& fi, bool captura)
    : m_inici(inici), m_fi(fi), m_numMovs(0), m_esCaptura(captura) {
}

void Moviment::afegeixMoviment(const Posicio& pos, const vector<Posicio>& captures)
{
    m_moviments.push_back(pos);
    m_movimentsCaptura.push_back(captures);
    m_numMovs = m_moviments.size();

	/*if (captures.size() > 0) {
		cout << "Captures: ";
		for (const auto& captura : captures) {
			cout << captura.toString() << " ";
		}
		cout << endl;
	}*/
}

void Moviment::eliminaMoviment(const Posicio& pos)
{
    for (auto it = m_moviments.begin(); it != m_moviments.end(); ++it)
    {
        if (*it == pos)
        {
            int index = distance(m_moviments.begin(), it);
            m_moviments.erase(it);
            m_movimentsCaptura.erase(m_movimentsCaptura.begin() + index);
            m_numMovs--;
            break; // Surt del bucle un cop troba i elimina la posició
        }
    }
}

void Moviment::mostraMoviments() const
{
    for (int i = 0; i < m_numMovs; ++i)
    {
        cout << m_moviments[i].toString() << " ";
    }
    cout << endl;
}

Posicio Moviment::getUltimaPosicio() const
{
    Posicio resultat = Posicio();
    if (m_numMovs > 0)
        resultat = m_moviments[m_numMovs - 1];

    return resultat;
}


bool Moviment::contePosicio(const Posicio& pos) const
{
    bool trobat = false;
    int i = 0;

    while (i < m_numMovs && !trobat)
    {
        trobat = (m_moviments[i] == pos);
        i++;
    }

    return trobat;
}

void Moviment::duplica(const Moviment& original)
{
    m_numMovs = original.m_numMovs;
    m_esCaptura = original.m_esCaptura;
    for (int i = 0; i < m_numMovs; i++)
        m_moviments[i] = original.m_moviments[i];
}

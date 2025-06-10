#pragma once

#include <iostream>
#include <vector>
#include "posicio.hpp"

using namespace std;

class Moviment {
private:
    vector <Posicio> m_moviments;
    vector <vector<Posicio>> m_movimentsCaptura;
    int m_numMovs;
    Posicio m_inici;
    Posicio m_fi;
    bool m_esCaptura;

public:

    Moviment();
    Moviment(const Posicio& inici, const Posicio& fi, bool captura = false);

    Posicio getInici() const { return m_inici; };
    Posicio getFi() const { return m_fi; };

    void setInici(const Posicio& pos) { m_inici = pos; };
    void setFi(const Posicio& pos) { m_fi = pos; };
    void setCaptura(bool captura) { m_esCaptura = captura; };

    const vector<Posicio>& getMoviments() const { return m_moviments; }
    const vector<vector<Posicio>>& getMovimentsCaptura() const { return m_movimentsCaptura; }

    const Posicio& getMoviment(int index) const { return m_moviments.at(index); }
    const vector<Posicio>& getMovimentsCaptura(int index) const { return m_movimentsCaptura.at(index); }

    int getNumMovs() const { return m_numMovs; }
    bool esCaptura() const { return m_esCaptura; }
    Posicio getUltimaPosicio() const;

    void clear() {
        m_moviments.clear();
        m_movimentsCaptura.clear();
        m_numMovs = 0;
        m_esCaptura = false;
    };
    void marcaComCaptura() { m_esCaptura = true; }
    void afegeixMoviment(const Posicio& pos, const vector<Posicio>& captures = vector<Posicio>());
    void eliminaMoviment(const Posicio& pos);
    void duplica(const Moviment& original);
    bool contePosicio(const Posicio& pos) const;
    void mostraMoviments() const;


};



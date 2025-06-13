#include "fitxa.hpp"

Fitxa Fitxa::llegeixFitxa(char& tipus, const string& posicioStr)
{
    Posicio posicio(posicioStr);

    // Crear la fitxa corresponent
    ColorFitxa color;
    TipusFitxa tipusFitxa;

    switch (tipus)
    {
    case 'O':
        color = COLOR_BLANC;
        tipusFitxa = TIPUS_NORMAL;
        break;
    case 'X':
        color = COLOR_NEGRE;
        tipusFitxa = TIPUS_NORMAL;
        break;
    case 'D':
        color = COLOR_BLANC;
        tipusFitxa = TIPUS_DAMA;
        break;
    case 'R':
        color = COLOR_NEGRE;
        tipusFitxa = TIPUS_DAMA;
        break;
    default:
        color = COLOR_BLANC;
        tipusFitxa = TIPUS_EMPTY;
        break;
    }

    return Fitxa(color, tipusFitxa, posicio);
}


void escriureFitxa(const string& nomFitxer, char tipusFitxa, const Posicio& posicio)
{
    ofstream fitxer(nomFitxer);
    fitxer << tipusFitxa << ' ';
    fitxer << posicio;
    fitxer.close();
}

void Fitxa::actualitzaMoviments(const Fitxa tauler[][N_COLUMNES])
{
    m_movsValids.clear();

    // Moviment de les fitxes normals
    if (m_tipus == TIPUS_NORMAL)
    {
        int dirFil = (m_color == COLOR_BLANC) ? -1 : 1; // Blanc (Amunt) Negre (Avall)

        // Moviments simples
        for (int dirCol : {-1, 1})
        {
            int novaFil = m_pos.getFila() + dirFil;
            int novaCol = m_pos.getColumna() + dirCol;
            if (Posicio(novaFil, novaCol).EsPosicioValida() &&
                tauler[novaFil][novaCol].getTipus() == TIPUS_EMPTY &&
                !m_movsValids.contePosicio(Posicio(novaFil, novaCol)))
            {
                m_movsValids.afegeixMoviment(Posicio(novaFil, novaCol));
            }
        }

        struct State { Posicio pos; vector<Posicio> captures; };
        vector<State> posActuals = { {m_pos, {}} };
        vector<State> novesPosicions;
        vector<Posicio> posVisitades = { m_pos };
        bool potContinuar = true;

        while (potContinuar && !posActuals.empty())
        {
            potContinuar = false;
            novesPosicions.clear();

            for (const State& estat : posActuals)
            {
                int filActual = estat.pos.getFila();
                int colActual = estat.pos.getColumna();

                for (int dirCol : {-1, 1})
                {
                    int filInter = filActual + dirFil;
                    int colInter = colActual + dirCol;
                    int filFinal = filInter + dirFil;
                    int colFinal = colInter + dirCol;

                    if (!Posicio(filInter, colInter).EsPosicioValida() ||
                        !Posicio(filFinal, colFinal).EsPosicioValida())
                        continue;

                    bool jaCapturat = false;
                    for (const Posicio& p : estat.captures)
                        if (p == Posicio(filInter, colInter)) { jaCapturat = true; break; }

                    if (!jaCapturat &&
                        tauler[filInter][colInter].getTipus() != TIPUS_EMPTY &&
                        tauler[filInter][colInter].getColor() != m_color &&
                        tauler[filFinal][colFinal].getTipus() == TIPUS_EMPTY)
                    {
                        Posicio novaPos(filFinal, colFinal);
                        bool visitada = false;
                        for (const Posicio& p : posVisitades)
                            if (p == novaPos) { visitada = true; break; }

                        if (!visitada)
                        {
                            auto novesCaptures = estat.captures;
                            novesCaptures.push_back(Posicio(filInter, colInter));

                            m_movsValids.afegeixMoviment(novaPos, novesCaptures);
                            m_movsValids.marcaComCaptura();

                            novesPosicions.push_back({ novaPos, novesCaptures });
                            posVisitades.push_back(novaPos);
                            potContinuar = true;
                        }
                    }
                }
            }

            posActuals = move(novesPosicions);
        }
    }
    // Moviments de les dames
    else if (m_tipus == TIPUS_DAMA)
    {
        bool anyCap = false;
        vector<Posicio> path;  // posicions ja capturades en la ruta actual

        // Lambda recursiva sense <functional>
        auto dfs = [&](auto&& self, const Posicio& cur) -> void {
            for (int df : {-1, 1}) {
                for (int dc : {-1, 1}) {
                    // Avança fins trobar la peça enemiga
                    int r = cur.getFila() + df, c = cur.getColumna() + dc;
                    while (Posicio(r, c).EsPosicioValida() &&
                        tauler[r][c].getTipus() == TIPUS_EMPTY)
                    {
                        r += df; c += dc;
                    }
                    Posicio mid(r, c);
                    if (!mid.EsPosicioValida()) continue;
                    if (tauler[r][c].getTipus() == TIPUS_EMPTY ||
                        tauler[r][c].getColor() == m_color)
                        continue;
                    // si ja capturada en aquesta ruta, skip
                    bool repetida = false;
                    for (const Posicio& p : path)
                        if (p == mid) { repetida = true; break; }
                    if (repetida) continue;

                    // única casella buida immediatament darrere
                    int r2 = r + df, c2 = c + dc;
                    Posicio land(r2, c2);
                    if (!land.EsPosicioValida() ||
                        tauler[r2][c2].getTipus() != TIPUS_EMPTY)
                        continue;

                    // registrar captura + moviment
                    anyCap = true;
                    path.push_back(mid);
                    m_movsValids.afegeixMoviment(land, path);
                    m_movsValids.marcaComCaptura();

                    // encadenar
                    self(self, land);

                    // desfer per provar altres diagonals
                    path.pop_back();
                }
            }
            };

        // iniciar recursió des de la posició actual
        dfs(dfs, m_pos);

        // Moviments simples "voladors" de la dama
        for (int df : {-1, 1})
        {
            for (int dc : {-1, 1})
            {
                int r = m_pos.getFila() + df;
                int c = m_pos.getColumna() + dc;
                while (Posicio(r, c).EsPosicioValida() &&
                    tauler[r][c].getTipus() == TIPUS_EMPTY)
                {
                    if (!m_movsValids.contePosicio(Posicio(r, c)))
                        m_movsValids.afegeixMoviment(Posicio(r, c));
                    r += df; c += dc;
                }
            }
        }
    }
}



void Fitxa::visualitza(int posX, int posY) const {

    IMAGE_NAME sprite = GRAFIC_NUM_MAX;

    switch (m_tipus) {
    case TIPUS_NORMAL:
        sprite = (m_color == COLOR_BLANC) ? GRAFIC_FITXA_BLANCA : GRAFIC_FITXA_NEGRA;
        break;
    case TIPUS_DAMA:
        sprite = (m_color == COLOR_BLANC) ? GRAFIC_DAMA_BLANCA : GRAFIC_DAMA_NEGRA;
        break;
    default:
        break;
    }

    GraphicManager::getInstance()->drawSprite(sprite, posX, posY);
}
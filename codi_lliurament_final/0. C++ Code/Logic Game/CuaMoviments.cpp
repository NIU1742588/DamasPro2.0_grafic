#include "CuaMoviments.hpp"

CuaMoviments::CuaMoviments()
    : m_primer(nullptr), m_ultim(nullptr) {
}

CuaMoviments::~CuaMoviments() {
    esbuida();
}

void CuaMoviments::afegeix(const Moviment& moviment) {
    Node* nou = new Node(moviment);

    if (buida()) {
        m_primer = nou;
    }
    else {
        m_ultim->seg = nou;
    }

    m_ultim = nou;
}

Moviment CuaMoviments::treu() {
    if (buida()) {
        //Retorna moviment inv�lid si la cua esta buida 
        return Moviment(Posicio(-1, -1), Posicio(-1, -1));
    }

    Node* aux = m_primer;
    Moviment mov = aux->moviment;
    m_primer = m_primer->seg;

    if (m_primer == nullptr) {
        m_ultim = nullptr;
    }

    delete aux;
    return mov;
}

bool CuaMoviments::buida() const {
    return m_primer == nullptr;
}

void CuaMoviments::esbuida() {
    while (!buida()) {
        treu();
    }
}

// Guarda tots els moviments en un arxiu
void CuaMoviments::guardaAFitxer(const std::string& nomFitxer) const {
    std::ofstream fitxer(nomFitxer);
    if (!fitxer.is_open()) {
        std::cerr << "Error obrint fitxer per escriptura: " << nomFitxer << std::endl;
        return;
    }

    Node* actual = m_primer;
    while (actual != nullptr) {
        const Posicio& inici = actual->moviment.getInici();
        const Posicio& fi = actual->moviment.getFi();

		fitxer << inici.toString() << " " << fi.toString() << "\n";
        actual = actual->seg;
    }

    fitxer.close();
}

// Carga movimients desde un arxiu
void CuaMoviments::carregaDeFitxer(const std::string& nomFitxer) 
{
    cout << "Carrega: " << nomFitxer << endl;
    esbuida();  // Netejar cua existent

    std::ifstream fitxer(nomFitxer);
    if (!fitxer.is_open()) {
        std::cerr << "Error obrint fitxer per lectura: " << nomFitxer << std::endl;
        return;
    }

    string posInici, posFinal;
    while (fitxer >> posInici >> posFinal) {

		/*cout << "Llegint moviment: "
			<< posInici<< " -> "
			<< posFinal<< endl;*/

		Posicio posicioInici(posInici);
		Posicio posicioFinal(posFinal);

        afegeix(Moviment(posicioInici, posicioFinal));
    }

    fitxer.close();
}
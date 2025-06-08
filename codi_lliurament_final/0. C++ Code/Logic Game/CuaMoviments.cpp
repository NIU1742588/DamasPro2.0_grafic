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
        // Devolver movimiento inválido si la cola está vacía
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
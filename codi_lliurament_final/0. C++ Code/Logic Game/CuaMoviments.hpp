#ifndef CUAMOVIMENTS_HPP
#define CUAMOVIMENTS_HPP

#include "moviment.hpp"
#include <fstream>  // Para guardar/leer de archivos
#include <iostream>

class CuaMoviments {
public:
    CuaMoviments();
    ~CuaMoviments();

    // Constructor de copia y operador de asignación (prohibidos)
    CuaMoviments(const CuaMoviments&) = delete;
    CuaMoviments& operator=(const CuaMoviments&) = delete;


    void afegeix(const Moviment& moviment);
    Moviment treu();
    bool buida() const;
    void esbuida();

    // Nuevos métodos para guardar y cargar desde archivo
    void guardaAFitxer(const std::string& nomFitxer) const;
    void carregaDeFitxer(const std::string& nomFitxer);

private:
    struct Node {
        Moviment moviment;
        Node* seg;
        Node(const Moviment& m) : moviment(m), seg(nullptr) {}
    };

    Node* m_primer;
    Node* m_ultim;
};

#endif
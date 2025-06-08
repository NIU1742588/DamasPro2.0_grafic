#ifndef CUAMOVIMENTS_HPP
#define CUAMOVIMENTS_HPP

#include "moviment.hpp"

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
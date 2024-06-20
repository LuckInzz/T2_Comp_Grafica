//
//  Ponto.h
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 18/08/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#ifndef Ponto_hpp
#define Ponto_hpp

#include <iostream>
#include <cmath>
using namespace std;

class Ponto {

public:
    float x,y,z;
    Ponto ();
    Ponto(float x, float y, float z=0);
    void set(float x, float y, float z=0);
    void imprime();
    void imprime(char const *msg);
    void imprime(char const *msgAntes, char const *msgDepois);
    void multiplica(double x, double y, double z);
    void soma(double x, double y, double z);
    void versor();
    double modulo();
    void rotacionaX(float angulo);
    void rotacionaY(float angulo);
    void rotacionaZ(float angulo);
} ;


Ponto ObtemMinimo (Ponto P1, Ponto P2);

Ponto ObtemMaximo (Ponto P1, Ponto P2);

bool operator==(Ponto P1, Ponto P2);
Ponto operator+(Ponto P1, Ponto P2);
Ponto operator- (Ponto P1, Ponto P2);
Ponto operator* (Ponto P1, float k);
Ponto operator-(Ponto P1);

double ProdEscalar(Ponto v1, Ponto v2);
void ProdVetorial (Ponto v1, Ponto v2, Ponto &vresult);
int intersec2d(Ponto k, Ponto l, Ponto m, Ponto n, double &s, double &t);
bool HaInterseccao(Ponto k, Ponto l, Ponto m, Ponto n);

long int getContadorInt();
void resetContadorInt();

int lado(Ponto P1, Ponto P2, Ponto A); // retorna uma das constantes a seguir

enum{
    ESQUERDA,
    DIREITA,
    SOBRE
};


double calculaDistancia(Ponto P, Ponto Q);
#endif /* Ponto_hpp */

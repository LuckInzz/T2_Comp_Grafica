// 
//  Ponto.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 18/08/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#include "Ponto.h"
Ponto::Ponto ()
{
    x=y=z=0;
}
Ponto::Ponto(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}
void Ponto::set(float x, float y, float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}
void Ponto::imprime() {
    cout << "(" << x << ", " << y << ", " << z <<")" << flush;
}
void Ponto::imprime(char const *msg)
{
    cout << msg;
    imprime();
}
void Ponto::imprime(char const *msgAntes, char const *msgDepois)
{
    imprime(msgAntes);
    cout << msgDepois;
}
void Ponto::multiplica(double x, double y, double z)
{
    this->x *= x;
    this->y *= y;
    this->z *= z;
}

void Ponto::soma(double x, double y, double z)
{
    this->x += x;
    this->y += y;
    this->z += z;
}

void Ponto::rotacionaZ(float angulo)
{
    float xr, yr;
    //cout << "Angulo: " << angulo << " ";
    double anguloRad = angulo * 3.14159265359/180.0;
    xr = x*cos(anguloRad) - y*sin(anguloRad);
    yr = x*sin(anguloRad) + y*cos(anguloRad);
    x = xr;
    y = yr;
    //imprime();
}


void Ponto::rotacionaY(float angulo)
{
    float xr, zr;
    double anguloRad = angulo* 3.14159265359/180.0;
    xr =  x*cos(anguloRad) + z*sin(anguloRad);
    zr = -x*sin(anguloRad) + z*cos(anguloRad);
    x = xr;
    z = zr;
}

void Ponto::rotacionaX(float angulo)
{
    float yr, zr;
    double anguloRad = angulo* 3.14159265359/180.0;
    yr =  y*cos(anguloRad) - z*sin(anguloRad);
    zr =  y*sin(anguloRad) + z*cos(anguloRad);
    y = yr;
    z = zr;
}
double Ponto::modulo()
{
    return sqrt(x*x+y*y+z*z);
}

void Ponto::versor()
{
    float m = modulo();
    x /= m;
    y /= m;
    z /= m;
}


Ponto ObtemMaximo (Ponto P1, Ponto P2)
{
    Ponto Max;
    
    Max.x = (P2.x > P1.x) ? P2.x : P1.x;
    Max.y = (P2.y > P1.y) ? P2.y : P1.y;
    Max.z = (P2.z > P1.x) ? P2.z : P1.z;
    return Max;
}
Ponto ObtemMinimo (Ponto P1, Ponto P2)
{
    Ponto Min;
    
    Min.x = (P2.x < P1.x) ? P2.x : P1.x;
    Min.y = (P2.y < P1.y) ? P2.y : P1.y;
    Min.z = (P2.z < P1.x) ? P2.z : P1.z;
    return Min;
}
bool operator==(Ponto P1, Ponto P2)
{
    if (P1.x != P2.x) return false;
    if (P1.y != P2.y) return false;
    if (P1.z != P2.z) return false;
    return true;

}
Ponto operator+(Ponto P1, Ponto P2)
{
    Ponto temp;
    temp = P1;
    temp.x += P2.x;
    temp.y += P2.y;
    temp.z += P2.z;
    return temp;
}

Ponto operator- (Ponto P1, Ponto P2)
{
    Ponto temp;
    temp = P1;
    temp.x -= P2.x;
    temp.y -= P2.y;
    temp.z -= P2.z;
    return temp;
}
Ponto operator* (Ponto P1, float k)
{
    Ponto temp;
    temp.x = P1.x * k;
    temp.y = P1.y * k;
    temp.z = P1.z * k;
    return temp;
}

Ponto operator-(Ponto P1)
{
    return P1 * -1;
}
// **********************************************************************
//    Calcula o produto escalar entre os vetores V1 e V2
// **********************************************************************
double ProdEscalar(Ponto v1, Ponto v2)
{
    return v1.x*v2.x + v1.y*v2.y+ v1.z*v2.z;
}
// **********************************************************************
//    Calcula o produto vetorial entre os vetores V1 e V2
// **********************************************************************
void ProdVetorial (Ponto v1, Ponto v2, Ponto &vresult)
{
    vresult.x = v1.y * v2.z - (v1.z * v2.y);
    vresult.y = v1.z * v2.x - (v1.x * v2.z);
    vresult.z = v1.x * v2.y - (v1.y * v2.x);
}
long int ContadorInt=0;
/* ********************************************************************** */
/*                                                                        */
/*  Calcula a interseccao entre 2 retas (no plano "XY" Z = 0)             */
/*                                                                        */
/* k : ponto inicial da reta 1                                            */
/* l : ponto final da reta 1                                              */
/* m : ponto inicial da reta 2                                            */
/* n : ponto final da reta 2                                              */
/*                                                                        */
/* s: valor do parâmetro no ponto de interseção (sobre a reta KL)         */
/* t: valor do parâmetro no ponto de interseção (sobre a reta MN)         */
/*                                                                        */
/* ********************************************************************** */
int intersec2d(Ponto k, Ponto l, Ponto m, Ponto n, double &s, double &t)
{
    double det;

    det = (n.x - m.x) * (l.y - k.y)  -  (n.y - m.y) * (l.x - k.x);

    if (det == 0.0)
        return 0 ; // não há intersecção

    s = ((n.x - m.x) * (m.y - k.y) - (n.y - m.y) * (m.x - k.x))/ det ;
    t = ((l.x - k.x) * (m.y - k.y) - (l.y - k.y) * (m.x - k.x))/ det ;

    return 1; // há intersecção
}
// **********************************************************************
// ux9k8IrAKy
// **********************************************************************
bool HaInterseccao(Ponto k, Ponto l, Ponto m, Ponto n)
{
    int ret;
    double s,t;
    
    ContadorInt = ContadorInt + 1;
    ret = intersec2d( k,  l,  m,  n, s, t);
    if (!ret) return false;
    if (s>=0.0 && s <=1.0 && t>=0.0 && t<=1.0)
        return true;
    else return false;

}
// **********************************************************************
//
// **********************************************************************
void resetContadorInt()
{
    ContadorInt = 0;
}
// **********************************************************************
//
// **********************************************************************
long int getContadorInt()
{
    return ContadorInt;
}


// **********************************************************************
//
// **********************************************************************
double calculaDistancia(Ponto P, Ponto Q)
{
    float dx, dy, dz;
    
    dx = P.x - Q.x;
    dy = P.y - Q.y;
    dz = P.z - Q.z;

    return sqrt(dx*dx+dy*dy+dz*dz);
}

// **********************************************************************
// int lado(Ponto P1, Ponto P2, Ponto A)
// **********************************************************************
int lado(Ponto P1, Ponto P2, Ponto A)
{
    Ponto V1, V2, V3;
    V1 = P2-P1;
    V2 = A-P1;
    ProdVetorial (V1, V2, V3);
    if (V3.z > 0)
        return ESQUERDA;
    if (V3.z < 0)
        return DIREITA;
    return SOBRE;
}

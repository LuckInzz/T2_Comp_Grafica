//
//  Instancia.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#include "Instancia.h"

void DesenhaPersonagem();
void DesenhaRetangulo();
// ***********************************************************
//  void InstanciaPonto(Ponto3D *p, Ponto3D *out)
//  Esta funcao calcula as coordenadas de um ponto no
//  sistema de referencia do universo (SRU), ou seja,
//  aplica as rotacoes, escalas e translacoes a um
//  ponto no sistema de referencia do objeto (SRO).
// ***********************************************************
void InstanciaPonto(Ponto &p, Ponto &out)
{
    GLfloat ponto_novo[4];
    GLfloat matriz_gl[4][4];
    int  i;
    
    glGetFloatv(GL_MODELVIEW_MATRIX,&matriz_gl[0][0]);
    
    for(i=0;i<4;i++)
    {
        ponto_novo[i]= matriz_gl[0][i] * p.x+
        matriz_gl[1][i] * p.y+
        matriz_gl[2][i] * p.z+
        matriz_gl[3][i];
    }
    out.x=ponto_novo[0];
    out.y=ponto_novo[1];
    out.z=ponto_novo[2];
}

Ponto InstanciaPonto(Ponto P)
{
    Ponto temp;
    InstanciaPonto(P, temp);
    return temp;
}
Instancia::Instancia()
{
    //cout << "Instanciado..." << endl;
    Rotacao = 0;
    Posicao = Ponto(0,0,0);
    Escala = Ponto(1,1,1);
    Pivot = Ponto(0,0,0);
}

void Instancia::desenha()
{
    // Aplica as transformacoes geometricas no modelo
    glPushMatrix();
        glTranslatef(Posicao.x, Posicao.y, 0);
        glTranslatef(Pivot.x, Pivot.y, Pivot.z);
        glRotatef(Rotacao, 0,0,1);
        glScalef(Escala.x, Escala.y, Escala.z);
        glTranslatef(-Pivot.x, -Pivot.y, -Pivot.z);

        // Obtem a posicao do ponto 0,0,0 no SRU
        // Nao eh usado aqui, mas eh util para detectar colisoes
        PosicaoDoPersonagem = InstanciaPonto(Ponto (0,0,0)+Pivot);
        
        (*modelo)(); // desenha a instancia
        
    glPopMatrix();
}

void Instancia::AtualizaPosicao(double tempoDecorrido)
{
    Posicao = Posicao + Direcao*tempoDecorrido*Velocidade;
}
void Instancia::ImprimeEnvelope(char const *msg1, char const *msg2)
{
    cout << msg1;
    for(int i=0;i<4;i++)
        Envelope[i].imprime();
    cout << msg2;
}

//
//  ModeloMatricial.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 02/05/21.
//  Copyright © 2024 Márcio Sarroglia Pinho. All rights reserved.
//
//
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

#include "ModeloMatricial.h"

ModeloMatricial::ModeloMatricial()
{
    nLinhas = nColunas = -1;
}
void ModeloMatricial::leModelo(const char *nome)
{
    ifstream input;            // ofstream arq;
    input.open(nome, ios::in); //arq.open(nome, ios::out);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << nome << "...";
    
    input >> nLinhas >> nColunas;

    for (int i=0; i< nLinhas; i++)
    {
        int cor;
        // Le cada elemento da linha
        for (int j=0;j<nColunas;j++)
        {
            input >> cor;
            if(!input)
            {
                cout << "***********************************"<< endl;
                cout << "Modelo Matricial - Erro na leitura do arquivo " << nome <<"." << endl;
                cout << "***********************************"<< endl;
                break;
            }
            Matriz[i][j] = cor;
        }
    }
    cout << "Modelo Matricial lido com sucesso!" << endl;

}
int ModeloMatricial::getColor(int i, int j)
{
    return Matriz[i][j];
}

void ModeloMatricial::Imprime()
{
    if (nLinhas == -1 || nColunas == -1)
    {   
        cout << "Modelo vazio." << endl;
    }
        
    for (int i=0; i<nLinhas; i++)
    {
        for (int j=0;j<nColunas;j++)
            cout << setw(3) << Matriz[i][j] << " ";
        cout << endl;
    }
}

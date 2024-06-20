//
//  ControlaTempo.cpp
//  Imagens
//
//  Created by Márcio Sarroglia Pinho on 23/03/20.
//  Copyright © 2020 rmd. All rights reserved.
//

#include "Temporizador.h"

// Inicializa o temporizador
Temporizador::Temporizador()
{
#ifdef _WIN32
    start_time = GetTickCount();
#else
    // Figure out time elapsed since last call to idle function
    gettimeofday(&start_time, NULL);
    
#endif

}
// Retorna o tempo decorrido desde a última chamada desta mesma função
double Temporizador::getDeltaT()
{
    double dt;

#ifdef _WIN32
    DWORD end_time;
    end_time = GetTickCount();
    dt = (float) (end_time - start_time) / 1000.0;
#else
    // Figure out time elapsed since last call to idle function
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    dt = (float)(end_time.tv_sec  - start_time.tv_sec) + 1.0e-6*(end_time.tv_usec - start_time.tv_usec);
#endif
    start_time = end_time;
    return dt;
}

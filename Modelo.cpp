//
//  Modelo.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 22/09/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//

#include "Modelo.h"

void Modelo::desenhaLinhas()
{
    glBegin(GL_LINE_STRIP);
    for (int i=0; i<Vertices.size(); i++)
        glVertex3f(Vertices[i].x,Vertices[i].y,Vertices[i].z);
    glEnd();
}



#include <cstdlib>

#include "Linha.h"

void Linha::geraLinha(int limite, int TamMax)
{
    float deltaX,deltaY;
        
    x1 = (rand() % limite*10)/10.0;
    y1 = (rand() % limite*10)/10.0;
    
    deltaX = (rand() % limite)/(float)limite;
    deltaY = (rand() % limite)/(float)limite;
    
    if(rand()%2)
        x2 = x1 + deltaX * TamMax;
    else x2 = x1 - deltaX * TamMax;
    if(rand()%2)
        y2 = y1 + deltaY * TamMax;
    else y2 = y1 - deltaY * TamMax;

}


void Linha::desenhaLinha()
{
	glBegin(GL_LINES);
		glVertex2f(x1,y1);
		glVertex2f(x2,y2);
	glEnd();
}

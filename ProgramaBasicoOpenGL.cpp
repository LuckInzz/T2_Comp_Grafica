// **********************************************************************
// PUCRS/Escola Polit�cnica
// COMPUTA��O GR�FICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.


#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>


using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <GL/glut.h>
#endif

#include "Ponto.h"
#include "Poligono.h"

#include "Temporizador.h"

#include "ListaDeCoresRGB.h"


// Limites l�gicos da �rea de desenho
Ponto Min, Max;
Poligono P1, P2;

bool desenha = false;

float angulo=0.0;

Ponto PosicaoDoCampoDeVisao, PontoClicado;
bool FoiClicado = false;


// **********************************************************************
//
// **********************************************************************
void init()
{
    Ponto MinPoly, MaxPoly, Folga;
    
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    Min = Ponto (-20, -20);
    Max = Ponto (20, 20);
    
    P1.LePoligono("Retangulo1x1.txt");

}

double nFrames=0;
double TempoTotal=0;
Temporizador T;
double AccumDeltaT=0;

// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;
    
    if (AccumDeltaT > 1.0/30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        angulo+=1.0;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    
    glOrtho(Min.x,Max.x, Min.y,Max.y, 0,1);
    //glOrtho(0,10, 0,10, 0,1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
//
// **********************************************************************
void DesenhaEixos()
{
    Ponto Meio;
    Meio.x = (Max.x+Min.x)/2;
    Meio.y = (Max.y+Min.y)/2;
    Meio.z = (Max.z+Min.z)/2;

    glBegin(GL_LINES);
    //  eixo horizontal
        glVertex2f(Min.x,Meio.y);
        glVertex2f(Max.x,Meio.y);
    //  eixo vertical
        glVertex2f(Meio.x,Min.y);
        glVertex2f(Meio.x,Max.y);
    glEnd();
}
// **********************************************************************
// void DesenhaTriangulo()
// **********************************************************************
void DesenhaTriangulo()
{
    glBegin(GL_TRIANGLES);
        glVertex2f(-10,-10);
        glVertex2f(0, 10);
        glVertex2f(10,-10);
    glEnd();
}
// **********************************************************************
void RotacionaAoRedorDeUmPonto(float alfa, Ponto P)
{
    glTranslatef(P.x, P.y, P.z);
    glRotatef(alfa, 0,0,1);
    glTranslatef(-P.x, -P.y, -P.z);
}
//
// **********************************************************************
//  void display( void )
// **********************************************************************
void display( void )
{

	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites logicos da area OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(4);

    
    defineCor(GreenYellow);
    //glScalef(1, -0.5, 1);
    glPushMatrix();
        DesenhaTriangulo();
    glPopMatrix();
    
    Ponto P = Ponto(-10, -10, 0);
    
    defineCor(VioletRed);

    DesenhaTriangulo();

    glColor3f(1,1,1); // R, G, B  [0..1]
    DesenhaEixos();
    
    defineCor(IndianRed);
    P1.desenhaPoligono();
    
    if (FoiClicado)
    {
        PontoClicado.imprime("- Ponto no universo: ", "\n");
        FoiClicado = false;
    }
    
	glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo n�mero de segundos e informa quanto frames
// se passaram neste per�odo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }

}

// **********************************************************************
// Esta fun��o captura o clique do botao direito do mouse sobre a �rea de
// desenho e converte a coordenada para o sistema de refer�ncia definido
// na glOrtho (ver fun��o reshape)
// Este c�digo � baseado em http://hamala.se/forums/viewtopic.php?t=20
// **********************************************************************
void Mouse(int button,int state,int x,int y)
{
    GLint viewport[4];
    GLdouble modelview[16],projection[16];
    GLfloat wx=x,wy,wz;
    GLdouble ox=0.0,oy=0.0,oz=0.0;

    if(state!=GLUT_DOWN)
      return;
    if(button!=GLUT_RIGHT_BUTTON)
     return;
    cout << "Botao da direita! ";

    glGetIntegerv(GL_VIEWPORT,viewport);
    y=viewport[3]-y;
    wy=y;
    glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
    glGetDoublev(GL_PROJECTION_MATRIX,projection);
    glReadPixels(x,y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);
    gluUnProject(wx,wy,wz,modelview,projection,viewport,&ox,&oy,&oz);
    PontoClicado = Ponto(ox,oy,oz);
    FoiClicado = true;
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
// **********************************************************************

void keyboard ( unsigned char key, int x, int y )
{

	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case 't':
            ContaTempo(3);
            break;
        case ' ':
            desenha = !desenha;
        break;
		default:
			break;
	}
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
		case GLUT_KEY_UP:       // Se pressionar UP
			glutFullScreen ( ); // Vai para Full Screen
			break;
        case GLUT_KEY_RIGHT:       // Se pressionar UP
            angulo--; // Vai para Full Screen
            break;
        case GLUT_KEY_LEFT:       // Se pressionar UP
            angulo++; // Vai para Full Screen
            break;
	    case GLUT_KEY_DOWN:     // Se pressionar UP
								// Reposiciona a janela
            glutPositionWindow (50,50);
			glutReshapeWindow ( 700, 500 );
			break;
		default:
			break;
	}
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;
    system ("pwd");

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 500, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de titulo da janela.
    glutCreateWindow    ( "Primeiro Programa em OpenGL" );

    // executa algumas inicializa��es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser� chamada
    // automaticamente sempre o usu�rio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );
    glutMouseFunc(Mouse);

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}

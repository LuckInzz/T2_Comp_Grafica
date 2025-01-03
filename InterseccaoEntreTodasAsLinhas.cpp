// **********************************************************************
// PUCRS/FACIN
// COMPUTAÇÃO GRÁFICA
//
// Teste de colisão em OpenGL
//       
// Marcio Sarroglia Pinho
// pinho@inf.pucrs.br
// **********************************************************************
#include <iostream>
#include <iomanip>
#include <cmath>
#include <ctime>
   
   
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
#include <glut.h>
#endif
 
#include "Ponto.h"
#include "Linha.h"

#include "Temporizador.h"

const int N_LINHAS = 50;

const int MAX_X = 100;  // Coordenada X maxima da janela

int ContChamadas;

Linha Linhas[N_LINHAS];

// **********************************************************************
//  void init(void)
//  Inicializa os parâmetros globais de OpenGL
// **********************************************************************
void init(void)
{
    // Define a cor do fundo da tela (BRANCO)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    srand(unsigned(time(NULL)));
    
    for(int i=0; i< N_LINHAS; i++)
        Linhas[i].geraLinha(MAX_X, 10);
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
    // Define os limites lógicos da área OpenGL dentro da Janela
    glOrtho(0,100,0,100,0,1);

    // Define a área a ser ocupada pela área OpenGL dentro da Janela
    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

}


// **********************************************************************
//
// **********************************************************************
void DesenhaLinhas()
{
    glColor3f(0,1,0);
    for(int i=0; i< N_LINHAS; i++)
        Linhas[i].desenhaLinha();
}

// **********************************************************************
// void DesenhaCenario()
// **********************************************************************
void DesenhaCenario()
{
    Ponto PA, PB, PC, PD;
    ContChamadas = 0;
    resetContadorInt();
    
    // Desenha as linhas do cenário
    glLineWidth(1);
    glColor3f(1,0,0);
    
    for(int i=0; i< N_LINHAS; i++)
    {
        PA.set(Linhas[i].x1, Linhas[i].y1);
        PB.set(Linhas[i].x2, Linhas[i].y2);
        for(int j=0; j< N_LINHAS; j++)
        {
            PC.set(Linhas[j].x1, Linhas[j].y1);
            PD.set(Linhas[j].x2, Linhas[j].y2);
            ContChamadas++;
            if (HaInterseccao(PA, PB, PC, PD))
            {
                Linhas[i].desenhaLinha();
                Linhas[j].desenhaLinha();
            }
        }
    }
}
// **********************************************************************
//  void display( void )
// **********************************************************************
void display( void )
{
    // Limpa a tela com  a cor de fundo
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    DesenhaLinhas();
    DesenhaCenario();
    
    glutSwapBuffers();
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
    
    if (AccumDeltaT > 1.0/30) // fixa a atualização da tela em 30
    {
        AccumDeltaT = 0;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
        cout << "Contador (de Intersecoes Existentes:" << getContadorInt()/2.0 << endl;
        cout << "Contador de Chamadas:" << ContChamadas << endl;
    }
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{
    switch ( key )
    {
    case 27:        // Termina o programa qdo
        exit ( 0 );   // a tecla ESC for pressionada
        break;
    case' ':
            init();
        break;
    default:
        break;
    }
    glutPostRedisplay();
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
        
        break;
    case GLUT_KEY_DOWN:     // Se pressionar DOWN
        
        break;
    case GLUT_KEY_LEFT:       // Se pressionar LEFT
        
        break;
    case GLUT_KEY_RIGHT:     // Se pressionar RIGHT
        
        break;
    default:
        break;
    }
    glutPostRedisplay();
}
void mouse(int button, int state, int x, int y)
{
    glutPostRedisplay();
}
void mouseMove(int x, int y)
{
    glutPostRedisplay();
}
// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "ARGC: " << argc << endl;
    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de título da janela.
    glutCreateWindow    ( "Algorimos de Calculo de Colisao" );

    // executa algumas inicializações
    init ();


    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // será chamada automaticamente quando
    // for necessário redesenhar a janela
    glutDisplayFunc ( display );
    glutIdleFunc ( animate );
    
    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // será chamada automaticamente quando
    // o usuário alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // será chamada automaticamente sempre
    // o usuário pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" será chamada
    // automaticamente sempre o usuário
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );
    //glutMouseFunc(mouse);
    //glutMotionFunc(mouseMove);

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}

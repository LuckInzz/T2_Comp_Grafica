// **********************************************************************
// PUCRS/Escola PolitŽcnica
// COMPUTA‚ÌO GRçFICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//

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
#include <glut.h>
#endif

#include "Ponto.h"
#include "Poligono.h"

#include "Temporizador.h"
Temporizador T;
double AccumDeltaT=0;

Poligono Mapa;
Poligono ConvexHull;
Poligono ConjuntoDePonto;
// Limites logicos da area de desenho
Ponto Min, Max, PontoClicado;

bool desenha = false;
bool FoiClicado = false;

class Faixa{
    vector<int> ArestasNaFaixa;
public:
    void CadastraAresta(int a)
    {
        ArestasNaFaixa.push_back(a);
    }
    int getNroDeArestas()
    {
        return ArestasNaFaixa.size();
    }
    int getAresta(int i)
    {
        return ArestasNaFaixa[i];
    }
};
class ConjuntoDeFaixas{
    vector<Faixa> TodasAsFaixas;

public:
    ConjuntoDeFaixas()
    {

    }
    void CadastraArestaNaFaixa(int f, int a)
    {
        TodasAsFaixas[f].CadastraAresta(a);
    }
    void CriaFaixas(int qtdDeFaixas) // pode ser substitu’da por uma construtora
    {
        for (int i=0; i<qtdDeFaixas; i++)
            TodasAsFaixas.push_back(Faixa());
    }
    Faixa getFaixa(int f)
    {
        return TodasAsFaixas[f];
    }
};

ConjuntoDeFaixas EspacoDividido;
float angulo=0.0;


void ImprimeFaixas()
{
    for (int i=0;i<10;i++)
    {
        cout << "Faixa: " << i << ": ";
        Faixa f;
        f = EspacoDividido.getFaixa(i);
        for (int a=0; a<f.getNroDeArestas();a++)
        {
            cout << f.getAresta(a) << " ";
        }
        cout << endl;

    }
}

// **********************************************************************
//
// **********************************************************************
void init()
{
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    Mapa.LePoligono("PoligonoDeTeste.txt");
    //Mapa.LePoligono("EstadoRS.txt");
    Mapa.obtemLimites(Min,Max);

    Min.x--;Min.y--;
    Max.x++;Max.y++;
    //cout << "Vertices no Vetor: " << Mapa.getNVertices() << endl;
    //GeraConvexHull();
    EspacoDividido.CriaFaixas(10);
    EspacoDividido.CadastraArestaNaFaixa(0,11);
    EspacoDividido.CadastraArestaNaFaixa(3,11);
    ImprimeFaixas();
    exit(0);
}

double nFrames=0;
double TempoTotal=0;
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

    if (AccumDeltaT > 1.0/30) // fixa a atualiza‹o da tela em 30
    {
        AccumDeltaT = 0;
        //angulo+=0.05;
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
    glOrtho(Min.x,Max.x,
            Min.y,Max.y,
            0,1);

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
void DesenhaLinha(Ponto P1, Ponto P2)
{
    glBegin(GL_LINES);
        glVertex3f(P1.x,P1.y,P1.z);
        glVertex3f(P2.x,P2.y,P2.z);
    glEnd();
}
// **********************************************************************
//  void display( void )
//
// **********************************************************************
void display( void )
{

	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites lógicos da área OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1,1,1); // R, G, B  [0..1]
    DesenhaEixos();

    glRotatef(angulo, 0,0,1);
    glLineWidth(2);
    glColor3f(1,1,0); // R, G, B  [0..1]
    Mapa.desenhaPoligono();

    if (FoiClicado == true)
    {
        Ponto Esq;
        Ponto Dir (-1,0);
        Esq = PontoClicado + Dir * 100;
        glColor3f(0,1,0); // R, G, B  [0..1]
        DesenhaLinha(PontoClicado, Esq);
        //F = CalculaFaixa(PontoClicado);

        glColor3f(1,0,0); // R, G, B  [0..1]
        Ponto P1, P2;
        for (int i=0; i < Mapa.getNVertices();i++)
        {
            Mapa.getAresta(i, P1, P2);
            //if(PassaPelaFaixa(i,F))
            if (HaInterseccao(PontoClicado,Esq, P1, P2))
                Mapa.desenhaAresta(i);
        }

    }


    //Mapa.desenhaVertices();

    if (desenha)
        ConvexHull.desenhaPoligono();

    glPointSize(5);
    glColor3f(0,1,0); // R, G, B  [0..1]
    //Mapa.desenhaVertices();

	glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo nœmero de segundos e informa quanto frames
// se passaram neste per’odo.
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
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
		case GLUT_KEY_UP:       // Se pressionar UP
			glutFullScreen ( ); // Vai para Full Screen
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
// Esta fun‹o captura o clique do botao direito do mouse sobre a ‡rea de
// desenho e converte a coordenada para o sistema de referncia definido
// na glOrtho (ver fun‹o reshape)
// Este c—digo Ž baseado em http://hamala.se/forums/viewtopic.php?t=20
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
    PontoClicado.imprime("- Ponto no universo: ", "\n");
    FoiClicado = true;
}


// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de titulo da janela.
    glutCreateWindow    ( "Poligonos em OpenGL" );

    // executa algumas inicializações
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // será chamada automaticamente quando
    // for necessário redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida‹o da tela. A funcao "display"
    // será chamada automaticamente sempre que a
    // m‡quina estiver ociosa (idle)
    glutIdleFunc(animate);

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

    glutMouseFunc(Mouse);

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}

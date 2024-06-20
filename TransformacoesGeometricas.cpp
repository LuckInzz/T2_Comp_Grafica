// **********************************************************************
// PUCRS/Escola Polit�cnica
// COMPUTA��O GR�FICA
//
// Programa b�sico para criar aplicacoes 3D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

#define PAREDE1 1
#define PAREDE2 2
#define CHAO 0
#define POS_INICIAL_PLAYER 3

#define WALL_HEIGHT 2.70f
#define WALL_THICKNESS 0.25f

#include <iostream>
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

#include "Temporizador.h"
#include "ListaDeCoresRGB.h"
#include "Ponto.h"
#include "Instancia.h"

void desenhaPersonagem();
void DesenhaLadrilho(int corB, int corD);

Temporizador T;
double AccumDeltaT = 0;

GLfloat AspectRatio, angulo = 0;

// Controle do modo de projecao
// 0: Projecao Paralela Ortografica; 1: Projecao Perspectiva
// A funcao "PosicUser" utiliza esta variavel. O valor dela eh alterado
// pela tecla 'p'
int ModoDeProjecao = 1;

// Controle do modo de projecao
// 0: Wireframe; 1: Faces preenchidas
// A funcao "Init" utiliza esta variavel. O valor dela eh alterado
// pela tecla 'e'
int ModoDeExibicao = 1;

Instancia Personagens[10];

double nFrames = 0;
double TempoTotal = 0;
Ponto CantoEsquerdo = Ponto(-20, 0, -10);
Ponto OBS;
Ponto ALVO;
Ponto VetorAlvo;

vector<vector<int>> labirinto;
int linhas, colunas;

// **********************************************************************
//  void init(void)
//        Inicializa os parametros globais de OpenGL
// **********************************************************************
void init(void)
{
    glClearColor(1.0f, 1.0f, 0.0f, 1.0f); // Fundo de tela preto

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    // glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    // glShadeModel(GL_SMOOTH);
    glShadeModel(GL_FLAT);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    if (ModoDeExibicao) // Faces Preenchidas??
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    OBS = Ponto(ALVO.x, ALVO.y + 0.5, ALVO.z + 10); // Posição inicial da camera
    ALVO = Ponto(0, 0, 0);                          // Posição inicial do personagem
    VetorAlvo = ALVO - OBS;
}

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

    if (AccumDeltaT > 1.0 / 30) // fixa a atualiza��o da tela em 30
    {
        AccumDeltaT = 0;
        angulo += 1;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: " << TempoTotal << " segundos. ";
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames / TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}

void readMap(const char *filename)
{
    ifstream file(filename);
    if (file.is_open())
    {
        file >> linhas >> colunas;
        labirinto.resize(linhas, vector<int>(colunas));
        for (int i = 0; i < linhas; ++i)
        {
            for (int j = 0; j < colunas; ++j)
            {
                file >> labirinto[i][j];
            }
        }
        file.close();
    }
    else
    {
        cout << "Unable to open file";
    }
}

void drawLabirinto()
{
    for (int i = 0; i < linhas; ++i)
    {
        for (int j = 0; j < colunas; ++j)
        {
            glPushMatrix();
            glTranslatef(CantoEsquerdo.x + j, 0, CantoEsquerdo.z + i);
            switch (labirinto[i][j])
            {
            case PAREDE1:
                glColor3f(0.5f, 0.5f, 0.5f); // Cinza para paredes
                glPushMatrix();
                glScalef(1.0f, WALL_HEIGHT, WALL_THICKNESS);
                glutSolidCube(1.0);
                glPopMatrix();
                break;
            case PAREDE2:
                glColor3f(0.5f, 0.5f, 0.5f); // Cinza para paredes
                glPushMatrix();
                glRotatef(90, 0, 0.1, 0);
                glScalef(1.0f, WALL_HEIGHT, WALL_THICKNESS);
                glutSolidCube(1.0);
                glPopMatrix();
                break;
            case CHAO:
                DesenhaLadrilho(DarkBrown, DarkBrown);
                break;
            case POS_INICIAL_PLAYER:
                DesenhaLadrilho(Green, Green);
                break;
            // Adicione mais cases para outros tipos de células
            default:
                break;
            }
            glPopMatrix();
        }
    }
}

void criaPersonagem()
{
    Personagens[0].Posicao = ALVO;
    Personagens[0].Escala = Ponto(1, 1);
    Personagens[0].Rotacao = 0;
    Personagens[0].IdDoModelo = 0;
    Personagens[0].modelo = desenhaPersonagem;
    Personagens[0].Pivot = Ponto(2.5, 0);
    Personagens[0].Direcao = Ponto(0, 1); // direcao do movimento para cima
    Personagens[0].Direcao.rotacionaZ(0); // direcao alterada para a direita
    Personagens[0].Velocidade = 7;        // move-se a 7 m/s
}

void desenhaPersonagem()
{
    glPushMatrix();
    glTranslatef(ALVO.x, ALVO.y, ALVO.z);
    //  glRotatef(angulo,0,1,0);
    glColor3f(0.5f, 0.0f, 0.5f); // Roxo
    glScaled(0.5, 1, 0.5);
    glutSolidCube(2);
    glPopMatrix();
}

// **********************************************************************
//  void DesenhaCubo()
// **********************************************************************
void DesenhaCubo(float tamAresta)
{
    glBegin(GL_QUADS);
    // Front Face
    glNormal3f(0, 0, 1);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);
    // Back Face
    glNormal3f(0, 0, -1);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    // Top Face
    glNormal3f(0, 1, 0);
    glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    // Bottom Face
    glNormal3f(0, -1, 0);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    // Right face
    glNormal3f(1, 0, 0);
    glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    // Left Face
    glNormal3f(-1, 0, 0);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glEnd();
}
void DesenhaParalelepipedo()
{
    glPushMatrix();
    glTranslatef(0, 0, -1);
    glScalef(1, 1, 2);
    glutSolidCube(2);
    // DesenhaCubo(1);
    glPopMatrix();
}

// **********************************************************************
// void DesenhaLadrilho(int corBorda, int corDentro)
// Desenha uma c�lula do piso.
// Eh possivel definir a cor da borda e do interior do piso
// O ladrilho tem largula 1, centro no (0,0,0) e est� sobre o plano XZ
// **********************************************************************
void DesenhaLadrilho(int corBorda, int corDentro)
{
    defineCor(corDentro); // desenha QUAD preenchido
    // glColor3f(1,1,1);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glEnd();

    defineCor(corBorda);
    // glColor3f(0,1,0);

    glBegin(GL_LINE_STRIP);
    glNormal3f(0, 1, 0);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glEnd();
}

// **********************************************************************
//
//
// **********************************************************************
void DesenhaPiso()
{
    srand(100); // usa uma semente fixa para gerar sempre as mesma cores no piso
    glPushMatrix();
    glTranslated(CantoEsquerdo.x, CantoEsquerdo.y, CantoEsquerdo.z);
    for (int x = 0; x < linhas; x++)
    {
        glPushMatrix();
        for (int z = 0; z < colunas; z++)
        {
            DesenhaLadrilho(DarkBrown, DarkBrown);
            glTranslated(0, 0, 1);
        }
        glPopMatrix();
        glTranslated(1, 0, 0);
    }
    glPopMatrix();
}

void DesenhaParedao()
{
    glPushMatrix();
    glRotatef(90, 0, 0, 1);
    DesenhaPiso();
    glPopMatrix();
}
void DesenhaChao()
{
    glPushMatrix();
    glTranslated(-20, 0, 0);
    DesenhaPiso();
    glPopMatrix();
    glPushMatrix();
    glTranslated(20, 0, 0);
    DesenhaPiso();
    glPopMatrix();
}
// **********************************************************************
//  void DefineLuz(void)
// **********************************************************************
void DefineLuz(void)
{
    // Define cores para um objeto dourado
    // GLfloat LuzAmbiente[]   = {0.0, 0.0, 0.0 } ;
    GLfloat LuzAmbiente[] = {0.4, 0.4, 0.4};
    GLfloat LuzDifusa[] = {0.7, 0.7, 0.7};
    // GLfloat LuzDifusa[]   = {0, 0, 0};
    GLfloat PosicaoLuz0[] = {0.0f, 3.0f, 5.0f}; // Posi��o da Luz
    GLfloat LuzEspecular[] = {0.9f, 0.9f, 0.9};
    // GLfloat LuzEspecular[] = {0.0f, 0.0f, 0.0 };

    GLfloat Especularidade[] = {1.0f, 1.0f, 1.0f};

    // ****************  Fonte de Luz 0

    glEnable(GL_COLOR_MATERIAL);

    // Habilita o uso de ilumina��o
    glEnable(GL_LIGHTING);

    // Ativa o uso da luz ambiente
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LuzAmbiente);
    // Define os parametros da luz n�mero Zero
    glLightfv(GL_LIGHT0, GL_AMBIENT, LuzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LuzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, LuzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, PosicaoLuz0);
    glEnable(GL_LIGHT0);

    // Ativa o "Color Tracking"
    glEnable(GL_COLOR_MATERIAL);

    // Define a reflectancia do material
    glMaterialfv(GL_FRONT, GL_SPECULAR, Especularidade);

    // Define a concentra��oo do brilho.
    // Quanto maior o valor do Segundo parametro, mais
    // concentrado ser� o brilho. (Valores v�lidos: de 0 a 128)
    glMateriali(GL_FRONT, GL_SHININESS, 128);
}
// **********************************************************************

void MygluPerspective(float fieldOfView, float aspect, float zNear, float zFar)
{
    // https://stackoverflow.com/questions/2417697/gluperspective-was-removed-in-opengl-3-1-any-replacements/2417756#2417756
    //  The following code is a fancy bit of math that is equivilant to calling:
    //  gluPerspective( fieldOfView/2.0f, width/height , 0.1f, 255.0f )
    //  We do it this way simply to avoid requiring glu.h
    // GLfloat zNear = 0.1f;
    // GLfloat zFar = 255.0f;
    // GLfloat aspect = float(width)/float(height);
    GLfloat fH = tan(float(fieldOfView / 360.0f * 3.14159f)) * zNear;
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}
// **********************************************************************
//  void PosicUser()
// **********************************************************************
void PosicUser()
{

    // Define os par�metros da proje��o Perspectiva
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define o volume de visualiza��o sempre a partir da posicao do
    // observador
    if (ModoDeProjecao == 0)
        glOrtho(-10, 10, -10, 10, 0, 20); // Projecao paralela Orthografica
    else
        MygluPerspective(60, AspectRatio, 0.1, 50); // Projecao perspectiva

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(OBS.x, OBS.y, OBS.z,    // Posi��o do Observador
              ALVO.x, ALVO.y, ALVO.z, // Posi��o do Alvo
              0.0, 1.0, 0.0);
}
// **********************************************************************
//  void reshape( int w, int h )
//		trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape(int w, int h)
{

    // Evita divis�o por zero, no caso de uam janela com largura 0.
    if (h == 0)
        h = 1;
    // Ajusta a rela��o entre largura e altura para evitar distor��o na imagem.
    // Veja fun��o "PosicUser".
    AspectRatio = 1.0f * w / h;
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Seta a viewport para ocupar toda a janela
    glViewport(0, 0, w, h);
    // cout << "Largura" << w << endl;

    PosicUser();
}

// **********************************************************************
//  void display( void )
// **********************************************************************
float PosicaoZ = -30;

void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    DefineLuz();

    PosicUser();

    glMatrixMode(GL_MODELVIEW);

    // DesenhaChao();
    drawLabirinto();
    criaPersonagem();
    desenhaPersonagem();

    /*glPushMatrix();
    glTranslatef(-4.0f, 1.0f, 0.0f);
    glRotatef(angulo, 0, 1, 0);
    glColor3f(0.6156862745, 0.8980392157, 0.9803921569); // Azul claro
    // glutSolidCube(2);
    glPopMatrix();*/

    // glColor3f(0.8,0.8,0);
    // glutSolidTeapot(2);
    // DesenhaParedao();

    glutSwapBuffers();
}

// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
//
// **********************************************************************
void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:     // Termina o programa qdo
        exit(0); // a tecla ESC for pressionada
        break;
    case 'p':
        ModoDeProjecao = !ModoDeProjecao;
        glutPostRedisplay();
        break;
    case 'e':
        ModoDeExibicao = !ModoDeExibicao;
        init();
        glutPostRedisplay();
        break;
    case 'w': // Andar para frente
        ALVO.z--;
        OBS.z--;
        break;
    case 's': // Andar para trás
        ALVO.z++;
        OBS.z++;
        break;
    case 'a': // Andar para esquerda
        ALVO.x--;
        OBS.x--;
        break;
    case 'd': // Andar para direita
        ALVO.x++;
        OBS.x++;
        break;
    default:
        cout << key;
        break;
    }
}

// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys(int a_keys, int x, int y)
{
    switch (a_keys)
    {
    case GLUT_KEY_UP:     // When Up Arrow Is Pressed...
        glutFullScreen(); // Go Into Full Screen Mode
        break;
    case GLUT_KEY_DOWN: // When Down Arrow Is Pressed...
        glutInitWindowPosition(0, 0);
        glutInitWindowSize(700, 700);
        break;
    case GLUT_KEY_LEFT:
        // ALVO.rotacionaY(-10);
        OBS.rotacionaY(-10);
        break;
    case GLUT_KEY_RIGHT:
        // ALVO.rotacionaY(10);
        OBS.rotacionaY(10);
        break;
        // case GLUT_KEY_RIGHT: // When Up Arrow Is Pressed...
        //  PosicaoLuz0[0]++; // Go Into Full Screen Mode
        // break;

    default:
        break;
    }
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
//
// **********************************************************************
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(700, 700);
    glutCreateWindow("Computacao Grafica - Exemplo Basico 3D");

    init();
    // system("pwd");

    readMap("MatrizMapa.txt");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(arrow_keys);
    glutIdleFunc(animate);

    glutMainLoop();
    return 0;
}

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
#define JANELA 3
#define PORTA 4
#define POS_INICIAL_PLAYER 9

#define WALL_HEIGHT 2.7
#define WALL_THICKNESS 1 // 0.25

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

int ModoDeProjecao = 1;
int ModoDeExibicao = 1;

Instancia Personagens[10];

double nFrames = 0;
double TempoTotal = 0;
Ponto CantoEsquerdo = Ponto(0, 0, 0);
Ponto OBS;
Ponto ALVO;
Ponto VetorAlvo;

vector<vector<int>> labirinto;
int linhas, colunas;
float posAlvoX = 48.0f, posAlvoY = 1.0f, posAlvoZ = 48.0f; // Posição inicial do personagem
float anguloDoPersonagem = 0.0f;
int ModoDeCamera = 0;
float anguloDaCamera = 0.0f;
float cameraDist1 = 0.1f;
float cameraDist3 = 10.0f;

// Caixa para detectar colisão
struct BoundingBox {
    Ponto min;
    Ponto max;
};

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_FLAT);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    if (ModoDeExibicao)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ALVO = Ponto(posAlvoX, posAlvoY, posAlvoZ);
    OBS = Ponto(ALVO.x, ALVO.y, ALVO.z + 10);
    VetorAlvo = ALVO - OBS;
}

void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0 / 30)
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

BoundingBox getBoundingBox(Ponto pos, float width, float height, float depth) 
{
    BoundingBox box;
    box.min = Ponto(pos.x - width / 2, pos.y - height / 2, pos.z - depth / 2);
    box.max = Ponto(pos.x + width / 2, pos.y + height / 2, pos.z + depth / 2);
    return box;
}

bool detectCollision(BoundingBox a, BoundingBox b) 
{
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
           (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
           (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

bool detectaColisao3D(Ponto personagemPos) {
    float personagemWidth = 0.4;
    float personagemHeight = 1.2;
    float personagemDepth = 0.4;
    
    BoundingBox personagemBox = getBoundingBox(personagemPos, personagemWidth, personagemHeight, personagemDepth);
    
    for (int i = 0; i < linhas; ++i) {
        for (int j = 0; j < colunas; ++j) {
            if (labirinto[i][j] == PAREDE1 || labirinto[i][j] == PAREDE2) {
                Ponto paredePos = Ponto(CantoEsquerdo.x + j, CantoEsquerdo.y + 1, CantoEsquerdo.z + i);
                float paredeWidth = 1.0;
                float paredeHeight = WALL_HEIGHT;
                float paredeDepth = WALL_THICKNESS;
                
                if (labirinto[i][j] == PAREDE2) {
                    paredeDepth = 1.0;
                    paredeWidth = WALL_THICKNESS;
                }

                BoundingBox paredeBox = getBoundingBox(paredePos, paredeWidth, paredeHeight, paredeDepth);
                
                if (detectCollision(personagemBox, paredeBox)) {
                    cout << "PERSONAGEM BATEU NA PAREDE!" << endl;
                    return true;
                }
            }
        }
    }
    return false;
}

void drawLabirinto()
{
    for (int i = 0; i < linhas; ++i)
    {
        for (int j = 0; j < colunas; ++j)
        {
            glPushMatrix();
            glTranslatef(CantoEsquerdo.x + j, CantoEsquerdo.y + 1, CantoEsquerdo.z + i);
            switch (labirinto[i][j])
            {
            case PAREDE1:
                glColor3f(0.5f, 0.5f, 0.5f);
                glPushMatrix();
                glScaled(1, WALL_HEIGHT, WALL_THICKNESS);
                glutSolidCube(1.0);
                glPopMatrix();
                break;
            case PAREDE2:
                glColor3f(0.5f, 0.5f, 0.5f);
                glPushMatrix();
                glRotatef(90, 0, 0.1, 0);
                glScaled(1, WALL_HEIGHT, WALL_THICKNESS);
                glutSolidCube(1.0);
                glPopMatrix();
                break;
            case JANELA:
                glColor3f(0.5f, 0.5f, 0.5f);
                glPushMatrix();
                glTranslated(0, -WALL_HEIGHT / 3, 0);
                glScaled(1, WALL_HEIGHT / 3, WALL_THICKNESS);
                glutSolidCube(1.0);
                glPopMatrix();

                glColor3f(0.5f, 0.5f, 0.5f);
                glPushMatrix();
                glTranslated(0, WALL_HEIGHT / 3, 0);
                glScaled(1, WALL_HEIGHT / 3, WALL_THICKNESS);
                glutSolidCube(1.0);
                glPopMatrix();
                break;
            case PORTA:
                glColor3f(0.5f, 0.5f, 0.5f);
                glPushMatrix();
                glTranslated(0, 1.05, 0);
                glScaled(1, 0.6, WALL_THICKNESS);
                glutSolidCube(1.0);
                glPopMatrix();
                break;
            default:
                break;
            }
            glPopMatrix();
        }
    }
}

void desenhaSeta()
{
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, 1.5f);
    glVertex3f(0.2f, 0.0f, 0.5f);
    glVertex3f(-0.2f, 0.0f, 0.5f);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.8f, 0.0f, 0.0f);
    glVertex3f(0.05f, 0.0f, 0.5f);
    glVertex3f(-0.05f, 0.0f, 0.5f);
    glVertex3f(-0.05f, 0.0f, -0.5f);
    glVertex3f(0.05f, 0.0f, -0.5f);
    glEnd();
}

void desenhaPersonagem()
{
    glPushMatrix();
    glTranslatef(ALVO.x, ALVO.y - 1, ALVO.z);
    glRotatef(anguloDoPersonagem, 0.0f, 1.0f, 0.0f);

    glColor3f(0.5f, 0.0f, 0.5f);
    glScaled(0.4, 1.2, 0.4);
    glutSolidCube(2);

    glPopMatrix();

    glPushMatrix();
    glTranslatef(ALVO.x, ALVO.y - 1, ALVO.z - 0.1);
    glRotatef(anguloDoPersonagem + 180, 0.0f, 1.0f, 0.0f);

    desenhaSeta();
    glPopMatrix();
}

void DesenhaCubo(float tamAresta)
{
    glBegin(GL_QUADS);
    glNormal3f(0, 0, 1);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glNormal3f(0, 0, -1);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glNormal3f(0, 1, 0);
    glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glNormal3f(0, -1, 0);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glNormal3f(1, 0, 0);
    glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
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
    glPopMatrix();
}

void DesenhaLadrilho(int corBorda, int corDentro)
{
    defineCor(corDentro);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glEnd();

    defineCor(corBorda);

    glBegin(GL_LINE_STRIP);
    glNormal3f(0, 1, 0);
    glVertex3f(-0.5f, 0.0f, -0.5f);
    glVertex3f(-0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, 0.5f);
    glVertex3f(0.5f, 0.0f, -0.5f);
    glEnd();
}

void DesenhaChaoV2()
{
    glColor3f(0.4f, 0.2f, 0.1f);
    glPushMatrix();
    glScaled(1, 1, 1);
    glutSolidCube(1.0);
    glPopMatrix();
}

void DesenhaPiso()
{
    srand(100);
    glPushMatrix();
    glTranslated(CantoEsquerdo.x, CantoEsquerdo.y, CantoEsquerdo.z);
    for (int x = 0; x < linhas; x++)
    {
        glPushMatrix();
        for (int z = 0; z < colunas; z++)
        {
            DesenhaChaoV2();
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

void DefineLuz(void)
{
    GLfloat LuzAmbiente[] = {0.4, 0.4, 0.4};
    GLfloat LuzDifusa[] = {0.7, 0.7, 0.7};
    GLfloat PosicaoLuz0[] = {0.0f, 3.0f, 5.0f};
    GLfloat LuzEspecular[] = {0.9f, 0.9f, 0.9};

    GLfloat Especularidade[] = {1.0f, 1.0f, 1.0f};

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LuzAmbiente);
    glLightfv(GL_LIGHT0, GL_AMBIENT, LuzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LuzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, LuzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, PosicaoLuz0);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glMaterialfv(GL_FRONT, GL_SPECULAR, Especularidade);
    glMateriali(GL_FRONT, GL_SHININESS, 128);
}

void MygluPerspective(float fieldOfView, float aspect, float zNear, float zFar)
{
    GLfloat fH = tan(float(fieldOfView / 360.0f * 3.14159f)) * zNear;
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void PosicUser()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (ModoDeProjecao == 0)
    {
        glOrtho(-10, 10, -10, 10, 0, 20);
    }
    else
    {
        MygluPerspective(60, AspectRatio, 0.1, 50);
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float cameraD = cameraDist1;

    if (ModoDeCamera == 0)
    {
        cameraD = cameraDist1;
        OBS.x = posAlvoX;
        OBS.z = posAlvoZ;
        OBS.y = posAlvoY;
    }
    else if (ModoDeCamera == 1)
    {
        cameraD = cameraDist3;
    }
    else if (ModoDeCamera == 2)
    {
        OBS.x = posAlvoX;
        OBS.z = posAlvoZ;
        OBS.y = posAlvoY + 25;

        ALVO.x = posAlvoX;
        ALVO.y = posAlvoY;
        ALVO.z = posAlvoZ;

        gluLookAt(OBS.x, OBS.y, OBS.z, ALVO.x, ALVO.y, ALVO.z, 0.0, 0.0, -1.0);
        return;
    }

    OBS.x = posAlvoX - cameraD * sin(anguloDaCamera * M_PI / 180.0f);
    OBS.z = posAlvoZ + cameraD * cos(anguloDaCamera * M_PI / 180.0f);

    ALVO.x = posAlvoX;
    ALVO.y = posAlvoY;
    ALVO.z = posAlvoZ;

    gluLookAt(OBS.x, OBS.y, OBS.z, ALVO.x, ALVO.y, ALVO.z, 0.0, 1.0, 0.0);
}

void reshape(int w, int h)
{
    if (h == 0)
        h = 1;
    AspectRatio = 1.0f * w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);

    PosicUser();
}

float PosicaoZ = -30;

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    PosicUser();
    glMatrixMode(GL_MODELVIEW);
    DesenhaPiso();
    drawLabirinto();
    desenhaPersonagem();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    float stepSize = 1.0f;
    float angleStep = 5.0f;
    Ponto novaPosicao = Ponto(posAlvoX, posAlvoY, posAlvoZ);

    switch (key)
    {
    case 27:
        exit(0);
        break;
    case 'p':
        ModoDeCamera++;
        if (ModoDeCamera > 2)
            ModoDeCamera = 0;
        break;
    case 'e':
        ModoDeExibicao = !ModoDeExibicao;
        init();
        glutPostRedisplay();
        break;
    case 'w':
        novaPosicao.x += stepSize * sin(anguloDaCamera * M_PI / 180.0f);
        novaPosicao.z -= stepSize * cos(anguloDaCamera * M_PI / 180.0f);
        break;
    case 's':
        novaPosicao.x -= stepSize * sin(anguloDaCamera * M_PI / 180.0f);
        novaPosicao.z += stepSize * cos(anguloDaCamera * M_PI / 180.0f);
        break;
    case 'a':
        anguloDaCamera -= angleStep;
        anguloDoPersonagem += angleStep;
        break;
    case 'd':
        anguloDaCamera += angleStep;
        anguloDoPersonagem -= angleStep;
        break;
    default:
        cout << key;
        break;
    }

    if (!detectaColisao3D(novaPosicao))
    {
        posAlvoX = novaPosicao.x;
        posAlvoZ = novaPosicao.z;
    }
}

void arrow_keys(int a_keys, int x, int y)
{
    switch (a_keys)
    {
    case GLUT_KEY_UP:
        glutFullScreen();
        break;
    case GLUT_KEY_DOWN:
        glutInitWindowPosition(0, 0);
        glutInitWindowSize(700, 700);
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(700, 700);
    glutCreateWindow("Computacao Grafica - Exemplo Basico 3D");

    init();

    readMap("MatrizMapa.txt");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(arrow_keys);
    glutIdleFunc(animate);

    glutMainLoop();
    return 0;
}

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
#define PISO 0
#define JANELA 3
#define PORTA 4
#define CADEIRA 5
#define MESA 6
#define POS_INICIAL_PLAYER 8
#define POS_FINAL_LABIRINTO 9

#define WALL_HEIGHT 2.7
#define WALL_THICKNESS 1 // 0.25

// #include <GL/glew.h>
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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cmath> // Para M_PI
#define PI 3.14159265358979323846

// #include "TextureClass.h"

void desenhaPersonagem();
void DesenhaLadrilho(int corB, int corD);
void initPositions();
void DesenharMensagemFinal(bool i);
void encerrarJogo(int value);
void DesenhaCuboComTextura(float aresta, GLuint tex);

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
int TOTAL_PONTOS = 10;    // total de pontos inicial
int TOTAL_ENERGIA = 2000; // energia inicial do jogador

double nFrames = 0;
double TempoTotal = 0;
Ponto CantoEsquerdo = Ponto(0, 0, 0);
Ponto OBS;
Ponto ALVO;
Ponto VetorAlvo;

vector<vector<int>> labirinto;
int linhas, colunas;
float posAlvoX = 48.0f, posAlvoY = 1.0f, posAlvoZ = 48.0f; // Posição inicial do personagem
int posInicialX, posInicialZ, posFinalX, posFinalZ;
float anguloDoPersonagem = 0.0f;
float anguloDoInimigo = 0.0f;
int ModoDeCamera = 0;
float anguloDaCamera = 0.0f;
float alturaCamera1 = 0.8;
float cameraDist1 = 0.01f; // Distância da câmera do personagem em 1º pessoa
float cameraDist3 = 20.0f; // Distância da câmera do personagem em 3º pessoa

int inimigoWidth = 1;
int inimigoHeight = 1;
int inimigoDepth = 1;

int personagemWidth = 0.8;
int personagemHeight = 0.8;
int personagemDepth = 0.8;

bool emMovimento = true;
float stepSize = 0.05f;
float angleStep = 5.0f;

GLuint texParede, texPiso, texEnergy;

std::vector<Ponto> posicoesInimigos;
std::vector<Ponto> posicoesCapsulasEnergia;

typedef struct // Struct para armazenar coordenadas de textura
{
    float U, V;
    void Set(float u, float v)
    {
        U = u;
        V = v;
    }
} TTexCoord;

// Caixa para detectar colisão
struct BoundingBox
{
    Ponto min;
    Ponto max;
};

typedef struct // Struct para armazenar um ponto
{
    float X, Y, Z;
    void Set(float x, float y, float z)
    {
        X = x;
        Y = y;
        Z = z;
    }
} TPoint;

typedef struct // Struct para armazenar um triângulo
{
    TPoint P1, P2, P3;
    TTexCoord T1, T2, T3; // Coordenadas de textura
} TTriangle;

class Objeto3D
{
    TTriangle *faces;    // vetor de faces
    unsigned int nFaces; // Variavel que armazena o numero de faces do objeto
public:
    Objeto3D()
    {
        nFaces = 0;
        faces = nullptr;
    }

    unsigned int getNFaces() const
    {
        return nFaces;
    }

    // Método para ler um arquivo .tri e carregar o objeto 3D
    void LeObjeto(const char *Nome)
    {
        std::ifstream arquivo(Nome);
        if (!arquivo)
        {
            std::cerr << "Erro ao abrir o arquivo " << Nome << std::endl;
            exit(1);
        }

        std::vector<TTriangle> tempFaces;
        float x, y, z, nx, ny, nz;
        while (arquivo >> x >> y >> z >> nx >> ny >> nz)
        {
            TTriangle tri;
            tri.P1.Set(x, y, z);
            tri.T1.Set(0.0f, 0.0f); // Adiciona coordenadas de textura padrão

            arquivo >> x >> y >> z >> nx >> ny >> nz;
            tri.P2.Set(x, y, z);
            tri.T2.Set(1.0f, 0.0f); // Adiciona coordenadas de textura padrão

            arquivo >> x >> y >> z >> nx >> ny >> nz;
            tri.P3.Set(x, y, z);
            tri.T3.Set(0.5f, 1.0f); // Adiciona coordenadas de textura padrão

            tempFaces.push_back(tri);
        }

        nFaces = tempFaces.size();
        faces = new TTriangle[nFaces];
        std::copy(tempFaces.begin(), tempFaces.end(), faces);

        arquivo.close();
    }

    // Método para exibir o objeto 3D usando OpenGL
    void ExibeObjeto() const
    {
        // glBindTexture(GL_TEXTURE_2D, texParede); // Ajuste conforme a textura desejada
        // glEnable(GL_TEXTURE_2D);

        glBegin(GL_TRIANGLES);
        for (unsigned int i = 0; i < nFaces; ++i)
        {
            glTexCoord2f(faces[i].T1.U, faces[i].T1.V);
            glVertex3f(faces[i].P1.X, faces[i].P1.Y, faces[i].P1.Z);

            glTexCoord2f(faces[i].T2.U, faces[i].T2.V);
            glVertex3f(faces[i].P2.X, faces[i].P2.Y, faces[i].P2.Z);

            glTexCoord2f(faces[i].T3.U, faces[i].T3.V);
            glVertex3f(faces[i].P3.X, faces[i].P3.Y, faces[i].P3.Z);
        }
        glEnd();

        // glDisable(GL_TEXTURE_2D);
    }

    // Método para liberar memória alocada
    ~Objeto3D()
    {
        delete[] faces;
    }
};

Objeto3D capsulaEnergia, cadeira, mesa, estatua, inimigo, personagem;

// **********************************************************************
//  void init(void)
//        Inicializa os parametros globais de OpenGL
// **********************************************************************
void init(void)
{
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // Fundo de tela azul claro

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);

    glEnable(GL_TEXTURE_2D); // Habilita textura
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    // glGenTextures(1, &texParede);

    // glShadeModel(GL_SMOOTH);
    glShadeModel(GL_FLAT);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    if (ModoDeExibicao) // Faces Preenchidas??
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    ALVO = Ponto(posInicialX, posAlvoY, posInicialZ); // Posição inicial do personagem
    OBS = Ponto(ALVO.x, ALVO.y, ALVO.z + 10);         // Posição inicial da camera
    VetorAlvo = ALVO - OBS;
}

// **********************************************************************
//
// **********************************************************************

// Função para carregar uma textura
GLuint carregarTextura(GLuint id, std::string filepath)
{
    unsigned char *image;
    int largura, altura, canais;

    image = stbi_load(filepath.c_str(), &largura, &altura, &canais, 4);

    // Configura os parâmetros de textura
    if (image)
    {
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, largura, altura, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        return id;
        stbi_image_free(image);
    }
    else
    {
        cout << "Não foi possivel carregar a textura!" << filepath.c_str() << endl;
    }
}

// Inicializar texturas
void inicializarTexturas()
{
    glGenTextures(1, &texParede);
    glGenTextures(1, &texPiso);
    // glGenTextures(1, &texEnergy);

    texParede = carregarTextura(texParede, "Parede.jpg");
    texPiso = carregarTextura(texPiso, "Piso.jpg");
    // texEnergy = carregarTextura(texEnergy, "Monster.jpg");

    if (texParede == 0 || texPiso == 0)
    {
        std::cerr << "Erro ao carregar uma ou mais texturas!" << std::endl;
        exit(EXIT_FAILURE); // Sai do programa se uma textura não foi carregada corretamente
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

bool detectaColisao(Ponto pos)
{
    BoundingBox personagemBox = getBoundingBox(pos, personagemWidth, personagemHeight, personagemDepth);

    for (int i = 0; i < linhas; ++i)
    {
        for (int j = 0; j < colunas; ++j)
        {
            // Verificar colisão com paredes
            if (labirinto[i][j] == PAREDE1 || labirinto[i][j] == JANELA)
            {
                Ponto paredePos = Ponto(CantoEsquerdo.x + j, CantoEsquerdo.y + 1, CantoEsquerdo.z + i);
                float paredeWidth = 1.0;
                float paredeHeight = WALL_HEIGHT;
                float paredeDepth = WALL_THICKNESS;

                BoundingBox paredeBox = getBoundingBox(paredePos, paredeWidth, paredeHeight, paredeDepth);

                if (detectCollision(personagemBox, paredeBox))
                {
                    cout << "PERSONAGEM BATEU EM UMA PAREDE!" << endl;
                    return true;
                }
            }

            // Verificar colisão com cadeiras
            /*if (labirinto[i][j] == CADEIRA)
            {
                Ponto cadeiraPos = Ponto(CantoEsquerdo.x + j, CantoEsquerdo.y + 1, CantoEsquerdo.z + i);
                float cadeiraWidth = 0.5;
                float cadeiraHeight = 0.5;
                float cadeiraDepth = 0.5;

                BoundingBox cadeiraBox = getBoundingBox(cadeiraPos, cadeiraWidth, cadeiraHeight, cadeiraDepth);

                if (detectCollision(personagemBox, cadeiraBox))
                {
                    cout << "PERSONAGEM BATEU EM UMA CADEIRA!" << endl;
                    return true;
                }
            }*/

            // Verificar colisão com mesas
            if (labirinto[i][j] == MESA)
            {
                Ponto mesaPos = Ponto(CantoEsquerdo.x + j, CantoEsquerdo.y + 1, CantoEsquerdo.z + i);
                float mesaWidth = 1.0;
                float mesaHeight = 0.5;
                float mesaDepth = 1.5;

                BoundingBox mesaBox = getBoundingBox(mesaPos, mesaWidth, mesaHeight, mesaDepth);

                if (detectCollision(personagemBox, mesaBox))
                {
                    cout << "PERSONAGEM BATEU EM UMA MESA!" << endl;
                    return true;
                }
            }
        }
    }

    return false;
}

void atualizaCamera()
{
    float alvoX = posAlvoX + sin(anguloDaCamera * M_PI / 180.0f);
    float alvoZ = posAlvoZ - cos(anguloDaCamera * M_PI / 180.0f);
    gluLookAt(posAlvoX, posAlvoY, posAlvoZ, alvoX, posAlvoY, alvoZ, 0.0f, 1.0f, 0.0f);
}

void animate()
{
    if (emMovimento)
    {
        TOTAL_ENERGIA -= 0.1;
        Ponto novaPosicao = Ponto(posAlvoX, posAlvoY, posAlvoZ);

        // Calcula a nova posição
        novaPosicao.x += stepSize * sin(anguloDaCamera * M_PI / 180.0f);
        novaPosicao.z -= stepSize * cos(anguloDaCamera * M_PI / 180.0f);

        // Verifica colisão antes de atualizar a posição
        if (!detectaColisao(novaPosicao))
        {
            posAlvoX = novaPosicao.x;
            posAlvoZ = novaPosicao.z;
        }

        atualizaCamera();
        glutPostRedisplay();
    }

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
            glTranslatef(CantoEsquerdo.x + j, CantoEsquerdo.y + 0.9, CantoEsquerdo.z + i);
            switch (labirinto[i][j])
            {
            case PAREDE1:
                glEnable(GL_TEXTURE_2D);
                // glBindTexture(GL_TEXTURE_2D, texParede);
                glPushMatrix();
                glScaled(1, WALL_HEIGHT, WALL_THICKNESS);
                DesenhaCuboComTextura(1, texParede);
                glPopMatrix();
                glDisable(GL_TEXTURE_2D);
                break;
            case JANELA:
                // Parte inferior da janela
                glEnable(GL_TEXTURE_2D);
                // glBindTexture(GL_TEXTURE_2D, texParede);
                glPushMatrix();
                glTranslated(0, -WALL_HEIGHT / 3, 0);         // Translação para a parte inferior
                glScaled(1, WALL_HEIGHT / 3, WALL_THICKNESS); // Escalamento para 1/3 da altura da parede
                DesenhaCuboComTextura(1, texParede);
                //  Parte superior
                glEnable(GL_TEXTURE_2D);
                glPopMatrix();
                glPushMatrix();
                glTranslated(0, 0.9, 0);
                glScaled(1, 0.9, WALL_THICKNESS);
                DesenhaCuboComTextura(1, texParede);
                glPopMatrix();
                break;
            case PORTA:
                glEnable(GL_TEXTURE_2D);
                //  glBindTexture(GL_TEXTURE_2D, texParede);
                glPushMatrix();
                glTranslated(0, 1.05, 0);
                glScaled(1, 0.6, WALL_THICKNESS);
                DesenhaCuboComTextura(1, texParede);
                glPopMatrix();
                break;
            case CADEIRA:
                glPushMatrix();
                glTranslated(0, -1.1, 0);
                glScalef(0.003f, 0.003f, 0.003f);
                glRotated(rand() % 360, 0, 1, 0);
                glColor3f(0.8f, 0.6f, 0.4f);
                cadeira.ExibeObjeto();
                glPopMatrix();
                break;
            case MESA:
                glPushMatrix();
                glTranslated(0, -1.1, 0);
                glScalef(0.7f, 0.7f, 0.7f);
                glColor3f(0.8f, 0.6f, 0.4f); // marrom claro
                mesa.ExibeObjeto();
                glPopMatrix();
                break;
            case POS_INICIAL_PLAYER:
                posInicialX = j;
                posInicialZ = i;
                break;
            case POS_FINAL_LABIRINTO:
                posFinalX = j;
                posFinalZ = i;
                break;
            default:
                break;
            }
            glPopMatrix();
        }
    }
}

bool isPositionValid(int x, int z, float minDistance = 5.0f)
{
    // Verifica se a posição está dentro dos limites do labirinto
    if (x < 0 || x >= linhas || z < 0 || z >= colunas)
    {
        cout << "Posicao fora dos limites: x=" << x << ", z=" << z << endl;
        return false;
    }

    if (labirinto[x][z] != PISO)
    {
        // cout << "Posicao nao e piso: x=" << x << ", z=" << z << endl;
        return false;
    }

    // Verifica se a posição está longe o suficiente do personagem principal
    float dist = sqrt(pow(CantoEsquerdo.x + z - posAlvoX, 2) + pow(CantoEsquerdo.z + x - posAlvoZ, 2));
    if (dist < minDistance)
    {
        // cout << "Distância mínima não atendida: dist=" << dist << ", minDistance=" << minDistance << endl;
        return false;
    }

    return true;
}

Ponto generateRandomPosition()
{
    int x, z;
    int attempts = 0;
    int maxAttempts = 50; // Número máximo de tentativas para encontrar uma posição válida

    do
    {
        x = rand() % linhas;
        z = rand() % colunas;
        attempts++;
    } while (!isPositionValid(x, z) && attempts < maxAttempts);

    if (attempts == maxAttempts)
    {
        cout << "Erro: Nao foi possivel encontrar uma posicao valida apos " << maxAttempts << " tentativas." << endl;
        // Retorna uma posição padrão caso não encontre uma posição válida
        return Ponto(CantoEsquerdo.x, CantoEsquerdo.y, CantoEsquerdo.z);
    }

    return Ponto(CantoEsquerdo.x + z, CantoEsquerdo.y + 1, CantoEsquerdo.z + x);
}

void initPositions()
{
    int numInimigos = 10;
    int numCapsulas = 10;

    for (int i = 0; i < numInimigos; i++)
    {
        Ponto pos = generateRandomPosition();
        cout << "Posicao do inimigo gerada: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << endl;
        posicoesInimigos.push_back(pos);
    }

    for (int i = 0; i < numCapsulas; i++)
    {
        Ponto pos = generateRandomPosition();
        cout << "Posicao da cápsula gerada: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << endl;
        posicoesCapsulasEnergia.push_back(pos);
    }
}

void desenhaSeta()
{
    glBegin(GL_TRIANGLES);
    // Cabeça da seta
    glColor3f(1.0f, 0.0f, 0.0f); // Vermelho
    glVertex3f(0.0f, 0.0f, 1.5f);
    glVertex3f(0.2f, 0.0f, 0.5f);
    glVertex3f(-0.2f, 0.0f, 0.5f);
    glEnd();

    glBegin(GL_QUADS);
    // Haste da seta
    glColor3f(0.8f, 0.0f, 0.0f); // Vermelho escuro
    glVertex3f(0.05f, 0.0f, 0.5f);
    glVertex3f(-0.05f, 0.0f, 0.5f);
    glVertex3f(-0.05f, 0.0f, -0.5f);
    glVertex3f(0.05f, 0.0f, -0.5f);
    glEnd();
}

void desenhaPersonagem()
{
    glPushMatrix();
    glTranslatef(ALVO.x, ALVO.y - 0.4, ALVO.z);
    // Aplicar a rotação do personagem em torno do eixo Y
    glRotatef(anguloDoPersonagem, 0.0f, 1.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 1.0f); // Branco
    glScaled(0.6, 0.6, 0.6);
    personagem.ExibeObjeto();
    glPopMatrix();

    if (ModoDeCamera != 0)
    {
        glPushMatrix();
        glTranslatef(ALVO.x, ALVO.y - 0.3, ALVO.z - 0.1);
        glRotatef(anguloDoPersonagem + 180, 0.0f, 1.0f, 0.0f);
        desenhaSeta();
        glPopMatrix();
    }
}

void desenhaInimigo()
{
    for (const auto &pos : posicoesInimigos)
    {
        if (isPositionValid(pos.x, pos.z))
        {
            glPushMatrix();
            glTranslatef(pos.x, 0.0f, pos.z);
            glRotatef(-90, 1, 0, 0); // Ajusta a posição que vem errada no .tri
            glRotatef(anguloDoInimigo, 0.0f, 0.0f, 1.0f);
            glScaled(0.01, 0.01, 0.01);
            glColor3f(1.0f, 0.1f, 0.1f);
            inimigo.ExibeObjeto();
            glPopMatrix();
        }
    }
}

void desenhaCapsulaEnergia()
{
    for (const auto &pos : posicoesCapsulasEnergia)
    {
        if (isPositionValid(pos.x, pos.z))
        {

            glPushMatrix();
            glTranslatef(pos.x, 0.0f, pos.z);
            glScalef(0.002f, 0.002f, 0.002f);
            glRotatef(angulo, 0, 1, 0); // Para girar em torno de si mesmo
            glColor3f(1.0f, 1.0f, 0.0f);
            capsulaEnergia.ExibeObjeto();
            // glBindTexture(GL_TEXTURE_2D, texEnergy);
            glPopMatrix();
        }
    }
}

void detectaColisaoCapsula()
{
    BoundingBox personagemBox = getBoundingBox(ALVO, personagemWidth, personagemHeight, personagemDepth);
    for (auto it = posicoesCapsulasEnergia.begin(); it != posicoesCapsulasEnergia.end();)
    {
        BoundingBox capsulaBox = getBoundingBox(*it, 1, 1, 1);
        if (detectCollision(personagemBox, capsulaBox))
        {
            cout << "Personagem atingiu a capsula de energia" << endl;
            // Remover a cápsula da lista
            it = posicoesCapsulasEnergia.erase(it);
            TOTAL_ENERGIA += 200;
        }
        else
        {
            ++it; // Avançar para a próxima cápsula se não houve colisão
        }
    }
}

void moveInimigo() {
    float velocidadeInimigo = 0.040f; // Velocidade de movimento do inimigo
    float distanciaPerseguicao = 10.0f; // Distância para começar a perseguição

    for (auto it = posicoesInimigos.begin(); it != posicoesInimigos.end();) {
        // Calcular vetor direção para o personagem principal
        float dirX = ALVO.x - it->x;
        float dirZ = ALVO.z - it->z;

        // Verificar se o personagem está perto o suficiente para começar a perseguição
        float distancia = sqrt(dirX * dirX + dirZ * dirZ);
        if (distancia < distanciaPerseguicao) {
            // Normalizar o vetor direção (para manter a mesma velocidade independentemente da distância)
            if (distancia != 0) {
                dirX /= distancia;
                dirZ /= distancia;
            }

            // Atualizar posição do inimigo em direção ao personagem principal
            bool colisao;
            int tentativas = 0;
            const int maxTentativas = 100;

            do {
                colisao = false;
                it->x += dirX * velocidadeInimigo;
                it->z += dirZ * velocidadeInimigo;

                // Atualizar ângulo do inimigo
                anguloDoInimigo = atan2(dirX, dirZ) * 180.0 / PI;

                BoundingBox inimigoBox = getBoundingBox(*it, inimigoWidth, inimigoHeight, inimigoDepth);

                for (int i = 0; i < linhas; ++i) {
                    for (int j = 0; j < colunas; ++j) {
                        if ((labirinto[i][j] == PAREDE1 || labirinto[i][j] == JANELA || labirinto[i][j] == MESA)) {
                            Ponto objPos = Ponto(CantoEsquerdo.x + j, CantoEsquerdo.y + 1, CantoEsquerdo.z + i);
                            float objWidth = (labirinto[i][j] == MESA) ? 1.0 : 1.0;
                            float objHeight = (labirinto[i][j] == MESA) ? 0.5 : WALL_HEIGHT;
                            float objDepth = (labirinto[i][j] == MESA) ? 1.5 : WALL_THICKNESS;

                            BoundingBox objBox = getBoundingBox(objPos, objWidth, objHeight, objDepth);

                            if (detectCollision(inimigoBox, objBox)) {
                                colisao = true;
                                break;
                            }
                        }
                    }
                    if (colisao) {
                        break;
                    }
                }

                if (colisao) {
                    // Voltar à posição anterior
                    it->x -= dirX * velocidadeInimigo;
                    it->z -= dirZ * velocidadeInimigo;

                    // Mudar de direção aleatoriamente
                    dirX = static_cast<float>(rand() % 360 - 180);
                    dirZ = static_cast<float>(rand() % 360 - 180);

                    // Normalizar o vetor direção
                    float length = sqrt(dirX * dirX + dirZ * dirZ);
                    if (length != 0) {
                        dirX /= length;
                        dirZ /= length;
                    }

                    tentativas++;
                    if (tentativas > maxTentativas) {
                        break; // Evitar loop infinito
                    }
                }
            } while (colisao);

            BoundingBox personagemBox = getBoundingBox(ALVO, personagemWidth, personagemHeight, personagemDepth);
            BoundingBox inimigoBox = getBoundingBox(*it, inimigoWidth, inimigoHeight, inimigoDepth);

            if (detectCollision(inimigoBox, personagemBox)) {
                TOTAL_PONTOS--;
                cout << "O inimigo colidiu com o personagem! Total de pontos atualizado: " << TOTAL_PONTOS << endl;

                // Remover o inimigo da lista
                it = posicoesInimigos.erase(it);

                if (TOTAL_PONTOS == 0) {
                    // DesenharMensagemFinal(false);
                    // glutTimerFunc(3000, encerrarJogo, 0);
                }
                continue; // Evitar incrementar o iterador inválido
            }
        }
        ++it; // Incrementar o iterador apenas se não houver remoção
    }
}

boolean acabouEnergia()
{
    if (TOTAL_ENERGIA == 0)
        return true;

    return false;
}

bool detectaColisaoFinal()
{
    // Defina uma pequena margem para considerar a colisão
    float margem = 0.5f;
    return (fabs(ALVO.x - posFinalX) < margem && fabs(ALVO.z - posFinalZ) < margem);
}

// **********************************************************************
//  void DesenhaCubo()
// **********************************************************************
void DesenhaCuboComTextura(float tamAresta, GLuint textura)
{
    glBindTexture(GL_TEXTURE_2D, textura);
    glEnable(GL_TEXTURE_2D);

    glBegin(GL_QUADS);
    // Front Face
    glNormal3f(0, 0, 1);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glEnd();

    // Back Face
    glBegin(GL_QUADS);
    glNormal3f(0, 0, -1);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glEnd();

    // Top Face
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glEnd();

    // Bottom Face
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glEnd();

    // Right Face
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glEnd();

    // Left Face
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
    glEnd();

    glDisable(GL_TEXTURE_2D);
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

void DesenhaChaoV2()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texPiso);
    //  glScaled(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(0.5f, -0.5f, 0.5f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(0.5f, -0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-0.5f, -0.5f, -0.5f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
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
            if (labirinto[z][x] == POS_FINAL_LABIRINTO)
            {
                glTranslated(0, -1, 0);
                glColor3f(0.0, 1.0, 0.0); // Verde
                glPushMatrix();
                glScaled(1, 1, 1);
                // glBindTexture(GL_TEXTURE_2D, texPiso);
                glutSolidCube(1.0);
                glPopMatrix();
                glTranslated(0, 1, 1);
            }
            else
            {
                // glColor3f(0.5f, 0.5f, 0.5f);
                DesenhaChaoV2();
                glTranslated(0, 0, 1);
            }
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Defina a projeção dependendo do modo de câmera
    if (ModoDeProjecao == 0)
    {
        glOrtho(-10, 10, -10, 10, 0, 20); // Projeção paralela Orthográfica
    }
    else
    {
        MygluPerspective(60, AspectRatio, 0.1, 200); // Projeção perspectiva
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float cameraD = cameraDist1; // Distância da câmera para primeira pessoa

    if (ModoDeCamera == 0)
    {
        cameraD = cameraDist1;
        OBS.x = posAlvoX;
        OBS.z = posAlvoZ;
        OBS.y = posAlvoY;
    }
    else if (ModoDeCamera == 1)
    {
        OBS.x = (colunas / 2) - 0.5;
        OBS.z = linhas + 15;
        OBS.y = 35;

        ALVO.x = posAlvoX;
        ALVO.y = posAlvoY;
        ALVO.z = posAlvoZ;

        gluLookAt(OBS.x, OBS.y, OBS.z,             // Posição do Observador
                  colunas / 2, ALVO.y, linhas / 2, // Posição do Alvo
                  0.0, 0.0, -1.0);                 // Vetor Up

        return;
    }
    else if (ModoDeCamera == 2)
    {
        // Defina a câmera acima do mapa
        OBS.x = posAlvoX;
        OBS.z = posAlvoZ + 8;
        OBS.y = posAlvoY + 15;

        ALVO.x = posAlvoX;
        ALVO.y = posAlvoY;
        ALVO.z = posAlvoZ;

        gluLookAt(OBS.x, OBS.y, OBS.z,    // Posição do Observador
                  ALVO.x, ALVO.y, ALVO.z, // Posição do Alvo
                  0.0, 0.0, -1.0);        // Vetor Up

        return;
    }

    // Cálculo da posição e orientação da câmera
    OBS.x = posAlvoX - cameraD * sin(anguloDaCamera * M_PI / 180.0f);
    OBS.z = posAlvoZ + cameraD * cos(anguloDaCamera * M_PI / 180.0f);

    ALVO.x = posAlvoX;
    ALVO.y = posAlvoY;
    ALVO.z = posAlvoZ;

    gluLookAt(OBS.x, OBS.y, OBS.z,    // Posição do Observador
              ALVO.x, ALVO.y, ALVO.z, // Posição do Alvo
              0.0, 1.0, 0.0);         // Vetor Up
}

void encerrarJogo(int value)
{
    exit(0);
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

void renderText(float x, float y, void *font, std::string text)
{
    glRasterPos2f(x, y);
    for (char c : text)
    {
        glutBitmapCharacter(font, c);
    }
}

// Função para desenhar a mensagem final do jogo
void DesenharMensagemFinal(bool resultado)
{
    // Limpar o buffer de cores e profundidade
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    std::string mensagem;

    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT)); // Define a projeção ortogonal
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Configuração da cor e mensagem
    if (resultado)
    {
        mensagem = "Voce venceu :)";
        glColor3f(0.0f, 0.0f, 1.0f); // Cor do texto (AZUL)
    }
    else
    {
        mensagem = "Voce perdeu :(";
        glColor3f(1.0f, 0.0f, 0.0f); // Cor do texto (VERMELHO)
    }

    // Renderize o texto na posição desejada (MEIO DA TELA)
    renderText(glutGet(GLUT_WINDOW_WIDTH) / 2 - 50, glutGet(GLUT_WINDOW_HEIGHT) / 2, GLUT_BITMAP_TIMES_ROMAN_24, mensagem);

    // Restaure as matrizes de projeção e modelo
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glutSwapBuffers();
}

void displayInfo()
{
    glColor3f(1.0f, 1.0f, 1.0f); // Cor do texto (branco)

    // Salvar o estado atual da matriz de projeção
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // Definir projeção ortogonal
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT));

    // Salvar o estado atual da matriz de modelo
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    std::string infoEnergiaStr = "Energia: " + std::to_string(TOTAL_ENERGIA);
    // Converta o número de vidas para uma string
    std::string infoPontosStr = "Pontos: " + std::to_string(TOTAL_PONTOS);

    // Renderize o texto na posição escolhida
    renderText(10, glutGet(GLUT_WINDOW_HEIGHT) - 20, GLUT_BITMAP_HELVETICA_18, infoEnergiaStr);
    renderText(glutGet(GLUT_WINDOW_WIDTH) - 100, glutGet(GLUT_WINDOW_HEIGHT) - 20, GLUT_BITMAP_HELVETICA_18, infoPontosStr);

    // Restaurar o estado anterior da matriz de modelo
    glPopMatrix();

    // Restaurar o estado anterior da matriz de projeção
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    // Voltar para a matriz de modelo
    glMatrixMode(GL_MODELVIEW);
}

void display(void)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // DefineLuz();

    PosicUser();

    glMatrixMode(GL_MODELVIEW);

    DesenhaPiso();
    drawLabirinto();
    desenhaPersonagem();
    desenhaCapsulaEnergia();
    desenhaInimigo();
    moveInimigo();
    detectaColisaoCapsula();
    displayInfo();

    if (acabouEnergia())
    {
        // DesenharMensagemFinal(false);
        // glutTimerFunc(3000, encerrarJogo, 0);
        encerrarJogo(1);
    }
    if (detectaColisaoFinal())
    {
        // DesenharMensagemFinal(true);
        // glutTimerFunc(3000, encerrarJogo, 0);
        encerrarJogo(1);
    }

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
    case 27:     // Termina o programa quando
        exit(0); // a tecla ESC for pressionada
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
    case 32: // Andar/parar com a tecla espaço
        emMovimento = !emMovimento;
        break;
    case 'a': // Virar para esquerda
        anguloDaCamera -= angleStep;
        anguloDoPersonagem += angleStep;
        break;
    case 'd': // Virar para direita
        anguloDaCamera += angleStep;
        anguloDoPersonagem -= angleStep;
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
    // system("pwd");

    readMap("MatrizMapa.txt");
    init();
    inicializarTexturas();

    initPositions();

    // Carrega objetos 3D
    capsulaEnergia.LeObjeto("energy.tri");
    cadeira.LeObjeto("Chair.tri");
    mesa.LeObjeto("table.tri");
    inimigo.LeObjeto("enemy.tri");
    personagem.LeObjeto("personagemPrincipal.tri");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(arrow_keys);
    glutIdleFunc(animate);

    glutMainLoop();
    return 0;
}

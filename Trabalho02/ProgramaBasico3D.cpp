// **********************************************************************
// PUCRS/Escola Politecnica
// COMPUTACAO GRAFICA
//
// Programa basico para criar aplicacoes 3D em OpenGL
//
// Professor
// Marcio Sarroglia Pinho
// pinho@pucrs.br
//
// Alunos
// Felipe Freitas
// f.freitas007@edu.pucrs.br
//
// Lucas Wolschick
// lucas.wolschick@edu.pucrs.br
// **********************************************************************

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

#include "SOIL/SOIL.h"
#include "ImageClass.h"
#include "Temporizador.h"
#include "ListaDeCoresRGB.h"
#include "Ponto.h"
Temporizador T;
double AccumDeltaT=0;


GLfloat AspectRatio;
GLuint texturaPiso, texturaParedao, texturaVeiculo, texturaCanhao;
bool showTexture = true;


// Controle do modo de projecao
// 0: Wireframe; 1: Faces preenchidas
// A funcao "Init" utiliza esta variavel. O valor dela e alterado pela tecla 'e'
int ModoDeExibicao = 1;

double nFrames = 0;
double TempoTotal = 0;

// Prototipos
void DesenhaChao();
void DesenhaParedao();
void DesenhaLadrilho();
void DesenhaParalelepipedo(float largura, float altura, float profundidade);
void DesenhaVeiculo();
void moveVeiculo(unsigned char key);
void rotacionaVeiculo(unsigned char key);
void rotacionaCanhao(unsigned char key);
void atiraProjetil();
Ponto CalculaBezier3(Ponto PC[], double t);
void DesenhaProjetil();
GLuint LoadTexture(const char *nomeTextura);
void init(void);
void display(void);
void PosicUser();
void keyboard(unsigned char key, int x, int y);
void arrow_keys(int a_keys, int x, int y);
void animate();
void MygluPerspective(float fieldOfView, float aspect, float zNear, float zFar);
void DefineLuz(void);
void reshape(int w, int h);

// Constants
const int wallHeight = 15;
const int sceneWidth = 25;
const int sceneDepth = 50;

Ponto PosicaoParedao;
bool wallGrid[sceneWidth][wallHeight];

Ponto PosicaoRelativaCamera;
Ponto TamanhoVeiculo;
float distanciaMovimentoVeiculo;
Ponto TamanhoCanhao;
Ponto PosicaoRelativaCanhao;

// Variables
Ponto PosicaoVeiculo;
Ponto AnguloVeiculo;
Ponto DirecaoVeiculo;
Ponto AnguloCanhao;
Ponto DirecaoCanhao;
float forcaCanhao;

Ponto PontosBezier[3];
bool isShooting;
float deslocamentoProjetil;

bool sideView;

void DesenhaChao() {
    glPushMatrix();
        if (showTexture) {
            glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texturaPiso);
        } else {
            defineCor(DarkGreen);
        }
        glTranslated(0, -1, 0);
        for (int x = 0; x < sceneWidth; x++) {
            glPushMatrix();
            for (int z = 0; z < sceneDepth; z++) {
                DesenhaLadrilho();
                glTranslated(0, 0, 1);
            }
            glPopMatrix();
            glTranslated(1, 0, 0);
        }
        if (showTexture)
            glPopMatrix();
    glPopMatrix();
}

void DesenhaParedao() {
    glPushMatrix();
        if (showTexture) {
            glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texturaParedao);
        } else {
            defineCor(DarkBrown);
        }
        glTranslated(PosicaoParedao.x, PosicaoParedao.y, PosicaoParedao.z);
        glRotatef(-90, 1, 0, 0);
        glPushMatrix();
            for (int x = 0; x < sceneWidth; x++) {
                glPushMatrix();
                    for (int z = 0; z < wallHeight; z++) {
                        if (wallGrid[x][z])
                            DesenhaLadrilho();
                        glTranslated(0, 0, 1);
                    }
                glPopMatrix();
                glTranslated(1, 0, 0);
            }
        glPopMatrix();
        if (showTexture)
            glPopMatrix();
    glPopMatrix();
    glPushMatrix();
        if (showTexture) {
            glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texturaPiso);
        } else {
            defineCor(DarkBrown);
        }
        glTranslated(PosicaoParedao.x, PosicaoParedao.y, PosicaoParedao.z);
        glPushMatrix();
            for (int x = 0; x < sceneWidth; x++) {
                DesenhaLadrilho();
                glTranslated(1, 0, 0);
            }
        glPopMatrix();
        if (showTexture)
            glPopMatrix();
    glPopMatrix();
}

void DesenhaLadrilho() {
    // Desenha quadrado preenchido
    glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glVertex3f(-0.5f, 0.0f, -0.5f);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(-0.5f, 0.0f, 0.5f);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(0.5f, 0.0f, 0.5f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0.5f, 0.0f, -0.5f);
    glEnd();

    defineCor(MediumGoldenrod);
    glBegin(GL_LINE_STRIP);
        glNormal3f(0,1,0);
        glVertex3f(-0.5f,  0.0f, -0.5f);
        glVertex3f(-0.5f,  0.0f,  0.5f);
        glVertex3f( 0.5f,  0.0f,  0.5f);
        glVertex3f( 0.5f,  0.0f, -0.5f);
    glEnd();
}

void DesenhaParalelepipedo(float largura, float altura, float profundidade) {
    glPushMatrix();
        glTranslatef(0, 0, 0);
        glScalef(largura, altura, profundidade);
        // glutSolidCube(1);
        float tamAresta = 1.0f;
        glBegin(GL_QUADS);
            // Front Face
            glNormal3f(0, 0, 1);
            glTexCoord2f(0.5f, 0.0f);
            glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
            glTexCoord2f(1.0f, 0.5f);
            glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
            glTexCoord2f(0.5f, 0.5);
            glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);

            // Back Face
            glNormal3f(0, 0, -1);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);

            // Top Face
            glNormal3f(0, 1, 0);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(-tamAresta / 2, tamAresta / 2, -tamAresta / 2);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(-tamAresta / 2, tamAresta / 2, tamAresta / 2);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);

            // Bottom Face
            glNormal3f(0, -1, 0);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(-tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(-tamAresta / 2, -tamAresta / 2, tamAresta / 2);

            // Right face
            glNormal3f(1, 0, 0);
            glTexCoord2f(1.0f, 0.0f);
            glVertex3f(tamAresta / 2, -tamAresta / 2, -tamAresta / 2);
            glTexCoord2f(1.0f, 1.0f);
            glVertex3f(tamAresta / 2, tamAresta / 2, -tamAresta / 2);
            glTexCoord2f(0.0f, 1.0f);
            glVertex3f(tamAresta / 2, tamAresta / 2, tamAresta / 2);
            glTexCoord2f(0.0f, 0.0f);
            glVertex3f(tamAresta / 2, -tamAresta / 2, tamAresta / 2);

            // Left Face
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
    glPopMatrix();
}

void DesenhaVeiculo() {
    glPushMatrix();
        // Base 
        glTranslatef(PosicaoVeiculo.x, PosicaoVeiculo.y, PosicaoVeiculo.z);
        glRotatef(AnguloVeiculo.y, 0, 1, 0);
        if (showTexture) {
            glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texturaVeiculo);
        } else {
            glColor3f(0.4f, 1.0f, 0.2f);
        }
        DesenhaParalelepipedo(TamanhoVeiculo.x, TamanhoVeiculo.y, TamanhoVeiculo.z);
        if (showTexture)
            glPopMatrix();
        
        // Canhão
        glTranslatef(PosicaoRelativaCanhao.x, PosicaoRelativaCanhao.y, PosicaoRelativaCanhao.z);
        glRotatef(AnguloCanhao.x, 1, 0, 0);
        if (showTexture) {
            glPushMatrix();
                glBindTexture(GL_TEXTURE_2D, texturaCanhao);
        } else {
            glColor3f(1.0f, 0.2f, 0.1f);
        }
        DesenhaParalelepipedo(TamanhoCanhao.x, TamanhoCanhao.y, TamanhoCanhao.z);
        if (showTexture)
            glPopMatrix();
    glPopMatrix();
}

void moveVeiculo(unsigned char key) {
    // Normaliza o vetor para ter comprimento 1, mantendo apenas a direção
    float modDirecao = DirecaoVeiculo.modulo();
    Ponto Direcao = Ponto(
        DirecaoVeiculo.x / modDirecao,
        DirecaoVeiculo.y / modDirecao,
        DirecaoVeiculo.z / modDirecao
    );
    Ponto DistanciaPercorrida = Direcao * distanciaMovimentoVeiculo;
    if (key != 'W' && key != 'w') {
        cout << "Tecla " << key << " invalida para movimentacao do veículo" << endl;
        return;
    }
    Ponto NovaPosicao = PosicaoVeiculo + DistanciaPercorrida;
    if (NovaPosicao.x < 0 || NovaPosicao.x > sceneWidth) {
        cout << "Movimento invalido, veiculo nao pode sair da pista" << endl;
        return;
    }
    if (NovaPosicao.z < 0 || NovaPosicao.z > sceneDepth) {
        cout << "Movimento invalido, veiculo nao pode sair da pista" << endl;
        return;
    }
    PosicaoVeiculo = NovaPosicao;
}

void rotacionaVeiculo(unsigned char key) {
    if (key != 'a' && key != 'A') {
        cout << "Tecla " << key << " invalida para rotacao do veiculo" << endl;
        return;
    }
    if (key == 'a') {
        AnguloVeiculo.y -= 1.0f;
    } else if (key == 'A') {
        AnguloVeiculo.y += 1.0f;
    }
    DirecaoVeiculo.rotacionaY(AnguloVeiculo.y);
}

void rotacionaCanhao(unsigned char key) {
    if (key != 'b' && key != 'B') {
        cout << "Tecla " << key << " invalida para movimentacao da  do canhao" << endl;
        return;
    }
    // if (AnguloVeiculo.y != 0) {
    //     cout << "Veiculo deve estar alinhado com o eixo Z para rotacionar o canhao" << endl;
    //     return;
    // }
    if (key == 'b') {
        if (AnguloCanhao.x > -90)
            AnguloCanhao.x -= 2.0f;
    } else if (key == 'B') {
        if (AnguloCanhao.x < 45)
            AnguloCanhao.x += 2.0f;
    }
}

void atiraProjetil() {
    DirecaoCanhao = Ponto(0, 0, 1);
    DirecaoCanhao.rotacionaX(AnguloCanhao.x);
    DirecaoCanhao.rotacionaY(AnguloVeiculo.y);
    // No material de apoio, "B"
    Ponto PosicaoCanhao = PosicaoVeiculo + PosicaoRelativaCanhao;
    Ponto DirecaoProjetil = PosicaoRelativaCanhao + DirecaoCanhao * forcaCanhao;
    float Distancia = 2 * forcaCanhao * cos(AnguloCanhao.x * M_PI / 180);
    // No material de apoio, "C"
    Ponto Alvo = PosicaoCanhao + Ponto(0, PosicaoCanhao.y - 5, Distancia);
    Alvo.rotacionaY(AnguloVeiculo.y);
 
    PontosBezier[0] = PosicaoCanhao;
    PontosBezier[1] = PosicaoCanhao + DirecaoProjetil * 0.5;
    PontosBezier[2] = Alvo;

    deslocamentoProjetil = 0.0f;
}

Ponto CalculaBezier3(Ponto PC[], double t) {
    double UmMenosT = 1 - t;
    return PC[0] * UmMenosT * UmMenosT + PC[1] * 2 * UmMenosT * t + PC[2] * t * t;
}

void DesenhaProjetil() {
    double DeltaT = 1.0 / 50;
    while (deslocamentoProjetil < 1.0) {
        Ponto P = CalculaBezier3(PontosBezier, deslocamentoProjetil);

        // TODO: Verificar colisões (parede, inimigos, chão)
        // Se não colidiu em nada, continua o movimento

        cout << "Deslocamento: " << deslocamentoProjetil << endl;
        P.imprime("Sou um ponto", "\n");
        glPushMatrix();
            glColor3f(0.0f, 0.0f, 0.0f);
            glTranslated(P.x, P.y, P.z);
            glutSolidSphere(TamanhoCanhao.x * 0.8, 20, 20);
        glPopMatrix();
        deslocamentoProjetil += DeltaT;
    }
    // Desenha pontos de controle
    float colors[3] = {OrangeRed, GreenYellow, SkyBlue};
    for (int i = 0; i < 3; i++) {
        glPushMatrix();
            defineCor(colors[i]);
            glTranslated(PontosBezier[i].x, PontosBezier[i].y, PontosBezier[i].z);
            glutSolidSphere(TamanhoCanhao.x * 0.8, 20, 20);
        glPopMatrix();
    }
}

GLuint LoadTexture(const char *nomeTextura) {
    GLenum errorCode;
    GLuint IdTEX;
    // Habilita o uso de textura
    glEnable(GL_TEXTURE_2D);

    // Define a forma de armazenamento dos pixels na textura (1= alihamento por byte)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Gera um identificar para a textura
    glGenTextures(1, &IdTEX); //  vetor que guardas os numeros das texturas
    errorCode = glGetError();
    if (errorCode == GL_INVALID_OPERATION) {
        cout << "Erro: glGenTextures chamada entre glBegin/glEnd." << endl;
        return -1;
    }

    // Define que tipo de textura sera usada
    // GL_TEXTURE_2D ==> define que sera usada uma textura 2D (bitmaps)
    // texture_id[OBJETO_ESQUERDA]  ==> define o numero da textura
    glBindTexture(GL_TEXTURE_2D, IdTEX);

    // Carrega a imagem
    ImageClass Img;

    int r = Img.Load(nomeTextura);
    if (!r) {
        cout << "Erro lendo imagem " << nomeTextura << endl;
        exit(1);
    }

    int level = 0;
    int border = 0;

    // Envia a textura para OpenGL, usando o formato apropriado
    int formato;
    formato = GL_RGB;
    if (Img.Channels() == 4)
        formato = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, level, formato,
                 Img.SizeX(), Img.SizeY(),
                 border, formato,
                 GL_UNSIGNED_BYTE, Img.GetImagePtr());
    
    errorCode = glGetError();
    if (errorCode == GL_INVALID_OPERATION) {
        cout << "Erro: glTexImage2D chamada entre glBegin/glEnd." << endl;
        return -1;
    }

    if (errorCode != GL_NO_ERROR) {
        cout << "Houve algum erro na criacao da textura." << endl;
        return -1;
    }

    // Ajusta os filtros iniciais para a textura
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Img.Delete();

    cout << "Carga de textura OK." << endl;
    return IdTEX;
}

void init(void) {
    // Fundo de tela azul claro
    glClearColor(0.6156862745f, 0.8980392157f, 0.9803921569f, 1.0f);

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    // glShadeModel(GL_FLAT);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    if (ModoDeExibicao) {
        texturaPiso = LoadTexture("assets/chaograma.jpg");
        texturaParedao = LoadTexture("assets/paredePedra.jpg");
        texturaVeiculo = LoadTexture("assets/camuflado.jpg");
        texturaCanhao = LoadTexture("assets/metal.jpg");
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    sideView = true;

    // Reset wall grid
    PosicaoParedao = Ponto(0, -1, sceneDepth / 2);
    for (int i = 0; i < sceneWidth; i++)
        for (int j = 0; j < wallHeight; j++)
            wallGrid[i][j] = true;

    PosicaoRelativaCamera = Ponto(0, 2, -5);
    TamanhoVeiculo = Ponto(2, 1, 3);
    distanciaMovimentoVeiculo = 1.0f;
    // TamanhoCanhao = Ponto((1/4.0) * TamanhoVeiculo.x,
    //                       (1/5.0) * TamanhoVeiculo.y,
    //                       (2/3.0) * TamanhoVeiculo.z);
    TamanhoCanhao = Ponto(0.5, 0.5, 2.0);
    PosicaoVeiculo = Ponto(6, 0, 4);
    AnguloVeiculo = Ponto(0, 0, 0);
    DirecaoVeiculo = Ponto(0, 0, 1);
    PosicaoRelativaCanhao = Ponto(0, 0.5, 1);
    AnguloCanhao = Ponto(0, 0, 0);
    DirecaoCanhao = Ponto(0, 0, 1);
    forcaCanhao = 5.0f;
    isShooting = false;
    deslocamentoProjetil = 1.0f;
}

void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DefineLuz();

	PosicUser();

	glMatrixMode(GL_MODELVIEW);

    DesenhaChao();

    DesenhaParedao();

    DesenhaVeiculo();

    DesenhaProjetil();

	glutSwapBuffers();
}

void PosicUser() {
    // Define os parametros da projecao Perspectiva
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Projecao perspectiva
    MygluPerspective(90, AspectRatio, 0.01, 50);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if (sideView) {
        float xObservador = -sceneWidth / 2.0;
        float yObservador = wallHeight * 2/5.0;
        float zObservador = sceneDepth / 2.0;
        float xTarget = sceneWidth;
        float yTarget = 0;
        float zTarget = sceneDepth / 2.0;
        gluLookAt(
            // Posicao da camera
            xObservador, yObservador, zObservador,
            // Posicao do alvo
            xTarget, yTarget, zTarget,
            // Vetor ViewUp
            0.0f, 1.0f, 0.0f
        );
    } else {
        float xObservador = PosicaoVeiculo.x + PosicaoRelativaCamera.x;
        float yObservador = PosicaoVeiculo.y + PosicaoRelativaCamera.y;
        float zObservador = PosicaoVeiculo.z + PosicaoRelativaCamera.z;
        gluLookAt(
            // Posicao da camera
            xObservador, yObservador, zObservador,
            // Posicao do alvo
            PosicaoVeiculo.x, PosicaoVeiculo.y, PosicaoVeiculo.z,
            // Vetor ViewUp
            0.0f, 1.0f, 0.0f
        );
    }
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
        // Termina o programa qdo a tecla ESC for pressionada
        case 27:
            exit(0);
            break;
        case 'e':
            ModoDeExibicao = !ModoDeExibicao;
            init();
            glutPostRedisplay();
            break;
        case 'p':
            sideView = !sideView;
            break;
        case 't':
            showTexture = !showTexture;
            break;
        case 'r':
            init();
            break;
        case 'w':
        case 'W':
            moveVeiculo(key);
            break;
        case 'a':
        case 'A':
            rotacionaVeiculo(key);
            break;
        case 'b':
        case 'B':
            rotacionaCanhao(key);
            break;
        case 'c':
            forcaCanhao += 0.5f;
            break;
        case 'C':
            forcaCanhao -= 0.5f;
            break;
        case ' ':
            if (isShooting)
                cout << "Ja existe um projetil em movimento" << endl;
            else {
                isShooting = true;
                atiraProjetil();
                isShooting = false;
            }
            break;
        default:
            cout << "Tecla " << key << " nao tem funcao definida" << endl;
            break;
    }
    glutPostRedisplay();
}

void arrow_keys(int a_keys, int x, int y) {
	switch (a_keys) {
        // When Up Arrow Is Pressed...
		case GLUT_KEY_UP:
			glutFullScreen();
			break;
        // When Down Arrow Is Pressed...
	    case GLUT_KEY_DOWN:
			glutInitWindowSize(1280, 720);
			break;
		default:
			break;
	}
}

void animate() {
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    // fixa a atualizacao da tela em 30
    if (AccumDeltaT > 1.0/30) {
        AccumDeltaT = 0;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0) {
        /*
            cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
            cout << "Nros de Frames sem desenho: " << nFrames << endl;
            cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        */
        TempoTotal = 0;
        nFrames = 0;
    }
}

void MygluPerspective(float fieldOfView, float aspect, float zNear, float zFar) {
    // https://stackoverflow.com/questions/2417697/gluperspective-was-removed-in-opengl-3-1-any-replacements/2417756#2417756
    // The following code is a fancy bit of math that is equivilant to calling:
    // gluPerspective(fieldOfView/2.0f, width/height, 0.1f, 255.0f)
    // We do it this way simply to avoid requiring glu.h
    // GLfloat zNear = 0.1f;
    // GLfloat zFar = 255.0f;
    // GLfloat aspect = float(width)/float(height);
    GLfloat fH = tan(float(fieldOfView / 360.0f * 3.14159f)) * zNear;
    GLfloat fW = fH * aspect;
    glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void DefineLuz(void) {
    // Define cores para um objeto dourado
    GLfloat LuzAmbiente[] = {0.4, 0.4, 0.4};
    GLfloat LuzDifusa[] = {0.7, 0.7, 0.7};
    GLfloat LuzEspecular[] = {0.9f, 0.9f, 0.9};
    GLfloat PosicaoLuz0[] = {0.0f, 3.0f, 5.0f};
    GLfloat Especularidade[] = {1.0f, 1.0f, 1.0f};

    // ****************  Fonte de Luz 0
    glEnable(GL_COLOR_MATERIAL);

    // Habilita o uso de iluminacao
    glEnable(GL_LIGHTING);

    // Ativa o uso da luz ambiente
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LuzAmbiente);
    // Define os parametros da luz numero Zero
    glLightfv(GL_LIGHT0, GL_AMBIENT, LuzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, LuzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, LuzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, PosicaoLuz0);
    glEnable(GL_LIGHT0);

    // Ativa o "Color Tracking"
    glEnable(GL_COLOR_MATERIAL);

    // Define a reflectancia do material
    glMaterialfv(GL_FRONT,GL_SPECULAR, Especularidade);

    // Define a concentracao do brilho.
    // Quanto maior o valor do Segundo parametro, mais
    // concentrado sera o brilho. (Valores validos: de 0 a 128)
    glMateriali(GL_FRONT, GL_SHININESS, 51);
}

void reshape(int w, int h) {
	// Evita divisao por zero, no caso de uam janela com largura 0.
	if (h == 0) h = 1;
    // Ajusta a relacao entre largura e altura para evitar distorcao na imagem.
    // Veja funcao "PosicUser".
	AspectRatio = 1.0f * w / h;
	// Reset the coordinate system before modifying
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// Seta a viewport para ocupar toda a janela
    glViewport(0, 0, w, h);
    //cout << "Largura" << w << endl;

	PosicUser();
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutInitWindowPosition(0,0);
	glutInitWindowSize(1280, 720);
	glutCreateWindow("Trabalho 2 - Felipe Freitas e Lucas Wolschick - Computacao Grafica - 2023/2 - Prof. Marcio Pinho");

	init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(arrow_keys);
	glutIdleFunc(animate);

	glutMainLoop();
	return 0;
}

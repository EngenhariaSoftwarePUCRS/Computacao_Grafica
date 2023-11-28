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

#include "Temporizador.h"
#include "ListaDeCoresRGB.h"
#include "Ponto.h"
Temporizador T;
double AccumDeltaT=0;


GLfloat AspectRatio;

// Controle do modo de projecao
// 0: Projecao Paralela Ortografica; 1: Projecao Perspectiva
// A funcao "PosicUser" utiliza esta variavel. O valor dela e alterado pela tecla 'p'
int ModoDeProjecao = 1;


// Controle do modo de projecao
// 0: Wireframe; 1: Faces preenchidas
// A funcao "Init" utiliza esta variavel. O valor dela e alterado pela tecla 'e'
int ModoDeExibicao = 1;

double nFrames = 0;
double TempoTotal = 0;

// Constants
int wallHeight;
int sceneWidth;
int sceneDepth;

Ponto TamanhoVeiculo;
float distanciaMovimentoVeiculo;
Ponto TamanhoCanhao;
Ponto PosicaoCanhao;

// Variables
Ponto PosicaoVeiculo;
Ponto AnguloVeiculo;
Ponto DirecaoVeiculo;
Ponto AnguloCanhao;
Ponto DirecaoCanhao;
float forcaCanhao;

void init(void) {
    glClearColor(0.6156862745f, 0.8980392157f, 0.9803921569f, 1.0f);
    // Fundo de tela azul claro

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    // glShadeModel(GL_FLAT);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    // Faces Preenchidas??
    if (ModoDeExibicao)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    sceneWidth = 25;
    wallHeight = 15;
    sceneDepth = 50;

    TamanhoVeiculo = Ponto(2, 1, 3);
    distanciaMovimentoVeiculo = 1.0f;
    // TamanhoCanhao = Ponto((1/4.0) * TamanhoVeiculo.x,
    //                       (1/5.0) * TamanhoVeiculo.y,
    //                       (2/3.0) * TamanhoVeiculo.z);
    TamanhoCanhao = Ponto(0.5, 0.5, 2.0);
    PosicaoVeiculo = Ponto(6, 0, 4);
    AnguloVeiculo = Ponto(0, 0, 0);
    DirecaoVeiculo = Ponto(0, 0, 1);
    AnguloCanhao = Ponto(0, 0, 0);
    DirecaoCanhao = Ponto(0, 0, 1);
    forcaCanhao = 1.0f;
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
            TempoTotal = 0;
            nFrames = 0;
        */
    }
}

void DesenhaCubo(float tamAresta) {
    glBegin(GL_QUADS);
    // Front Face
    glNormal3f(0, 0, 1);
    glVertex3f(-tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2,  tamAresta/2);
    // Back Face
    glNormal3f(0, 0, -1);
    glVertex3f(-tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2, -tamAresta/2);
    // Top Face
    glNormal3f(0, 1, 0);
    glVertex3f(-tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2, -tamAresta/2);
    // Bottom Face
    glNormal3f(0, -1, 0);
    glVertex3f(-tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2, -tamAresta/2,  tamAresta/2);
    // Right face
    glNormal3f(1, 0, 0);
    glVertex3f( tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2,  tamAresta/2);
    // Left Face
    glNormal3f(-1, 0, 0);
    glVertex3f(-tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f(-tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2, -tamAresta/2);
    glEnd();
}

void DesenhaParalelepipedo(float largura, float altura, float profundidade) {
    glPushMatrix();
        glTranslatef(0, 0, 0);
        glScalef(largura, altura, profundidade);
        DesenhaCubo(1);
    glPopMatrix();
}

void DesenhaVeiculo() {
    glPushMatrix();
        // Base 
        glTranslatef(PosicaoVeiculo.x, PosicaoVeiculo.y, PosicaoVeiculo.z);
        glRotatef(AnguloVeiculo.y, 0, 1, 0);
        glColor3f(0.4f, 1.0f, 0.2f);
        DesenhaParalelepipedo(TamanhoVeiculo.x, TamanhoVeiculo.y, TamanhoVeiculo.z);
        
        // Canhão
        glTranslatef(0, 0.5, 1);
        glRotatef(AnguloCanhao.x, 1, 0, 0);
        glColor3f(1.0f, 0.2f, 0.1f);
        DesenhaParalelepipedo(TamanhoCanhao.x, TamanhoCanhao.y, TamanhoCanhao.z);
    glPopMatrix();
}

// **********************************************************************
// void DesenhaLadrilho(int corBorda, int corDentro)
// Desenha uma celula do piso.
// E possivel definir a cor da borda e do interior do piso
// O ladrilho tem largula 1, centro no (0, 0, 0) e esta sobre o plano XZ
// **********************************************************************
void DesenhaLadrilho(int corBorda, int corDentro) {
    // Desenha quadrado preenchido
    defineCor(corDentro);
    glBegin(GL_QUADS);
        glNormal3f(0,1,0);
        glVertex3f(-0.5f,  0.0f, -0.5f);
        glVertex3f(-0.5f,  0.0f,  0.5f);
        glVertex3f( 0.5f,  0.0f,  0.5f);
        glVertex3f( 0.5f,  0.0f, -0.5f);
    glEnd();

    defineCor(corBorda);
    glBegin(GL_LINE_STRIP);
        glNormal3f(0,1,0);
        glVertex3f(-0.5f,  0.0f, -0.5f);
        glVertex3f(-0.5f,  0.0f,  0.5f);
        glVertex3f( 0.5f,  0.0f,  0.5f);
        glVertex3f( 0.5f,  0.0f, -0.5f);
    glEnd();
}

void DesenhaMalhaLadrilhos(int width, int depth, int innerColor) {
    glPushMatrix();
    for(int x = 0; x < width; x++) {
        glPushMatrix();
        for(int z = 0; z < depth; z++) {
            DesenhaLadrilho(MediumGoldenrod, innerColor);
            glTranslated(0, 0, 1);
        }
        glPopMatrix();
        glTranslated(1, 0, 0);
    }
    glPopMatrix();
}

void DesenhaParedao() {
    glPushMatrix();
        glTranslated(0, 0, sceneDepth / 2);
        glRotatef(-90, 1, 0, 0);
        DesenhaMalhaLadrilhos(sceneWidth, wallHeight, DarkBrown);
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, -0.5, sceneDepth / 2);
        DesenhaMalhaLadrilhos(sceneWidth, 1, DarkBrown);
    glPopMatrix();
}

void DesenhaChao() {
    glPushMatrix();
        glTranslated(0, -0.5, 0);
        DesenhaMalhaLadrilhos(sceneWidth, sceneDepth / 2, BlueViolet);
    glPopMatrix();
    glPushMatrix();
        int behindWallDepth = sceneDepth / 2 + 1;
        glTranslated(0, -0.5, behindWallDepth);
        DesenhaMalhaLadrilhos(sceneWidth, sceneDepth / 2, IndianRed);
    glPopMatrix();
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

void PosicUser() {
    // Define os parametros da projecao Perspectiva
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define o volume de visualizacao sempre a partir da posicao do observador
    if (ModoDeProjecao == 0)
        // Projecao paralela Orthografica
        glOrtho(-10, 10, -10, 10, 0, 7);
    // Projecao perspectiva
    else MygluPerspective(90,AspectRatio,0.01,50);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        PosicaoVeiculo.x - 1, PosicaoVeiculo.y + 1, PosicaoVeiculo.z - 5,
        PosicaoVeiculo.x, PosicaoVeiculo.y, PosicaoVeiculo.z,
        // Vetor ViewUp
        0.0f, 1.0f, 0.0f
    );
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

float PosicaoZ = -30;
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DefineLuz();

	PosicUser();

	glMatrixMode(GL_MODELVIEW);

    DesenhaChao();

    DesenhaParedao();

    DesenhaVeiculo();

	glutSwapBuffers();
}

void moveVeiculo(unsigned char key) {
    // Normaliza o vetor para ter comprimento 1, mantendo apenas a direção
    DirecaoVeiculo.versor();
    Ponto DistanciaPercorrida = DirecaoVeiculo * distanciaMovimentoVeiculo;
    if (key != 'W' && key != 'w' && key != 'S' && key != 's') {
        cout << "Tecla " << key << " invalida para movimentacao do veículo" << endl;
        return;
    }
    if (key == 'S' || key == 's') {
        DistanciaPercorrida = -DistanciaPercorrida;
    }
    Ponto NovaPosicao = PosicaoVeiculo + DistanciaPercorrida;
    NovaPosicao.imprime("Eae", "\n");
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

void corrigeDirecaoCanhao() {
    Ponto DirecaoDoCanhao = Ponto(1, 0, 0);
    DirecaoDoCanhao.rotacionaZ(AnguloCanhao.z);
    DirecaoDoCanhao.rotacionaY(AnguloVeiculo.y);
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
    corrigeDirecaoCanhao();
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
    corrigeDirecaoCanhao();
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
        // Termina o programa qdo a tecla ESC for pressionada
        case 27:
            exit(0);
            break;
        case 'p':
            ModoDeProjecao = !ModoDeProjecao;
            break;
        case 'e':
            ModoDeExibicao = !ModoDeExibicao;
            break;
        case 'r':
            init();
            break;
        case 'w':
        case 'W':
        case 's':
        case 'S':
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
            cout << "Tiro" << endl;
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

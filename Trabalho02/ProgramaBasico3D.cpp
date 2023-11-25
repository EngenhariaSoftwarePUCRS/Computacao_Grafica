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


GLfloat AspectRatio, angulo=0;

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

int wallHeight;
int sceneWidth;
int floorDepth;
Ponto PosicaoObservador;
Ponto PosicaoAlvo;
Ponto VetorObservadorAlvo;

Ponto PosicaoBaseCanhao;
Ponto PosicaoMiraCanhao;
float velocidadeMovimento;

void init(void) {
    glClearColor(0.6156862745f, 0.8980392157f, 0.9803921569f, 1.0f);
    // Fundo de tela azul claro

    glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);
    //glShadeModel(GL_FLAT);

    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    // Faces Preenchidas??
    if (ModoDeExibicao)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    sceneWidth = 25;
    wallHeight = 15;
    floorDepth = 25;
    PosicaoObservador = Ponto(6, 0, 3);
    PosicaoAlvo = Ponto(6, 0, 4);
    VetorObservadorAlvo = PosicaoAlvo - PosicaoObservador;
    velocidadeMovimento = 0.5f;
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
        angulo+= 1;
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
    glNormal3f(0,0,1);
    glVertex3f(-tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2,  tamAresta/2);
    // Back Face
    glNormal3f(0,0,-1);
    glVertex3f(-tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2, -tamAresta/2);
    // Top Face
    glNormal3f(0,1,0);
    glVertex3f(-tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2, -tamAresta/2);
    // Bottom Face
    glNormal3f(0,-1,0);
    glVertex3f(-tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2, -tamAresta/2,  tamAresta/2);
    // Right face
    glNormal3f(1,0,0);
    glVertex3f( tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2, -tamAresta/2);
    glVertex3f( tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f( tamAresta/2, -tamAresta/2,  tamAresta/2);
    // Left Face
    glNormal3f(-1,0,0);
    glVertex3f(-tamAresta/2, -tamAresta/2, -tamAresta/2);
    glVertex3f(-tamAresta/2, -tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2,  tamAresta/2);
    glVertex3f(-tamAresta/2,  tamAresta/2, -tamAresta/2);
    glEnd();

}

void DesenhaParalelepipedo() {
    glPushMatrix();
        glTranslatef(0,0,-1);
        glScalef(1,1,2);
        glutSolidCube(2);
        //DesenhaCubo(1);
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
        glTranslated(0, -1, floorDepth);
        glRotatef(90, -90, 0, 1);
        DesenhaMalhaLadrilhos(sceneWidth, wallHeight, DarkBrown);
    glPopMatrix();
    glPushMatrix();
        glTranslated(0, -1, floorDepth);
        DesenhaMalhaLadrilhos(sceneWidth, 1, DarkBrown);
    glPopMatrix();
}

void DesenhaChao() {
    glPushMatrix();
        glTranslated(0, -1, 0);
        DesenhaMalhaLadrilhos(sceneWidth, floorDepth, BlueViolet);
    glPopMatrix();
    glPushMatrix();
        int behindWallDepth = floorDepth + 1;
        glTranslated(0, -1, behindWallDepth);
        DesenhaMalhaLadrilhos(sceneWidth, floorDepth, IndianRed);
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
        PosicaoObservador.x, PosicaoObservador.y, PosicaoObservador.z,
        PosicaoAlvo.x, PosicaoAlvo.y, PosicaoAlvo.z,
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

	glPushMatrix();
		glTranslatef(5.0f, 0.0f, 3.0f);
        glRotatef(angulo,0,1,0);
        // Vermelho
		glColor3f(0.5f,0.0f,0.0f);
        glutSolidCube(2);
        //DesenhaCubo(1);
	glPopMatrix();

	glPushMatrix();
		glTranslatef(10.0f, 0.0f, 12.0f);
		glRotatef(angulo, 0, 1, 0);
        // Azul claro
		glColor3f(0.6156862745, 0.8980392157, 0.9803921569);
        glutSolidCube(2);
		//DesenhaCubo(1);
	glPopMatrix();

    DesenhaChao();

    DesenhaParedao();

	glutSwapBuffers();
}

void moveObservador(unsigned char key) {
    // Normaliza o vetor para ter comprimento 1, mantendo apenas a direção
    VetorObservadorAlvo.versor();
    Ponto DistanciaPercorrida = VetorObservadorAlvo * velocidadeMovimento;
    if (key != 'w' && key != 's') {
        cout << "Tecla " << key << " invalida para movimentacao do observador" << endl;
        return;
    }
    if (key == 's') {
        DistanciaPercorrida = -DistanciaPercorrida;
    }
    PosicaoObservador = PosicaoObservador + DistanciaPercorrida;
}

void keyboard(unsigned char key, int x, int y) {
	switch(key) {
        // Termina o programa qdo a tecla ESC for pressionada
        case 27:
            exit(0);
            break;
        case 'p':
            ModoDeProjecao = !ModoDeProjecao;
            glutPostRedisplay();
            break;
        case 'e':
            ModoDeExibicao = !ModoDeExibicao;
        case 'r':
            init();
            glutPostRedisplay();
            break;
        case 'w':
        case 's':
            moveObservador(key);
            break;
        default:
            cout << "Tecla " << key << " nao tem funcao definida" << endl;
            break;
  }
}

void arrow_keys(int a_keys, int x, int y) {
	switch (a_keys) {
        // When Up Arrow Is Pressed...
		case GLUT_KEY_UP:
			glutFullScreen();
			break;
        // When Down Arrow Is Pressed...
	    case GLUT_KEY_DOWN:
			glutInitWindowSize (700, 500);
			break;
        case GLUT_KEY_LEFT:
            VetorObservadorAlvo.rotacionaY(3.6f);
            break;
        case GLUT_KEY_RIGHT:
            VetorObservadorAlvo.rotacionaY(-3.6f);
            break;
		default:
			break;
	}
    PosicaoAlvo = PosicaoObservador + VetorObservadorAlvo;
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

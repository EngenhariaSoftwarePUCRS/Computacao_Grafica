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
#include <stdlib.h>

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
#include "DiagramaVoronoi.h"

#include "ListaDeCoresRGB.h"

#include "Temporizador.h"

Temporizador T;
double AccumDeltaT=0;

Poligono Pontos;

Voronoi Voro;
int *CoresDosPoligonos;

// Limites logicos da area de desenho
Ponto Min, Max, PontoClicado;

Ponto andante;
int poligonoAnterior = 0;
bool estaNoAnterior = false;
bool debug = false;
int PassoInicial(Ponto &p, bool &inPrevious);

bool FoiClicado = false;

float angulo=0.0;

// **********************************************************************
//
// **********************************************************************
void printString(string s, int posX, int posY)
{
    //defineCor(cor);
    glColor3f(1,1,1);
    glRasterPos3i(posX, posY, 0); //define posicao na tela
    for (int i = 0; i < s.length(); i++)
    {
//GLUT_BITMAP_HELVETICA_10, GLUT_BITMAP_TIMES_ROMAN_24,GLUT_BITMAP_HELVETICA_18
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
    }
}
// **********************************************************************
//
// **********************************************************************
void ImprimeNumeracaoDosVertices(Poligono &P)
{
    for(int i=0;i<P.getNVertices();i++)
    {
        Ponto aux;
        aux = P.getVertice(i);
        char msg[10];
        sprintf(msg,"%d",i);
        printString(msg,aux.x, aux.y);
    }
}
void ImprimeNroDoPoligono(Poligono P, int n) {
    char msg[10];

    sprintf(msg,"%d",n);

    Ponto Soma, A;
    for (int i = 0; i < P.getNVertices(); i++) {
        A = P.getVertice(i);
        Soma = Soma + A;
    }

    double div = 1.0/P.getNVertices();

    Soma = Soma * div;

    printString(msg, Soma.x, Soma.y);
}
// **********************************************************************
//
// **********************************************************************
void GeraPontos(int qtd)
{
    time_t t;
    Ponto Escala;
    Escala = (Max - Min) * (1.0/1000.0);
    srand((unsigned) time(&t));
    for (int i = 0;i<qtd; i++)
    {
        float x = rand() % 1000;
        float y = rand() % 1000;
        x = x * Escala.x + Min.x;
        y = y * Escala.y + Min.y;
        Pontos.insereVertice(Ponto(x,y));
    }
}

// **********************************************************************
//
// **********************************************************************
void init()
{
    srand(0);
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    Voro.LePoligonos("ListaDePoligonos-V2.txt");
    Voro.obtemLimites(Min,Max);
    if (debug) {
        Min.imprime("Minimo:", "\n");
        Max.imprime("Maximo:", "\n");
    }
    Ponto Largura;
    Largura = Max - Min;
    Min = Min - Largura * 0.1;
    Max = Max + Largura * 0.1;

    Voro.obtemVizinhosDasArestas();

    CoresDosPoligonos = new int[Voro.getNPoligonos()];
    for (int i = 0; i < Voro.getNPoligonos(); i++)
        CoresDosPoligonos[i] = i * 2;

    // Ajusta a largura da janela logica
    // em funcao do tamanho dos poligonos

    // Cria pontinho que será movido
    andante = Ponto(7.5, 6);

    bool inside;
    int qtdChamadasProdutoVetorial = 0;
    for (int i = 0; i < Voro.getNPoligonos(); i++) {
        poligonoAnterior = i;
        cout << "Poligono " << i << endl;
        qtdChamadasProdutoVetorial += PassoInicial(andante, inside);
        if (inside)
            break;
    }
    cout << "Chamadas Produto Vetorial Para Descobrir o poligono inicial: " << qtdChamadasProdutoVetorial << endl;
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

    if (AccumDeltaT > 1.0/30) // fixa a atualizacao da tela em 30
    {
        AccumDeltaT = 0;
        //angulo+=0.05;
        // glutPostRedisplay();
    }
    if (TempoTotal > 50.0)
    {
        if (debug) {
            cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
            cout << "Nros de Frames sem desenho: " << nFrames << endl;
            cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        }
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
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
// **********************************************************************
//
// **********************************************************************
void DesenhaLinha(Ponto P1, Ponto P2)
{
    glBegin(GL_LINES);
        glVertex3f(P1.x,P1.y,P1.z);
        glVertex3f(P2.x,P2.y,P2.z);
    glEnd();
}
// **********************************************************************
void DesenhaPonto(Ponto P, int tamanho)
{
    glPointSize(tamanho);
    glBegin(GL_POINTS);
        glVertex3f(P.x,P.y,P.z);
    glEnd();
}
// **********************************************************************
//  void display( void )
// **********************************************************************
void display(void)
{
	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites logicos da area OpenGL dentro da Janela
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

    Poligono P;
    for (int i = 0; i < Voro.getNPoligonos(); i++)
    {
        P = Voro.getPoligono(i);

        defineCor(CoresDosPoligonos[i]);
        P.pintaPoligono();

        glColor3f(0, 0, 0);
        P.desenhaPoligono();

        ImprimeNroDoPoligono(P, i);
    }

    Ponto FinalLinha(1000, 0);
    Ponto InicioLinha(andante - FinalLinha);
    DesenhaLinha(InicioLinha, andante);

    // Generate random RGB color
    float r = (rand() % 1000) / 1000.0;
    float g = (rand() % 1000) / 1000.0;
    float b = (rand() % 1000) / 1000.0;
    glColor3f(r, g, b);
    DesenhaPonto(andante, 15);

    glColor3f(1, 1, 1);
    Ponto P1, P2;
    for (int i = 0; i < Voro.getNPoligonos(); i++)
    {
        Poligono P = Voro.getPoligono(i);
        Ponto MaxPol, MinPol;
        P.obtemLimites(MinPol, MaxPol);
        int nroInterseccoes = 0;
        for (int j = 0; j < P.getNVertices(); j++)
        {
            P.getAresta(j, P1, P2);
            if (HaInterseccao(andante, InicioLinha, P1, P2)) {
                nroInterseccoes++;
                P.desenhaAresta(j);
            }
        }
        if (nroInterseccoes % 2 == 1)
            break;
    }

    glutSwapBuffers();
}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo numero de segundos e informa quanto frames
// se passaram neste periodo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;

    if (debug)
        cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            if (debug)
                cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }
}

/**
Inicialmente o teste deve ser feito para verificar se o ponto ainda está no mesmo polígono que estava antes do último movimento. Se estiver, nenhum outro teste deve ser feito. Como os polígonos são todos convexos, o teste deve ser realizado com o algoritmo de inclusão de pontos em polígonos convexos.

@return quantas vezes a função ProdVetorial foi chamada.
*/
int PassoInicial(Ponto &p, bool &inPrevious) {
    int qtdChamadasProdutoVetorial = 0;
    bool left, right = false;
    Poligono P = Voro.getPoligono(poligonoAnterior);
    Ponto x1, x2;
    for (int j = 0; j < P.getNVertices(); j++) {
        P.getAresta(j, x1, x2);
        Ponto direcao = x2 - x1;
        Ponto pontoAoVerticeInicial = x1 - p;
        qtdChamadasProdutoVetorial++;
        Ponto CrossProduct;
        ProdVetorial(direcao, pontoAoVerticeInicial, CrossProduct);
        float produtoVetorial = CrossProduct.z;
        if (produtoVetorial > 0)
            right = true;
        else if (produtoVetorial < 0)
            left = true;
    }
    // Se, para todos os lados do polígono, o sinal do produto vetorial for consistente (ou seja, todos positivos ou todos negativos), o ponto está dentro do polígono, e você define inside como verdadeiro.
    if ((right && !left) || (!right && left))
        inPrevious = true;

    return qtdChamadasProdutoVetorial;
}

/**
Inclusão de pontos em polígonos côncavos:  O teste de inclusão deve ser realizado 
somente com os polígonos cujos envelopes cruzarem a linha horizontal usada para o teste. 

@return quantas vezes a função HaIntersecao foi chamada. 
*/
// int InclusaoPontosPoligonosConcavos(Ponto &p) {
//     int qtdChamadasHaInterseccao = 0;
//     Ponto P1, P2;
//     for (int i = 0; i < Voro.getNPoligonos(); i++) {
//         Poligono P = Voro.getPoligono(i);
//         Ponto MaxPol, MinPol;
//         P.obtemLimites(MinPol, MaxPol);
//         if (p.y > MinPol.y && p.y < MaxPol.y && p.x > MinPol.x && p.x < MaxPol.x) {
//             int nroInterseccoes = 0;
//             for (int j = 0; j < P.getNVertices(); j++) {
//                 P.getAresta(j, P1, P2);
//                 qtdChamadasHaInterseccao++;
//                 if (HaInterseccao(p, P1, P2)) {
//                     nroInterseccoes++;
//                     P.desenhaAresta(j);
//                 }
//             }
//             if (nroInterseccoes % 2 == 1) {
//                 cout << "Ponto dentro do poligono" << endl;
//                 poligonoAnterior = i;
//                 break;
//             }
//         }
//     }

//     return qtdChamadasHaInterseccao;
// }

int checkPointPosition(Ponto &p) {
    Ponto FinalLinha(1000, 0);
    Ponto InicioLinha(andante - FinalLinha);
    glColor3f(1, 1, 1);
    DesenhaLinha(InicioLinha, andante);

    glColor3f(1, 0, 0);
    DesenhaPonto(andante, 10);

    glColor3f(1, 0, 1);
    Ponto P1, P2;
    int qtdChamadasProdutoVetorial = 0;
    cout << "\n\n===============================" << endl;
    cout << "Pol anterior " << poligonoAnterior << endl;
    for (int i = 0; i < Voro.getNPoligonos(); i++)
    {
        Poligono P = Voro.getPoligono(i);
        Ponto MaxPol, MinPol;
        P.obtemLimites(MinPol, MaxPol);
        int nroInterseccoes = 0;
        for (int j = 0; j < P.getNVertices(); j++)
        {
            P.getAresta(j, P1, P2);
            qtdChamadasProdutoVetorial++;
            if (HaInterseccao(andante, InicioLinha, P1, P2)) {
                nroInterseccoes++;
                P.desenhaAresta(j);
            }
        }
        // if (nroInterseccoes > 0) {
            cout << "\nAnalisando novo poligono" << endl;
            cout << "Nro int com o poligono " << i << " = " << nroInterseccoes << endl;
            if (nroInterseccoes % 2 == 1) {
                cout << "Ponto dentro do poligono" << endl;
                poligonoAnterior = i;
                break;
            } else
                cout << "Ponto fora do poligono" << endl;
        // }
    }

    cout << "Pol anterior " << poligonoAnterior << endl;
    glutPostRedisplay();

    return qtdChamadasProdutoVetorial;
}

bool checkPointInBounds(Ponto &p) {
    if (p.x > Max.x)
        p.x = Min.x;
    else if (p.x < Min.x)
        p.x = Max.x;

    if (p.y > Max.y)
        p.y = Min.y;
    else if (p.y < Min.y)
        p.y = Max.y;

    bool inBounds = true;
    Ponto Min, Max;
    Voro.obtemLimites(Min, Max);

    if (p.x < Min.x || p.x > Max.x)
        inBounds = false;
    if (p.y < Min.y || p.y > Max.y)
        inBounds = false;

    return inBounds;
}

void movePointVertical(Ponto &p, float distance) {
    p.y += distance;
}

void movePointHorizontal(Ponto &p, float distance) {
    p.x += distance;
}

void movePoint(char key) {
    switch (key) {
        case 'w':
            movePointVertical(andante, 0.3);
            break;
        case 'a':
            movePointHorizontal(andante, -0.3);
            break;
        case 's':
            movePointVertical(andante, -0.3);
            break;
        case 'd':
            movePointHorizontal(andante, 0.3);
            break;
    }
    
    // Passo Opcional: Checar se o ponto esta entre os polígonos
    cout << "\t[SYS]" << "Checando se o ponto esta dentro da área do diagrama..." << endl;
    bool inBounds = checkPointInBounds(andante);
    if (!inBounds) {
        cout << "Me poe pra dentro por favor ._." << endl;
        return;
    }

    // Passo 0: Checar se o ponto esta no mesmo polígono que estava antes do último movimento
    cout << "\t[SYS]" << "Checando se o ponto esta no mesmo polígono que estava antes do último movimento..." << endl;
    bool inPrevious;
    int qtdChamadasProdutoVetorial = PassoInicial(andante, inPrevious);
    cout << "Chamadas Produto Vetorial (Passo Inicial): " << qtdChamadasProdutoVetorial << endl;
    if (inPrevious) {
        cout << "Sigo no mesmo poligono" << endl;
        return;
    }

    // Passo 1: Inclusao de pontos em poligonos concavos
    cout << "\t[SYS]" << "Inclusao de pontos em poligonos concavos..." << endl;
    // int qtdChamadasHaInterseccao = InclusaoPontosPoligonosConcavos(andante);

    // Passo 2: Inclusao de pontos em poligonos convexos
    cout << "Passo 2: Inclusao de pontos em poligonos convexos" << endl;

    // Passo 3: Inclusao de pontos em poligonos convexos utilizando a informação de vizinhança
    cout << "Passo 3: Inclusao de pontos em poligonos convexos utilizando a informação de vizinhança" << endl;
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
        case 'w':
        case 'a':
        case 's':
        case 'd':
            movePoint(key);
            break;
        case 't':
            ContaTempo(3);
            break;
		default:
			break;
	}

	glutPostRedisplay();
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
    if(button!=GLUT_LEFT_BUTTON)
     return;
    if (debug)
        cout << "Botao da Esquerda! ";

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
    if (debug)
        cout << "Programa OpenGL" << endl;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition(724, 180);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize(1080, 720);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de titulo da janela.
    glutCreateWindow("Trabalho 1 - Felipe Freitas e Lucas Wolschick - Computacao Grafica - 2023/2 - Prof. Marcio Pinho");

    // executa algumas inicializacoes
    init();

    // Define que o tratador de evento para
    // o redesenho da tela sera a funcao "display"
    // Esta sera chamada automaticamente quando
    // for necessario redesenhar a janela
    glutDisplayFunc(display);

    // Define que o tratador de evento para
    // a invalidacao da tela e a funcao "animate"
    // Esta sera chamada automaticamente sempre que a
    // maquina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela e  funcao "reshape"
    // Esta sera chamada automaticamente quando
    // o usuario alterar o tamanho da janela
    glutReshapeFunc(reshape);

    // Define que o tratador de evento para
    // as teclas e a funcao "keyboard"
    // Esta sera chamada automaticamente sempre
    // o usuario pressionar uma tecla comum
    glutKeyboardFunc(keyboard);

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...) e a funcao "arrow_keys"
    // Esta sera chamada automaticamente sempre
    // que o usuario pressionar uma tecla especial
    glutSpecialFunc(arrow_keys);

    glutMouseFunc(Mouse);

    // inicia o tratamento dos eventos
    glutMainLoop();

    return 0;
}

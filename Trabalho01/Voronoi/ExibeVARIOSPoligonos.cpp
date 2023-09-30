// **********************************************************************
// PUCRS/Escola Politecnica
// COMPUTACAO GRAFICA
//
// Programa basico para criar detectar posicionamento de um ponto dentro de um Diagrama de Voronoi
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
//
// Felipe Freitas Silva
// f.freitas007@edu.pucrs.br
//
// Lucas Wolschick
// lucas.wolschick@edu.pucrs.br
// **********************************************************************


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
double AccumDeltaT = 0;

Poligono Pontos;

Voronoi Voro;
int *CoresDosPoligonos;

// Limites logicos da area de desenho
Ponto Min, Max, PontoClicado;

bool FoiClicado = false;

float angulo = 0.0;

// Variaveis e declaracoes de funcao criadas para a resolucao do trabalho
Ponto andante;
// Estas informacoes sao variaveis pois dependem do tamanho da janela
float tamanhoAndante, passoAndante;
// Variavel que guarda o polígono que o ponto estava antes do último movimento
int poligonoAnterior = 0;
// Debug = imprime mais informacoes na tela
bool debug = true;
// IncluiPontosPoligonosConcavos = desenha a linha ate o ponto quando o metodo de inclusao de pontos em poligonos côncavos e chamado
bool incluiPontosPoligonosConcavos;
// IncluiPontosPoligonosConvexos = pinta a aresta que foi cruzada quando o metodo de inclusao de pontos em poligonos convexos e chamado
bool incluiPontosPoligonosConvexos;
int PassoInicial(Ponto &p, bool &estaNoPoligonoAnterior);
int InclusaoPontosPoligonosConcavos(Ponto &p, int &poligonoPosMovimento, bool debug);
int InclusaoPontosPoligonosConvexos(Ponto &p, int &poligonoPosMovimento, Poligono &P, int &arestaCruzada, bool debug);

// **********************************************************************
//
// **********************************************************************
void printString(string s, int posX, int posY)
{
    glColor3f(1, 1, 1);
    // define posicao na tela
    glRasterPos3i(posX, posY, 0);
    for (int i = 0; i < s.length(); i++)
    {
        // GLUT_BITMAP_HELVETICA_10, GLUT_BITMAP_TIMES_ROMAN_24,GLUT_BITMAP_HELVETICA_18
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, s[i]);
    }
}
// **********************************************************************
//
// **********************************************************************
void ImprimeNumeracaoDosVertices(Poligono &P)
{
    for(int i = 0;i < P.getNVertices(); i++)
    {
        Ponto aux = P.getVertice(i);
        char msg[10];
        sprintf(msg, "%d", i);
        printString(msg, aux.x, aux.y);
    }
}
// **********************************************************************
//
// **********************************************************************
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
    for (int i = 0; i < qtd; i++)
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
    cout << "[SYS]" << " Inicializando programa..." << endl;
    srand(0);
    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    cout << "[SYS]" << " Inicializando Diagrama de Voronoi..." << endl;
    char* nomeArquivo = "casosTeste/5.txt";
    cout << "[SYS]" << " Lendo poligonos do arquivo " << nomeArquivo << "..." << endl;
    Voro.LePoligonos(nomeArquivo);
    cout << "[SYS]" << " Calculando limites do diagrama Voronoi..." << endl;
    Voro.obtemLimites(Min, Max);
    Ponto Largura = Max - Min;
    Min = Min - Largura * 0.1;
    Max = Max + Largura * 0.1;
    Ponto Meio = (Max + Min) * 0.5;
    if (debug) {
        cout << "\tLargura: " << Largura.x << endl;
        Min.imprime("\tMinimo:", "\n");
        Max.imprime("\tMaximo:", "\n");
        Meio.imprime("\tMeio:", "\n");
    }
    cout << "[SYS]" << " Limites do diagrama Voronoi calculados!" << endl;

    cout << "[SYS]" << " Analizando informacoes de vizinhanca..." << endl;
    Voro.obtemVizinhosDasArestas();
    for (int i = 0; i < Voro.getNPoligonos(); i++) {
        Poligono P = Voro.getPoligono(i);
        if (debug)
            cout << "\tPoligono " << i << " tem " << P.getNVizinhos() << " vizinhos." << endl;
    }
    cout << "[SYS]" << " Informacoes de vizinhanca analizadas!" << endl;

    cout << "[SYS]" << " Gerando cores dos poligonos..." << endl;
    CoresDosPoligonos = new int[Voro.getNPoligonos()];
    for (int i = 0; i < Voro.getNPoligonos(); i++)
        CoresDosPoligonos[i] = i * 2;
    cout << "[SYS]" << " Cores dos poligonos geradas!" << endl;

    // Cria pontinho que sera movido
    cout << "[SYS]" << " Criando ponto que sera movido..." << endl;
    tamanhoAndante = Max.x * 0.01;
    passoAndante = tamanhoAndante;
    andante = Ponto(Meio.x, Meio.y);
    cout << "[SYS]" << " Ponto criado!" << endl;

    cout << "[SYS]" << " Computando posicao inicial do ponto..." << endl;
    bool inside = false;
    int qtdChamadasProdutoVetorial = 0;
    for (int i = 0; i < Voro.getNPoligonos(); i++) {
        poligonoAnterior = i;
        if (debug)
            cout << "\tAnalisando se estou dentro do poligono " << i << endl;
        qtdChamadasProdutoVetorial += PassoInicial(andante, inside);
        if (inside)
            break;
        cout << "\tTestando se o ponto esta dentro de outro poligono..." << endl;
    }
    cout << "\tQuantidade de chamadas para ProdVetorial para descobrir o poligono inicial: " << qtdChamadasProdutoVetorial << endl;
    cout << "[SYS]" << " Posicao inicial do ponto computada! ";andante.imprime();cout << endl;
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
    if (debug)
        cout << "\n[SYS]" << " Redesenhando a tela..." << endl;
	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites logicos da area OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1, 1, 1); // R, G, B  [0..1]
    DesenhaEixos();

    glRotatef(angulo, 0, 0, 1);
    glLineWidth(2);

    // Desenha os poligonos
    if (debug)
        cout << "[SYS]" << " Redesenhando os poligonos..." << endl;
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

    // Gera uma cor aleatória para o ponto cada vez que ele e movido
    if (debug)
        cout << "[SYS]" << " Gerando cor aleatoria para o ponto..." << endl;
    float r = (rand() % 1000) / 1000.0;
    float g = (rand() % 1000) / 1000.0;
    float b = (rand() % 1000) / 1000.0;
    glColor3f(r, g, b);
    DesenhaPonto(andante, tamanhoAndante);

    // Desenha a linha na tela ate o ponto quando o metodo de inclusao de pontos em poligonos côncavos e chamado
    if (incluiPontosPoligonosConcavos) {
        int poligonoPosMovimento;
        if (debug)
            cout << "[SYS]" << " Desenhando linha ate o ponto..." << endl;
        InclusaoPontosPoligonosConcavos(andante, poligonoPosMovimento, false);
        incluiPontosPoligonosConcavos = false;
    }

    // Pinta a aresta que foi cruzada quando o metodo de inclusao de pontos em poligonos convexos e chamado
    if (incluiPontosPoligonosConvexos) {
        int poligonoPosMovimento, arestaCruzada;
        Poligono P = Voro.getPoligono(poligonoAnterior);
        if (debug)
            cout << "[SYS]" << " Pintando aresta que foi cruzada..." << endl;
        InclusaoPontosPoligonosConvexos(andante, poligonoPosMovimento, P, arestaCruzada, false);
        incluiPontosPoligonosConvexos = false;
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
Verifica se o ponto P esta dentro da janela de visualizacao.

@return true se o ponto esta dentro da janela do diagrama de Voronoi, false caso contrario.
*/
bool VerificaPontoDentroLimites(Ponto &p) {
    // if (debug)
    //     cout << "[SYS]" << " Verificando se o ponto esta dentro da janela..." << endl;

    if (p.x > Max.x) {
        if (debug)
            cout << "\tPonto estava fora dos limites, reposicionando...";
        p.x = Min.x;
    } else if (p.x < Min.x) {
        if (debug)
            cout << "\tPonto estava fora dos limites, reposicionando...";
        p.x = Max.x;
    }

    if (p.y > Max.y) {
        if (debug)
            cout << "\tPonto estava fora dos limites, reposicionando...";
        p.y = Min.y;
    } else if (p.y < Min.y) {
        if (debug)
            cout << "\tPonto estava fora dos limites, reposicionando...";
        p.y = Max.y;
    }

    bool dentroLimites = true;
    Ponto Min, Max;
    Voro.obtemLimites(Min, Max);

    cout << "[SYS]" << " Verificando se o ponto esta dentro dos limites do diagrama..." << endl;
    if (p.x < Min.x || p.x > Max.x)
        dentroLimites = false;
    if (p.y < Min.y || p.y > Max.y)
        dentroLimites = false;

    return dentroLimites;
}

/**
Inicialmente o teste deve ser feito para verificar se o ponto ainda esta no mesmo polígono que estava antes do ultimo movimento. Se estiver, nenhum outro teste deve ser feito. Como os polígonos sao todos convexos, o teste deve ser realizado com o algoritmo de inclusao de pontos em polígonos convexos.

@return quantas vezes a funcao ProdVetorial foi chamada.
*/
int PassoInicial(Ponto &p, bool &estaNoPoligonoAnterior) {
    if (debug)
        cout << "[SYS]" << " Fazendo passo inicial..." << endl;

    int qtdChamadasProdutoVetorial = 0;
    bool left = false, right = false;
    Poligono P = Voro.getPoligono(poligonoAnterior);
    Ponto x1, x2;

    if (debug)
        cout << "[SYS]" << " Verificando se o ponto ainda esta dentro do poligono " << poligonoAnterior << "..." << endl;
    for (int j = 0; j < P.getNVertices(); j++) {
        P.getAresta(j, x1, x2);
        Ponto direcao = x2 - x1;
        Ponto pontoAoVerticeInicial = x1 - p;

        qtdChamadasProdutoVetorial++;
        Ponto CrossProduct;
        ProdVetorial(direcao, pontoAoVerticeInicial, CrossProduct);

        float produtoVetorial = CrossProduct.z;
        if (produtoVetorial > 0) {
            if (debug)
                cout << "\tPonto esta a direita da aresta " << j << endl;
            right = true;
        } else if (produtoVetorial < 0) {
            if (debug)
                cout << "\tPonto esta a esquerda da aresta " << j << endl;
            left = true;
        }
    }
    // Se, para todos os lados do polígono, o sinal do produto vetorial for consistente (ou seja, todos positivos ou todos negativos), o ponto esta dentro do polígono, e você define inside como verdadeiro.
    if (right != left) {
        if (debug)
            cout << "\tSinal do produto vetorial consistente, entao o ponto esta dentro do poligono " << poligonoAnterior << endl;
        estaNoPoligonoAnterior = true;
    } else {
        if (debug)
            cout << "\tSinal do produto vetorial inconsistente, entao o ponto nao esta dentro do poligono " << poligonoAnterior << endl;
    }

    return qtdChamadasProdutoVetorial;
}

/**
Inclusao de pontos em polígonos côncavos:  O teste de inclusao deve ser realizado somente com os polígonos cujos envelopes cruzarem a linha horizontal usada para o teste.

@return quantas vezes a funcao HaIntersecao foi chamada.
*/
int InclusaoPontosPoligonosConcavos(Ponto &p, int &poligonoPosMovimento, bool debug = true) {
    // if (debug)
    //     cout << "[SYS]" << " Fazendo teste de inclusao de pontos em poligonos concavos..." << endl;

    int qtdChamadasHaInterseccao = 0;

    Ponto FinalLinha(1000, 0);
    Ponto InicioLinha(andante - FinalLinha);
    glColor3f(1, 1, 1);
    DesenhaLinha(InicioLinha, andante);

    Poligono P;
    for (int i = 0; i < Voro.getNPoligonos(); i++) {
        if (i == poligonoAnterior) {
            if (debug)
                cout << "\tJa sei que nao estou no mesmo poligono que estava antes do ultimo movimento, entao nao preciso testar o poligono " << i << endl;
            continue;
        }

        P = Voro.getPoligono(i);
        Ponto MaxPol, MinPol;
        P.obtemLimites(MinPol, MaxPol);

        if (andante.y > MaxPol.y || andante.y < MinPol.y) {
            if (debug)
                cout << "\tPonto esta fora das faixas de interseccao do poligono " << i << ", entao nao preciso testar o poligono " << i << endl;
            continue;
        }

        int nroInterseccoes = 0;
        Ponto pA, pB;
        for (int j = 0; j < P.getNVertices(); j++)
        {
            P.getAresta(j, pA, pB);
            qtdChamadasHaInterseccao++;
            if (HaInterseccao(andante, InicioLinha, pA, pB)) {
                if (debug)
                    cout << "\tAresta " << j << " do poligono " << i << " cruza a linha horizontal!" << endl;
                nroInterseccoes++;
                P.desenhaAresta(j);
            } else {
                if (debug)
                    cout << "\tAresta " << j << " do poligono " << i << " nao cruza a linha horizontal!" << endl;
            }
        }
        if (nroInterseccoes % 2 == 1) {
            if (debug)
                cout << "\tNumero de interseccoes e impar, entao o ponto esta dentro do poligono " << i << endl;
            poligonoPosMovimento = i;
            break;
        }
    }

    return qtdChamadasHaInterseccao;
}

/**
Inclusao de pontos em polígonos convexos:  O teste de inclusao deve ser realizado somente com os polígonos cujos envelopes contiverem o ponto.

@return quantas vezes a funcao ProdVetorial foi chamada.
*/
int InclusaoPontosPoligonosConvexos(Ponto &p, int &poligonoPosMovimento) {
    // if (debug)
    //     cout << "[SYS]" << " Fazendo teste de inclusao de pontos em poligonos convexos..." << endl;

    int qtdChamadasProdutoVetorial = 0;
    bool left, right;
    Ponto pA, pB;
    Poligono P;

    for (int i = 0; i < Voro.getNPoligonos(); i++) {
        if (i == poligonoAnterior) {
            if (debug)
                cout << "\tJa sei que nao estou no mesmo poligono que estava antes do ultimo movimento, entao nao preciso testar o poligono " << i << endl;
            continue;
        }

        P = Voro.getPoligono(i);
        if (!P.pontoEstaDentro(p)) {
            if (debug)
                cout << "\tPonto nao esta dentro do envelope do poligono " << i << ", entao nao preciso testar o poligono " << i << endl;
            continue;
        }

        left = right = false;

        for (int j = 0; j < P.getNVertices(); j++) {
            P.getAresta(j, pA, pB);
            Ponto direcao = pB - pA;
            Ponto pontoAoVerticeInicial = pA - p;

            qtdChamadasProdutoVetorial++;
            Ponto CrossProduct;
            ProdVetorial(direcao, pontoAoVerticeInicial, CrossProduct);

            float produtoVetorial = CrossProduct.z;
            if (produtoVetorial > 0) {
                if (debug)
                    cout << "\tPonto esta a direita da aresta " << j << " do poligono " << i << endl;
                right = true;
            } else if (produtoVetorial < 0) {
                if (debug)
                    cout << "\tPonto esta a esquerda da aresta " << j << " do poligono " << i << endl;
                left = true;
            }
        }
        // Se, para todos os lados do polígono, o sinal do produto vetorial for consistente (ou seja, todos positivos ou todos negativos), o ponto esta dentro do polígono, e você define inside como verdadeiro.
        if (right != left) {
            if (debug)
                cout << "\tSinal do produto vetorial consistente, entao o ponto esta dentro do poligono " << i << endl;
            poligonoPosMovimento = i;
            break;
        } else {
            if (debug)
                cout << "\tSinal do produto vetorial inconsistente, entao o ponto nao esta dentro do poligono " << i << endl;
        }
    }

    return qtdChamadasProdutoVetorial;
}

/**
Inclusao de pontos em polígonos convexos utilizando a informacao de vizinhanca disponível no Diagrama de Voronoi: o novo polígono deve ser determinado testando qual aresta foi o “cruzada” quando o ponto saiu do polígono.

@return quantas vezes a funcao ProdVetorial foi chamada.
*/
int InclusaoPontosPoligonosConvexos(Ponto &p, int &poligonoPosMovimento, Poligono &P, int &arestaCruzada, bool debug = true) {
    // if (debug)
    //     cout << "[SYS]" << " Fazendo teste de inclusao de pontos em poligonos convexos utilizando a informacao de vizinhanca..." << endl;

    int qtdChamadasProdutoVetorial = 0;
    bool left, right;
    Ponto pA, pB;
    Poligono Vizinho;

    if (debug)
        cout << "\tQtd de vizinhos encontrados: " << P.getNVizinhos() << endl;
    for (int i = 0; i < P.getNVizinhos(); i++) {
        if (debug)
            cout << "\tComparando com " << i + 1 << "o vizinho" << endl;

        Vizinho = P.getVizinho(i);
        // if (!Vizinho.pontoEstaDentro(p)) {
        //     if (debug)
        //         cout << "\tPonto nao esta dentro do envelope do poligono " << i << ", entao nao preciso testar o poligono " << i << endl;
        //     continue;
        // }

        left = right = false;

        for (int j = 0; j < Vizinho.getNVertices(); j++) {
            if (debug)
                cout << "\t\tTestando aresta " << j << " do vizinho " << i + 1 << endl;
            Vizinho.getAresta(j, pA, pB);
            Ponto direcao = pB - pA;
            Ponto pontoAoVerticeInicial = pA - p;

            qtdChamadasProdutoVetorial++;
            Ponto CrossProduct;
            ProdVetorial(direcao, pontoAoVerticeInicial, CrossProduct);

            float produtoVetorial = CrossProduct.z;
            if (produtoVetorial == 0) {
                if (debug)
                    cout << "\t\tPonto esta na aresta " << j << " do vizinho " << i + 1 << endl;
                continue;
            } else if (produtoVetorial > 0) {
                if (debug)
                    cout << "\t\tPonto esta a direita da aresta " << j << " do vizinho " << i + 1 << endl;
                right = true;
            } else if (produtoVetorial < 0) {
                if (debug)
                    cout << "\t\tPonto esta a esquerda da aresta " << j << " do vizinho " << i + 1 << endl;
                left = true;
            }
        }
        // Se, para todos os lados do polígono, o sinal do produto vetorial for consistente (ou seja, todos positivos ou todos negativos), o ponto esta dentro do polígono, e você define inside como verdadeiro.
        if (right != left) {
            if (debug)
                cout << "\t\tSinal do produto vetorial consistente, entao o ponto esta dentro do vizinho " << i + 1 << endl;
            arestaCruzada = i;
            break;
        }
    }
    
    Poligono PoligonoVizinho;
    for (int i = 0; i < Voro.getNPoligonos(); i++) {
        PoligonoVizinho = Voro.getPoligono(i);
        if (Vizinho == PoligonoVizinho) {
            poligonoPosMovimento = i;
            if (debug)
                cout << "\tO ponto foi para o poligono " << i << ", que e o " << i + 1 << "o vizinho" << endl;
            break;
        }
    }

    return qtdChamadasProdutoVetorial;
}

void movePointVertical(Ponto &p, float distance) {
    if (debug)
        cout << "[SYS]" << " Movendo ponto verticalmente " << distance << " unidades..." << endl;
    p.y += distance;
}

void movePointHorizontal(Ponto &p, float distance) {
    if (debug)
        cout << "[SYS]" << " Movendo ponto horizontalmente " << distance << " unidades..." << endl;
    p.x += distance;
}

void movePoint(char key) {
    switch (key) {
        case 'q':
            if (debug)
                cout << "[SYS]" << " Movendo ponto para a diagonal superior esquerda..." << endl;
            movePointHorizontal(andante, -passoAndante);
            movePointVertical(andante, passoAndante);
            break;
        case 'w':
            if (debug)
                cout << "[SYS]" << " Movendo ponto para cima..." << endl;
            movePointVertical(andante, passoAndante);
            break;
        case 'e':
            if (debug)
                cout << "[SYS]" << " Movendo ponto para a diagonal superior direita..." << endl;
            movePointHorizontal(andante, passoAndante);
            movePointVertical(andante, passoAndante);
            break;
        case 'a':
            if (debug)
                cout << "[SYS]" << " Movendo ponto para a esquerda..." << endl;
            movePointHorizontal(andante, -passoAndante);
            break;
        case 's':
            if (debug)
                cout << "[SYS]" << " Movendo ponto para baixo..." << endl;
            movePointVertical(andante, -passoAndante);
            break;
        case 'd':
            if (debug)
                cout << "[SYS]" << " Movendo ponto para a direita..." << endl;
            movePointHorizontal(andante, passoAndante);
            break;
        case 'z':
            if (debug)
                cout << "[SYS]" << " Movendo ponto para a diagonal inferior esquerda..." << endl;
            movePointHorizontal(andante, -passoAndante);
            movePointVertical(andante, -passoAndante);
            break;
        case 'x':
            if (debug)
                cout << "[SYS]" << " Movendo ponto para baixo..." << endl;
            movePointVertical(andante, -passoAndante);
            movePointHorizontal(andante, passoAndante);
            break;
    }

    cout << "\n\n=====================================================================================================" << endl;

    // Passo Opcional: Checar se o ponto esta entre os poligonos
    cout << "[SYS] " << "Checando se o ponto esta dentro dos limites da janela e do diagrama..." << endl;
    bool dentroLimites = VerificaPontoDentroLimites(andante);
    if (!dentroLimites) {
        cout << "[SYS] " << "O ponto esta fora da area do diagrama!" << endl;
        cout << "[SYS] " << "Nao ha mais nada a ser feito." << endl;
        return;
    }
    cout << "[SYS] " << "O ponto esta dentro da area do diagrama!" << endl;
    cout << "[SYS] " << "Continuando...\n" << endl;

    // Passo 0: Checar se o ponto esta no mesmo poligono que estava antes do ultimo movimento
    int poligonoPosMovimento;
    cout << "[SYS] " << "Checando se o ponto esta no mesmo poligono que estava antes do ultimo movimento..." << endl;
    bool estaNoPoligonoAnterior = false;
    int qtdChamadasProdutoVetorial = PassoInicial(andante, estaNoPoligonoAnterior);
    cout << "\tChamadas Produto Vetorial (Passo Inicial): " << qtdChamadasProdutoVetorial << endl;
    if (estaNoPoligonoAnterior) {
        cout << "\tSigo no mesmo poligono (" << poligonoAnterior << ")" << endl;
        cout << "\tNao ha mais nada a ser feito." << endl;
        return;
    }
    cout << "\tO ponto nao esta no mesmo poligono que estava antes do ultimo movimento!" << endl;
    cout << "[SYS] " << "Continuando...\n" << endl;

    // Passo 1: Inclusao de pontos em poligonos concavos
    cout << "[SYS] " << "Fazendo teste de inclusao de pontos em poligonos concavos..." << endl;
    int qtdChamadasHaInterseccao = InclusaoPontosPoligonosConcavos(andante, poligonoPosMovimento);
    incluiPontosPoligonosConcavos = true;
    cout << "\tChamadas HaInterseccao (InclusaoPontosPoligonosConcavos): " << qtdChamadasHaInterseccao << endl;
    cout << "[SYS] " << "Continuando...\n" << endl;

    // Passo 2: Inclusao de pontos em poligonos convexos
    cout << "[SYS] " << "Fazendo teste de inclusao de pontos em poligonos convexos..." << endl;
    qtdChamadasProdutoVetorial = InclusaoPontosPoligonosConvexos(andante, poligonoPosMovimento);
    cout << "\tChamadas Produto Vetorial (InclusaoPontosPoligonosConvexos): " << qtdChamadasProdutoVetorial << endl;
    cout << "[SYS] " << "Continuando...\n" << endl;

    // Passo 3: Inclusao de pontos em poligonos convexos utilizando a informacao de vizinhanca
    cout << "[SYS] " << "Fazendo teste de inclusao de pontos em poligonos convexos utilizando a informacao de vizinhanca..." << endl;
    Poligono P = Voro.getPoligono(poligonoAnterior);
    int arestaCruzada;
    qtdChamadasProdutoVetorial = InclusaoPontosPoligonosConvexos(andante, poligonoPosMovimento, P, arestaCruzada);
    cout << "\tChamadas Produto Vetorial (InclusaoPontosPoligonosConvexos): " << qtdChamadasProdutoVetorial << endl;
    cout << "\tAresta cruzada: " << arestaCruzada << endl;
    cout << "[SYS] " << "Continuando..." << endl;

    poligonoAnterior = poligonoPosMovimento;

    cout << "=====================================================================================================" << endl;
}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
// **********************************************************************
void keyboard ( unsigned char key, int x, int y )
{
	switch ( key )
	{
		case 27: // ESC
            cout << "Finalizando o programa." << endl;
            cout << "Obrigado por usar o programa!" << endl;
			exit(0);
			break;
        case 'q':
        case 'w':
        case 'e':
        case 'a':
        case 's':
        case 'd':
        case 'z':
        case 'x':
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
// Esta funcao captura o clique do botao direito do mouse sobre a �rea de
// desenho e converte a coordenada para o sistema de refer�ncia definido
// na glOrtho (ver funcao reshape)
// Este codigo e baseado em http://hamala.se/forums/viewtopic.php?t=20
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
    if (debug)
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

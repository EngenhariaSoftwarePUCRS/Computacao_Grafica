//
//  DiagramaVoronoi.cpp
//  OpenGLTest
//
//  Created by Márcio Sarroglia Pinho on 23/08/23.
//  Copyright © 2023 Márcio Sarroglia Pinho. All rights reserved.
//

#include "DiagramaVoronoi.h"
#include "Envelope.h"

ifstream input;

Voronoi::Voronoi()
{
    debug = false;
}

Poligono Voronoi::LeUmPoligono()
{
    Poligono P;
    unsigned int qtdVertices;
    input >> qtdVertices;  // arq << qtdVertices
    for (int i=0; i< qtdVertices; i++)
    {
        double x,y;
        // Le um ponto
        input >> x >> y;
        if (debug)
            Ponto(x, y).imprime();
        if(!input)
        {
            if (debug)
                cout << "Fim inesperado da linha." << endl;
            break;
        }
        P.insereVertice(Ponto(x,y));
    }
    if (debug)
        cout << "Poligono lido com sucesso!" << endl;
    return P;
}

void Voronoi::LePoligonos(const char *nome)
{
    input.open(nome, ios::in); //arq.open(nome, ios::out);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    string S;

    input >> qtdDePoligonos;
    if (debug)
        cout << "qtdDePoligonos:" << qtdDePoligonos << endl;
    Ponto A, B;
    Diagrama[0] = LeUmPoligono();
    Diagrama[0].obtemLimites(A, B); // obtem o envelope do poligono
    Diagrama[0].envelopa(A, B);
    for (int i = 1; i < qtdDePoligonos; i++)
    {
        Diagrama[i] = LeUmPoligono();
        Diagrama[i].obtemLimites(A, B); // obtem o envelope do poligono
        Diagrama[i].envelopa(A, B);

        Min = ObtemMinimo(A, Min);
        Max = ObtemMaximo(B, Max);
    }
    if (debug)
        cout << "Lista de Poligonos lida com sucesso!" << endl;
}

Poligono Voronoi::getPoligono(int i)
{
    if (i >= qtdDePoligonos)
    {
        if (debug)
            cout << "Nro de Poligono Inexistente" << endl;
        return Diagrama[0];
    }
    return Diagrama[i];
}

unsigned int Voronoi::getNPoligonos()
{
    return qtdDePoligonos;
}

void Voronoi::obtemLimites(Ponto &min, Ponto &max)
{
    min = this->Min;
    max = this->Max;
}

void Voronoi::obtemVizinhosDasArestas()
{
    Poligono *P1, *P2;
    Envelope e1, e2;
    for (int i = 0; i < qtdDePoligonos; i++)
    {
        P1 = &Diagrama[i];
        e1 = P1->getEnvelope();
        for (int j = 0; j < qtdDePoligonos; j++)
        {
            if (i == j)
                continue;
            P2 = &Diagrama[j];
            e2 = P2->getEnvelope();
            if (e1.temColisao(e2))
            {
                if (debug)
                    cout << "Poligono " << i << " tem vizinho " << j << endl;
                P1->insereVizinho(*P2);
            }
        }
    }
}

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
        if (debug)
            cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    string S;

    input >> qtdDePoligonos;
    if (debug)
        cout << "qtdDePoligonos:" << qtdDePoligonos << endl;
    Ponto A, B;
    Diagrama[0] = LeUmPoligono();
    Diagrama[0].obtemLimites(Min, Max);// obtem o envelope do poligono
    Diagrama[0].envelope = Envelope(Min, Max);
    for (int i=1; i< qtdDePoligonos; i++)
    {
        Diagrama[i] = LeUmPoligono();
        Diagrama[i].obtemLimites(A, B); // obtem o envelope do poligono
        Diagrama[i].envelope = Envelope(Min, Max);

        Min = ObtemMinimo (A, Min);
        Max = ObtemMaximo (B, Max);
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
    for (int i = 0; i < qtdDePoligonos; i++) {
        Poligono p1 = this->getPoligono(i);
        Poligono p2 = this->getPoligono((i + 1) % qtdDePoligonos);

        for (int j = 0; j < p1.getNVertices(); j++) {
            Ponto p1a = p1.getVertice(j);
            Ponto p1b = p1.getVertice((j + 1) % p1.getNVertices());

            for (int k = 0; k < p2.getNVertices(); k++) {
                Ponto p2a = p2.getVertice(k);
                Ponto p2b = p2.getVertice((k + 1) % p2.getNVertices());

                if (p1a == p2b && p1b == p2a) {
                    p1.insereVizinho(p2);
                    p2.insereVizinho(p1);
                }
            }
        }
    }
}

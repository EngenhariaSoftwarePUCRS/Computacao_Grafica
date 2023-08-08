//
//  Matematica.cpp
//  Testes
//
//  Created by Márcio Sarroglia Pinho on 19/03/20.
//  Copyright © 2020 Márcio Sarroglia Pinho. All rights reserved.
//
#include <stdio.h>

class Vetor{
    double x,y;
public:
    Vetor(){ x=0; y=0;} // construtora
    void versor()
    {
        
    }
    void set(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
    void get (double &x, double &y)
    {
        x = this->x;
        y = this->y;
    }
    double getX() { return this->x; }
    double getY() { return this->y; }
    void add(Vetor v)
    {
        this->x += v.getX();
        this->y += v.getY();
    }
    void multiply(double n) { this->x *=n; this->y *=n;}
    void set(Vetor Inicial, Vetor Final)
    {
        this->x = Final.getX()-Inicial.getX();
        this->y = Final.getY()-Inicial.getY();
    }
    void imprime()
    {
        printf("%f, %f", x,y);
    }
};

int main()
{
    Vetor V1, V2;
    Vetor A, B, P1;
    A.set(0,3);
    B.set(5,1);
    P1.set(2.5,0);
    V1.set(A, B);
    V1.imprime();
    printf("\n");
    return 0;
}

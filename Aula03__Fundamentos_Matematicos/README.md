# Aula 03 - Fundamentos Matemáticos para Computação Gráfica - 08/08/2023

### [Link para os slides](https://www.inf.pucrs.br/pinho/CG/SlidesEmPDF/FundamentosMatematicos.pdf)

## Conceitos Básicos

**Ponto**
As **coordenadas** de um ponto identificam uma **posição individual** no plano ou no espaço

No caso 2D, existe um eixo horizontal é chamado de **Eixo X** e um vertical, de 

A intersecção dos eixos define a **origem** do sistema

As **coordenadas** informam as distâncias entre o ponto e a origem do sistema, medidas ao longo de cada um dos eixos

**Vetor**

Define uma direção

Possue um tamanho (módulo)

Não tem origem

$V = (x, y, z)$

Operações

- Módulo
  - $|V| = \sqrt{x^2 + y^2 + z^2}$
- Versor (vetor unitário)
  - $V = {V \over |V|}$
- Multiplicação por um escalar
  - $V(x, y, z)*n = (x*n, y*n, z*n)$
- A **soma** de um **vetor** a um **ponto** permite o deslocamento do ponto na direção do vetor
  - $B = A + DIR$
- O tamanho do vetor fornece a velocidade de movimento
  - $B = A + DIR*veloc$
- A rotação permite a mudança de direção em um deslocamento
  - $xr = x * cos(\alpha) - y * sen(\alpha)$
  - $yr = x * sen(\alpha) + y * cos(\alpha)$
- Produto escalar
  - Tem este nome pois retorna um número
  - AKA: Produto interno, *dot product*, *inner product*
  - $V1 * V2 = x1 * x2 + y1 * y2 + z1 * z2$
  - Pode ser usado para calcular o ângulo entre 2 vetores pois, se V1 e V2 são unitários, então 
  - $V1 * V2 = cos(\alpha)$
- Produto Vetorial $V1 \times V2$
  - AKA: Produto externo, *cross product*, *outer product*
  - Regra da mão direita
    - Indicador: $V1$
    - Médio: $V2$
    - Polegar: $V1 \times V2$
  - Vetor perpendicular a $V1$ e $V2$
    - $V1 \times V2 = \det \begin{bmatrix} Xn & Yn & Zn \\ X1 & Y1 & Z1 \\ X2 & Y2 & Z2 \end{bmatrix}$
    - $Xn = Y1 * Z2 - Z1 * Y2$
    - $Yn = Z1 * X2 - X1 * Z2$
    - $Zn = X1 * Y2 - Y1 * X2$

**Retas**

Ponto inicial + Vetor iniciado no ponto

Utiliza-se a multiplicação do vetor por um escalar para obter qualquer ponto da reta
  - $R(t) = A + (B - A) * t$
  - $R(t) = A + B * t - A * t$
  - $R(t) = A * (1 - t) + B * t$ $t \in [0..1]$

**Polígonos**

<u>Sequência</u> de vértices



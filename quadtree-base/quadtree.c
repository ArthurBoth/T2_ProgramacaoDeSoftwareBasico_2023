#include "quadtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>     /* OpenGL functions */
#endif

unsigned int first = 1;
char desenhaBorda = 1;

QuadNode* newNode(int x, int y, int width, int height)
{
    QuadNode* n = malloc(sizeof(QuadNode));
    n->x = x;
    n->y = y;
    n->width = width;
    n->height = height;
    n->NW = n->NE = n->SW = n->SE = NULL;
    n->color[0] = n->color[1] = n->color[2] = 0;
    n->id = first++;
    return n;
}

QuadNode* geraQuadtree(Img* pic, float minError)
{
    // Converte o vetor RGBPixel para uma MATRIZ que pode acessada por pixels[linha][coluna]
    RGBPixel (*pixels)[pic->width] = (RGBPixel(*)[pic->height]) pic->img;

    /* Veja como acessar os primeiros 10 pixels da imagem, por exemplo:
    int i;
    for(i=0; i<10; i++){
        printf("%02X %02X %02X\n",pixels[0][i].r,pixels[1][i].g,pixels[2][i].b);
    }
    */
    int width = pic->width;
    int height = pic->height;

    //////////////////////////////////////////////////////////////////////////
    QuadNode* raiz = newQuadtree(pic,minError,0,0,height,width);
    //////////////////////////////////////////////////////////////////////////

// COMENTE a linha abaixo quando seu algoritmo ja estiver funcionando
// Caso contrario, ele ira gerar uma arvore de teste com 3 nodos

// #define DEMO
#ifdef DEMO

    /************************************************************/
    /* Teste: criando uma raiz e dois nodos a mais              */
    /************************************************************/

    QuadNode* raiz = newNode(0,0,width,height);
    raiz->status = PARCIAL;
    raiz->color[0] = 0;
    raiz->color[1] = 0;
    raiz->color[2] = 255;

    int meiaLargura = width/2;
    int meiaAltura = height/2;

    QuadNode* nw = newNode(meiaLargura, 0, meiaLargura, meiaAltura);
    nw->status = PARCIAL;
    nw->color[0] = 0;
    nw->color[1] = 0;
    nw->color[2] = 255;

    // Aponta da raiz para o nodo nw
    raiz->NW = nw;

    QuadNode* nw2 = newNode(meiaLargura+meiaLargura/2, 0, meiaLargura/2, meiaAltura/2);
    nw2->status = CHEIO;
    nw2->color[0] = 255;
    nw2->color[1] = 0;
    nw2->color[2] = 0;

    // Aponta do nodo nw para o nodo nw2
    nw->NW = nw2;

#endif
    // Finalmente, retorna a raiz da árvore
    return raiz;
}

// Limpa a memória ocupada pela árvore
void clearTree(QuadNode* n)
{
    if(n == NULL) return;
    if(n->status == PARCIAL)
    {
        clearTree(n->NE);
        clearTree(n->NW);
        clearTree(n->SE);
        clearTree(n->SW);
    }
    //printf("Liberando... %d - %.2f %.2f %.2f %.2f\n", n->status, n->x, n->y, n->width, n->height);
    free(n);
}

// Ativa/desativa o desenho das bordas de cada região
void toggleBorder() {
    desenhaBorda = !desenhaBorda;
    printf("Desenhando borda: %s\n", desenhaBorda ? "SIM" : "NÃO");
}

// Desenha toda a quadtree
void drawTree(QuadNode* raiz) {
    if(raiz != NULL)
        drawNode(raiz);
}

// Grava a árvore no formato do Graphviz
void writeTree(QuadNode* raiz) {
    FILE* fp = fopen("quad.dot", "w");
    fprintf(fp, "digraph quadtree {\n");
    if (raiz != NULL)
        writeNode(fp, raiz);
    fprintf(fp, "}\n");
    fclose(fp);
    printf("\nFim!\n");
}

void writeNode(FILE* fp, QuadNode* n)
{
    if(n == NULL) return;

    if(n->NE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NE->id);
    if(n->NW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->NW->id);
    if(n->SE != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SE->id);
    if(n->SW != NULL) fprintf(fp, "%d -> %d;\n", n->id, n->SW->id);
    writeNode(fp, n->NE);
    writeNode(fp, n->NW);
    writeNode(fp, n->SE);
    writeNode(fp, n->SW);
}

// Desenha todos os nodos da quadtree, recursivamente
void drawNode(QuadNode* n)
{
    if(n == NULL) return;

    glLineWidth(0.1);

    if(n->status == CHEIO) {
        glBegin(GL_QUADS);
        glColor3ubv(n->color);
        glVertex2f(n->x, n->y);
        glVertex2f(n->x+n->width-1, n->y);
        glVertex2f(n->x+n->width-1, n->y+n->height-1);
        glVertex2f(n->x, n->y+n->height-1);
        glEnd();
    }

    else if(n->status == PARCIAL)
    {
        if(desenhaBorda) {
            glBegin(GL_LINE_LOOP);
            glColor3ubv(n->color);
            glVertex2f(n->x, n->y);
            glVertex2f(n->x+n->width-1, n->y);
            glVertex2f(n->x+n->width-1, n->y+n->height-1);
            glVertex2f(n->x, n->y+n->height-1);
            glEnd();
        }
        drawNode(n->NE);
        drawNode(n->NW);
        drawNode(n->SE);
        drawNode(n->SW);
    }
    // Nodos vazios não precisam ser desenhados... nem armazenados!
}

unsigned char** grayscale(Img* pic,int x, int y,int height, int width)
{
	int i,j,aux;
	RGBPixel (*pixels)[pic->width] = (RGBPixel(*)[pic->height]) pic->img;
	
	unsigned char **gray = malloc(width * sizeof(unsigned char*));
	for (i=0;i<width;i++){
		gray[i] = malloc(height * sizeof(unsigned char));
	}
	
	for (i=y;i<(width+y);i++){
		for (j=x;j<(height+x);j++){
            aux = (0.3 * (pixels[j][i].r)) + (0.59 * (pixels[j][i].g)) + (0.11 * (pixels[j][i].b));
			gray[i-y][j-x] = aux;
		}
	}
	return gray;
}
unsigned char* avgColour(Img* pic,int x, int y,int height, int width)
{
	int i,j;
    int red = 0, green = 0, blue = 0;
    int size = height * width;
	RGBPixel (*pixels)[pic->width] = (RGBPixel(*)[pic->height]) pic->img;
    unsigned char *avg = malloc(3 * sizeof(unsigned char)); // 0 para RED, 1 para GREEN e 2 para BLUE
    
    for (i=y;i<(width+y);i++){
        for (j=x;j<(height+x);j++){
            red += pixels[i][j].r;
            green += pixels[i][j].g;
            blue += pixels[i][j].b;
        }
    }

    avg[0] = (unsigned char) (red/size);
    avg[1] = (unsigned char) (green/size);
    avg[2] = (unsigned char) (blue/size);
    return avg;
}

int* histogram (unsigned char** grayI,int x, int y,int height, int width)
{
    int h,i,j, aux;
    int *hist = malloc(256 * sizeof(int));

    for (i=0;i<256;i++){
        hist[i] = 0;
    }

    for (i=0;i<(width);i++){
        for (j=0;j<(height);j++){
            aux = grayI[i][j];
            hist[aux]++;
        }
    }
    return hist;
}

float calcError(Img* pic, int x, int y, int height, int width)
{
    unsigned char** gray = grayscale(pic, x, y, height, width);
    int* hist = histogram(gray, x, y, height, width);

    int i, j;
    double avgI = 0, sum = 0, size = (height * width);
    float error, aux = 1.0 / size;

    for (i = 0; i < 256; i++) {
        avgI += (i * hist[i]);
    }
    avgI = avgI / size;

    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            sum += (gray[i][j] - avgI) * (gray[i][j] - avgI);
        }
    }

    error = sqrt(aux * sum);

    // Libera a memória alocada para gray e hist
    for (i = 0; i < width; i++) {
        free(gray[i]);
    }
    free(gray);
    free(hist);

    return error;
}

QuadNode* newQuadtree (Img* pic,float minError,int x, int y,int height, int width)
{
    // onde 'x' é a coordenada da altura (height) e 'y' da largura (width)
    int h = height/2, w = width/2;
    float error = calcError(pic,x,y,height,width);
    unsigned char* avg = avgColour(pic,x,y,height,width);
    QuadNode* raiz = newNode(x,y,width,height);
    raiz->color[0] = avg[0];
    raiz->color[1] = avg[1];
    raiz->color[2] = avg[2];
    if (error < minError){ // condição de parada recursiva
        raiz->status = CHEIO;
    } else {
        raiz->status = PARCIAL;
        raiz->NW = newQuadtree(pic,minError,x,y,h,w);
        raiz->NE = newQuadtree(pic,minError,x,w,h,w);
        raiz->SE = newQuadtree(pic,minError,h,w,h,w);
        raiz->SW = newQuadtree(pic,minError,h,y,h,w);
    }
    free(avg);
    return raiz;
}

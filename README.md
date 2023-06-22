# T2_ProgramacaoDeSoftwareBasico
Para este trabalho fizemos cinco novos métodos.
Um para tornar a imagem em tons de cinza.
Um para calcular a cor média de uma região.
Um para calcular o histograma baseado na imagem em tons de cinza.
Um para calcular o nível de erro da região da imagem.
E um para criar a QuadTree, sendo esse recursivo.

Imediatamente declaramos que não fomos capazes de realizar um código que funcionasse em todos os casos. 
Isso se deve tanto por uma subvestimação da quantidade de tempo necessária para realizar este trabalho, como também uma falta de conhecimento em relação à estrutura QuadTree,
além de termos dificuldades com a compilação do código, que não está padronizado para rodar em sistemas Windows,
assim, também possuimos dificuldades de realizar o debugging do que conseguimos fazer. Acretitamos, porém, que fizemos um bom trabalho neste quesito mesmo assim.
Por essas razões não somos capazes de dizer o motivo do código estar agindo da maneira que está.

Segue abaixo uma cópia do trecho de código que produzimos, comentada linha-a-linha:


// torna a imagem em escala de cinza
unsigned char** grayscale(Img* pic,int x, int y,int height, int width)
{
	int i,j,aux; // variáveis de apoio
	RGBPixel (*pixels)[pic->width] = (RGBPixel(*)[pic->height]) pic->img; // para acessar os pixeis da imagem como se fosse uma matriz
	
	unsigned char **gray = malloc(width * sizeof(unsigned char*));
	for (i=0;i<width;i++){
		gray[i] = malloc(height * sizeof(unsigned char));
	} // gera uma matriz com os valores em escala de cinza
	
	for (i=y;i<(width+y);i++){ // começando no pixel y, e perconrrendo "width" vezes
		for (j=x;j<(height+x);j++){// começando no pixel x, e perconrrendo "height" vezes
            aux = (0.3 * (pixels[j][i].r)) + (0.59 * (pixels[j][i].g)) + (0.11 * (pixels[j][i].b)); // formula disponibilizada para converter para tons de cinza
			gray[i-y][j-x] = aux; // deve-se subtrair 'y' e 'x' de 'i' e 'j' para acessar a matriz a partir da posição [0][0]
		}
	}
	return gray;
}

// calcula a cor média da imagem
unsigned char* avgColour(Img* pic,int x, int y,int height, int width)
{
	int i,j; // variáveis de apoio
    int red = 0, green = 0, blue = 0; // somatórios dos valores vermelho, verde e azul de cada pixel da região
    int size = height * width; // quantidade total de pixeis na região
	RGBPixel (*pixels)[pic->width] = (RGBPixel(*)[pic->height]) pic->img; // para acessar os pixeis da imagem como se fosse uma matriz
    unsigned char *avg = malloc(3 * sizeof(unsigned char)); // guarda as médias de cor para vermelho, verde e azul
    // 0 para RED, 1 para GREEN e 2 para BLUE
    
    for (i=y;i<(width+y);i++){ // começando no pixel y, e perconrrendo "width" vezes
        for (j=x;j<(height+x);j++){ // começando no pixel x, e perconrrendo "height" vezes
            red += pixels[i][j].r; // adiciona o valor vermelho
            green += pixels[i][j].g; // adiciona o valor verde
            blue += pixels[i][j].b; // adiciona o valor azul
        }
    }

    avg[0] = (unsigned char) (red/size); // divide pelo tamanho para conseguir a média
    avg[1] = (unsigned char) (green/size); // divide pelo tamanho para conseguir a média
    avg[2] = (unsigned char) (blue/size); // divide pelo tamanho para conseguir a média
    return avg;
}

// calcula o histograma com base em uma matriz de tons de cinza
int* histogram (unsigned char** grayI,int x, int y,int height, int width)
{
    int h,i,j, aux; // variáveis de apoio
    int *hist = malloc(256 * sizeof(int)); // cria o histograma

    for (i=0;i<256;i++){
        hist[i] = 0; // inicializa o histograma com 0 em todas as
    }

    for (i=0;i<(width);i++){
        for (j=0;j<(height);j++){
            aux = grayI[i][j];
            hist[aux]++; // soma um ao contador de intensidade
        }
    }
    return hist;
}

// calcula o erro da região
float calcError(Img* pic, int x, int y, int height, int width)
{
    unsigned char** gray = grayscale(pic, x, y, height, width); // chama o método grayscale para calcular a escala de cinza
    int* hist = histogram(gray, x, y, height, width); // chama o método histogram para calcular o histograma

    int i, j; // variáveis de apoio
    double avgI = 0, sum = 0, size = (height * width); // repectivamente, a média de intensidade do histograma, o somatório dos quadrados das diferenças entre cada valor e a média, e o número de pixeis existentes na região
    float error, aux = 1.0 / size; // respectivamente, o erro que será calculado e retornado e a divisão de um pelo tamanho da região

    for (i = 0; i < 256; i++) {
        avgI += (i * hist[i]); // soma todos os valores do histograma, multiplicados pela suas frequências
    }
    avgI = avgI / size; // divide a soma pelo número de elementos para se obter a média

    for (i = 0; i < width; i++) {
        for (j = 0; j < height; j++) {
            sum += (gray[i][j] - avgI) * (gray[i][j] - avgI); // calcula a variância de intensidade dos pixeis da região
        }
    }

    error = sqrt(aux * sum); // calcula o desvio padrão a partir da variância

    // Libera a memória alocada para gray e hist
    for (i = 0; i < width; i++) {
        free(gray[i]);
    }
    free(gray);
    free(hist);

    return error;
}

// método recursivo que gera a Quadtree e retorna a sua raiz
QuadNode* newQuadtree (Img* pic,float minError,int x, int y,int height, int width){
    // onde 'x' é a coordenada da altura (height) e 'y' da largura (width), height é a altura da região, e width é a largura da região
    int h = height/2, w = width/2; // calcula a altura e largura dos filhos da raiz
    float error = calcError(pic,x,y,height,width); // calcula o erro da região
    unsigned char* avg = avgColour(pic,x,y,heiht,width); // calcula a cor média da região
    QuadNode* raiz = newNode(x,y,width,height); // cria um nodo para representar a raiz desta Sub-QuadTree
    raiz->color[0] = avg[0]; // salva os vermelhos na raiz
    raiz->color[1] = avg[1]; // salva os verdes na raiz
    raiz->color[2] = avg[2]; // salva os azuis na raiz
    if (error < minError){ // condição de parada recursiva
        raiz->status = CHEIO;
    } else {
        // se a condição de parada não for atingida...
        raiz->status = PARCIAL; // como ela terá filhos, seu status é parcial
        raiz->NW = newQuadtree(pic,minError,x,y,h,w); // região superior esquerda
        raiz->NE = newQuadtree(pic,minError,x,w,h,w); // região superior direita
        raiz->SE = newQuadtree(pic,minError,h,w,h,w); // região inferior direita
        raiz->SW = newQuadtree(pic,minError,h,y,h,w); // região inferior esquerda
    }
    free(avg); // Libera a memória alocada avg
    return raiz; // retorna a raiz
}
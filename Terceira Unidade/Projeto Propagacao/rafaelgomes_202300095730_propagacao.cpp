#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>

// Estrutura do nó para a abordagem híbrida de conjuntos disjuntos.
// Mescla a estrutura de árvore (ponteiro para o pai) com a de lista
// encadeada (para organizar os filhos de um mesmo nó).
struct No {
    // Ponteiros para a estrutura de árvore e lista
    No* pai;
    No* primeiroFilho; // Ponteiro para o primeiro filho na lista de filhos
    No* proximoIrmao;  // Ponteiro para o próximo irmão na lista de filhos

    // Heurística de união por tamanho
    int tamanho;

    // Coordenadas da pessoa na grade
    int x, y;
};

// Função pseudoaleatória myrand(), conforme especificado nos slides.
uint32_t myrand() {
    static uint32_t next = 1;
    next = next * 1103515245 + 12345;
    return next;
}

// Função g(z) para calcular o deslocamento, conforme especificado.
int g(int z) {
    return (z + (-1 + (myrand() % 3)));
}

// --- Implementação de Conjuntos Disjuntos com Estrutura Híbrida ---

// criar_conjunto: Inicializa um nó como um novo conjunto.
// É uma raiz sem filhos ou irmãos, com tamanho 1.
void criar_conjunto(No* x, int coord_x, int coord_y) {
    x->pai = x;
    x->primeiroFilho = nullptr;
    x->proximoIrmao = nullptr;
    x->tamanho = 1;
    x->x = coord_x;
    x->y = coord_y;
}

// encontrar_conjunto: Idêntico à versão com árvores puras.
// Retorna a raiz do conjunto e aplica COMPRESSÃO DE CAMINHOS.
No* encontrar_conjunto(No* x) {
    if (x->pai != x) {
        x->pai = encontrar_conjunto(x->pai);
    }
    return x->pai;
}

// unir_conjuntos: Une dois conjuntos usando UNIÃO POR TAMANHO.
// A raiz da árvore menor é adicionada à lista de filhos da raiz da árvore maior.
void unir_conjuntos(No* x, No* y) {
    No* raizX = encontrar_conjunto(x);
    No* raizY = encontrar_conjunto(y);

    if (raizX == raizY) {
        return; // Já estão no mesmo conjunto.
    }

    // Determina a maior e a menor árvore com base no tamanho.
    No* maior = (raizX->tamanho >= raizY->tamanho) ? raizX : raizY;
    No* menor = (raizX->tamanho < raizY->tamanho) ? raizX : raizY;

    // 1. A raiz da árvore menor agora aponta para a raiz da maior.
    menor->pai = maior;
    
    // 2. Insere a raiz menor na lista de filhos da raiz maior.
    menor->proximoIrmao = maior->primeiroFilho;
    maior->primeiroFilho = menor;
    
    // 3. Atualiza o tamanho da nova árvore unificada.
    maior->tamanho += menor->tamanho;
}

// --- Função Principal ---

int main(int argc, char *argv[]) {
    // Otimização para acelerar a leitura e escrita de dados
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    std::ofstream outputFile(argv[2]);

    int num_regioes;
    inputFile >> num_regioes;

    for (int r = 1; r <= num_regioes; ++r) {
        int altura, largura, x0, y0;
        inputFile >> altura >> largura >> x0 >> y0;

        int total_pessoas = altura * largura;
        
        // CORREÇÃO 1: Aloca um array contíguo para os OBJETOS No.
        No* pessoas = new No[total_pessoas];
        
        // CORREÇÃO 2: A grid é declarada como uma matriz de OBJETOS (No**).
        No** grid = new No*[altura];
        for (int i = 0; i < altura; ++i) {
            grid[i] = new No[largura];
        }

        // CORREÇÃO 3: Preenche a grid com os OBJETOS.
        for (int i = 0; i < altura; ++i) {
            for (int j = 0; j < largura; ++j) {
                int index = i * largura + j;
                // Copia o objeto do array pessoas para a grid.
                grid[i][j] = pessoas[index];
                // Inicializa o objeto na grid.
                criar_conjunto(&grid[i][j], i, j);
            }
        }
        
        // CORREÇÃO 4: Usa o endereço do objeto na grid.
        No* paciente_zero = &grid[x0][y0];
        No* rep_infectados = encontrar_conjunto(paciente_zero);
        int num_conjuntos = total_pessoas;

        No** ordem_infeccao = new No*[total_pessoas];
        ordem_infeccao[0] = paciente_zero;
        int infectados_count = 1;

        while (num_conjuntos > 1) {
            int propagador_x = rep_infectados->x;
            int propagador_y = rep_infectados->y;
            
            No* vitima;
            No* rep_vitima;

            do {
                int novo_x, novo_y;
                
                do {
                    novo_x = g(propagador_x);
                } while (novo_x < 0 || novo_x >= altura);

                do {
                    novo_y = g(propagador_y);
                } while (novo_y < 0 || novo_y >= largura);
                
                // CORREÇÃO 5: A atribuição para 'vitima' (No*) funciona.
                vitima = &grid[novo_x][novo_y];
                rep_vitima = encontrar_conjunto(vitima);

            } while (rep_vitima == rep_infectados);

            unir_conjuntos(rep_infectados, vitima);
            num_conjuntos--;

            rep_infectados = encontrar_conjunto(rep_infectados);
            
            ordem_infeccao[infectados_count] = vitima;
            infectados_count++;
        }

        outputFile << r << ":";
        for (int i = 0; i < infectados_count; ++i) {
            outputFile << "(" << ordem_infeccao[i]->x << "," << ordem_infeccao[i]->y << ");";
        }
        outputFile << std::endl;

        delete[] pessoas;
        for (int i = 0; i < altura; ++i) {
            delete[] grid[i];
        }
        delete[] grid;
        delete[] ordem_infeccao;
    }

    inputFile.close();
    outputFile.close();

    return 0;
}


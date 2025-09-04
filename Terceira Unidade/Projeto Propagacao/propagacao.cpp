#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <vector> // Usando std::vector para gerenciamento de memória otimizado e seguro.

// A estrutura de nó mais eficiente: árvore pura.
struct No {
    No* pai;
    int altura;   // Heurística de união por altura (rank).
    int x, y;
};

// Função pseudoaleatória myrand(), conforme especificado.
uint32_t myrand() {
    static uint32_t next = 1;
    next = next * 1103515245 + 12345;
    return next;
}

// Função g(z) para calcular o deslocamento, conforme especificado.
int g(int z) {
    return (z + (-1 + (myrand() % 3)));
}

// --- Implementação de Conjuntos Disjuntos com Árvores Otimizadas ---

// criar_conjunto: Inicializa um nó como uma nova árvore (um conjunto).
void criar_conjunto(No* x, int coord_x, int coord_y) {
    x->pai = x;
    x->altura = 0;
    x->x = coord_x;
    x->y = coord_y;
}

// encontrar_conjunto: Retorna a raiz do conjunto aplicando COMPRESSÃO DE CAMINHOS.
No* encontrar_conjunto(No* x) {
    if (x->pai != x) {
        x->pai = encontrar_conjunto(x->pai);
    }
    return x->pai;
}

// unir_conjuntos: Une dois conjuntos usando UNIÃO POR ALTURA.
void unir_conjuntos(No* x, No* y) {
    No* raizX = encontrar_conjunto(x);
    No* raizY = encontrar_conjunto(y);

    if (raizX == raizY) {
        return;
    }

    if (raizX->altura > raizY->altura) {
        raizY->pai = raizX;
    } else {
        raizX->pai = raizY;
        if (raizX->altura == raizY->altura) {
            raizY->altura++;
        }
    }
}

// --- Função Principal ---

int main(int argc, char *argv[]) {
    // Otimização de I/O padrão em C++.
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
        
        // OTIMIZAÇÃO: Usa std::vector para gerenciar a memória dos objetos No.
        std::vector<No> pessoas(total_pessoas);
        
        // OTIMIZAÇÃO: A grid agora é um std::vector de vetores de ponteiros.
        std::vector<std::vector<No*>> grid(altura, std::vector<No*>(largura));

        // Inicializa a grade.
        for (int i = 0; i < altura; ++i) {
            for (int j = 0; j < largura; ++j) {
                int index = i * largura + j;
                grid[i][j] = &pessoas[index];
                criar_conjunto(grid[i][j], i, j);
            }
        }
        
        No* paciente_zero = grid[x0][y0];
        No* rep_infectados = encontrar_conjunto(paciente_zero);
        int num_conjuntos = total_pessoas;

        // OTIMIZAÇÃO: Usa std::vector para a ordem de infecção.
        std::vector<No*> ordem_infeccao;
        ordem_infeccao.reserve(total_pessoas); // Pré-aloca memória para evitar realocações.
        ordem_infeccao.push_back(paciente_zero);

        // Loop principal da simulação (lógica inalterada).
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
                
                vitima = grid[novo_x][novo_y];
                rep_vitima = encontrar_conjunto(vitima);

            } while (rep_vitima == rep_infectados);

            unir_conjuntos(rep_infectados, vitima);
            num_conjuntos--;

            rep_infectados = encontrar_conjunto(rep_infectados);
            
            ordem_infeccao.push_back(vitima);
        }

        // Escreve a saída no formato especificado.
        outputFile << r << ":";
        for (const auto& infectado : ordem_infeccao) { // Usando range-based for.
            outputFile << "(" << infectado->x << "," << infectado->y << ");";
        }
        outputFile << std::endl;
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
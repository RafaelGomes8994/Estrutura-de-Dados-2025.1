#include <iostream>
#include <fstream>
#include <cstring> // Para strcpy

// Estrutura para armazenar os dados relevantes de uma aposta.
// Guardamos apenas o código e os acertos para otimizar a memória.
struct Aposta {
    char codigo[33]; // 32 caracteres hex + 1 para o terminador nulo '\0'
    int acertos;
};

// --- Funções Utilitárias do Heap ---
// Conforme os slides (p. 4-6, ed_15_arvore_heap.pdf)

// Retorna o índice do pai de um nó i
int pai(int i) {
    return (i - 1) / 2;
}

// Retorna o índice do filho esquerdo de um nó i
int esquerdo(int i) {
    return 2 * i + 1;
}

// Retorna o índice do filho direito de um nó i
int direito(int i) {
    return 2 * i + 2;
}

// Troca duas apostas de lugar
void trocar(Aposta& a, Aposta& b) {
    Aposta temp = a;
    a = b;
    b = temp;
}

// --- Funções do Heap Máximo ---
// Lógica do heapify (p. 9, ed_15_arvore_heap.pdf) adaptada para heap máximo.
void heapify_max(Aposta* vetor, int tamanho, int i) {
    int maior = i;
    int e = esquerdo(i);
    int d = direito(i);

    // Se o filho esquerdo for maior que a raiz
    if (e < tamanho && vetor[e].acertos > vetor[maior].acertos)
        maior = e;

    // Se o filho direito for maior que o maior até agora
    if (d < tamanho && vetor[d].acertos > vetor[maior].acertos)
        maior = d;

    // Se o maior não for a raiz, troca e continua o heapify recursivamente
    if (maior != i) {
        trocar(vetor[i], vetor[maior]);
        heapify_max(vetor, tamanho, maior);
    }
}

// Constrói um heap máximo a partir de um vetor (p. 16, ed_15_arvore_heap.pdf)
void construir_heap_max(Aposta* vetor, int tamanho) {
    for (int i = (tamanho / 2) - 1; i >= 0; i--) {
        heapify_max(vetor, tamanho, i);
    }
}

// Extrai o elemento máximo (raiz) do heap
Aposta extrair_max(Aposta* vetor, int& tamanho) {
    Aposta raiz = vetor[0];
    vetor[0] = vetor[tamanho - 1];
    tamanho--;
    heapify_max(vetor, tamanho, 0);
    return raiz;
}


// --- Funções do Heap Mínimo ---
// Lógica do heapify adaptada para heap mínimo (comparação invertida).
void heapify_min(Aposta* vetor, int tamanho, int i) {
    int menor = i;
    int e = esquerdo(i);
    int d = direito(i);

    if (e < tamanho && vetor[e].acertos < vetor[menor].acertos)
        menor = e;

    if (d < tamanho && vetor[d].acertos < vetor[menor].acertos)
        menor = d;

    if (menor != i) {
        trocar(vetor[i], vetor[menor]);
        heapify_min(vetor, tamanho, menor);
    }
}

// Constrói um heap mínimo a partir de um vetor
void construir_heap_min(Aposta* vetor, int tamanho) {
    for (int i = (tamanho / 2) - 1; i >= 0; i--) {
        heapify_min(vetor, tamanho, i);
    }
}

// Extrai o elemento mínimo (raiz) do heap
Aposta extrair_min(Aposta* vetor, int& tamanho) {
    Aposta raiz = vetor[0];
    vetor[0] = vetor[tamanho - 1];
    tamanho--;
    heapify_min(vetor, tamanho, 0);
    return raiz;
}


// --- Função Principal ---
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: ./programa <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    std::ofstream outputFile(argv[2]);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Erro ao abrir os arquivos." << std::endl;
        return 1;
    }

    // 1. Leitura dos dados iniciais
    double premio_total;
    int num_apostas;
    bool numeros_sorteados[51] = {false}; // Vetor para busca rápida O(1)

    inputFile >> premio_total >> num_apostas;
    for (int i = 0; i < 10; ++i) {
        int num;
        inputFile >> num;
        if (num >= 1 && num <= 50) {
            numeros_sorteados[num] = true;
        }
    }

    // 2. Leitura e processamento das apostas
    Aposta* todas_apostas = new Aposta[num_apostas];
    for (int i = 0; i < num_apostas; ++i) {
        inputFile >> todas_apostas[i].codigo;
        int acertos_count = 0;
        for (int j = 0; j < 15; ++j) {
            int num_apostado;
            inputFile >> num_apostado;
            if (num_apostado >= 1 && num_apostado <= 50 && numeros_sorteados[num_apostado]) {
                acertos_count++;
            }
        }
        todas_apostas[i].acertos = acertos_count;
    }
    inputFile.close();
    
    if (num_apostas == 0) {
        outputFile.close();
        delete[] todas_apostas;
        return 0;
    }

    // 3. Criação dos Heaps
    Aposta* max_heap = new Aposta[num_apostas];
    Aposta* min_heap = new Aposta[num_apostas];
    for (int i = 0; i < num_apostas; ++i) {
        max_heap[i] = todas_apostas[i];
        min_heap[i] = todas_apostas[i];
    }

    construir_heap_max(max_heap, num_apostas);
    construir_heap_min(min_heap, num_apostas);

    // 4. Extração dos Ganhadores
    int max_acertos = max_heap[0].acertos;
    int min_acertos = min_heap[0].acertos;

    Aposta* ganhadores_max = new Aposta[num_apostas];
    int count_max = 0;
    int heap_max_size = num_apostas;
    while (heap_max_size > 0 && max_heap[0].acertos == max_acertos) {
        ganhadores_max[count_max++] = extrair_max(max_heap, heap_max_size);
    }

    Aposta* ganhadores_min = new Aposta[num_apostas];
    int count_min = 0;
    int heap_min_size = num_apostas;
    while (heap_min_size > 0 && min_heap[0].acertos == min_acertos) {
        ganhadores_min[count_min++] = extrair_min(min_heap, heap_min_size);
    }

    // 5. Escrita dos resultados no arquivo de saída
    if (min_acertos == max_acertos) {
        // Se o min e o max são iguais, todos dividem o prêmio total
        double premio_individual = premio_total / count_max;
        outputFile << "[" << count_max << ":" << max_acertos << ":" << (int)premio_individual << "]\n";
        for (int i = 0; i < count_max; ++i) {
            outputFile << ganhadores_max[i].codigo << "\n";
        }
    } else {
        // Faixa de maior número de acertos
        double premio_individual_max = (premio_total / 2.0) / count_max;
        outputFile << "[" << count_max << ":" << max_acertos << ":" << (int)premio_individual_max << "]\n";
        for (int i = 0; i < count_max; ++i) {
            outputFile << ganhadores_max[i].codigo << "\n";
        }

        // Faixa de menor número de acertos
        double premio_individual_min = (premio_total / 2.0) / count_min;
        outputFile << "[" << count_min << ":" << min_acertos << ":" << (int)premio_individual_min << "]\n";
        for (int i = 0; i < count_min; ++i) {
            outputFile << ganhadores_min[i].codigo << "\n";
        }
    }
    outputFile.close();

    // 6. Liberação da memória alocada
    delete[] todas_apostas;
    delete[] max_heap;
    delete[] min_heap;
    delete[] ganhadores_max;
    delete[] ganhadores_min;

    return 0;
}
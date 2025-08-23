#include <cstdio> // Para I/O mais rápido (fscanf, fprintf)
#include <stdint.h>
#include <iostream> // Apenas para std::cerr em caso de erro

// Função myrand() (sem alterações)
uint32_t myrand() {
    static uint32_t next = 1;
    next = next * 1103515245 + 12345;
    return next;
}

// Estrutura do Nó (sem alterações)
struct Node {
    Node* P;
    uint32_t H;
    int x, y;
};

// --- Funções Union-Find com a versão iterativa (já otimizada) ---
void make_set(Node* node, int x, int y) {
    node->P = node;
    node->H = 0;
    node->x = x;
    node->y = y;
}

Node* find_set_iterative(Node* node) {
    Node* root = node;
    while (root != root->P) {
        root = root->P;
    }
    Node* current = node;
    while (current != root) {
        Node* next = current->P;
        current->P = root;
        current = next;
    }
    return root;
}

void union_sets(Node* x_node, Node* y_node) {
    Node* root_x = find_set_iterative(x_node);
    Node* root_y = find_set_iterative(y_node);
    if (root_x == root_y) return;
    if (root_x->H > root_y->H) {
        root_y->P = root_x;
    } else {
        root_x->P = root_y;
        if (root_x->H == root_y->H) {
            root_y->H++;
        }
    }
}

// *** FUNÇÃO DE SIMULAÇÃO FINAL ***
void run_simulation_final(int region_id, int height, int width, int x0, int y0, FILE* outputFile) {
    int total_people = height * width;
    
    // --- OTIMIZAÇÃO DE MEMÓRIA (ARENA) ---
    // 1. Calcular o tamanho total de memória necessário
    size_t people_bytes = total_people * sizeof(Node);
    size_t infected_bytes = total_people * sizeof(bool);
    size_t order_bytes = total_people * sizeof(Node*);
    // 2. Alocar um único bloco gigante
    char* buffer = new char[people_bytes + infected_bytes + order_bytes];
    // 3. Apontar nossos ponteiros para as seções corretas do bloco
    Node* people = (Node*)buffer;
    bool* infected = (bool*)(buffer + people_bytes);
    Node** infection_order = (Node**)(buffer + people_bytes + infected_bytes);
    
    // Inicializa o array 'infected' com zeros (false)
    for(int i = 0; i < total_people; ++i) infected[i] = false;

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int index = r * width + c;
            make_set(&people[index], c, r);
        }
    }

    int current_x = x0;
    int current_y = y0;
    int current_idx = current_y * width + current_x;
    infected[current_idx] = true;
    
    infection_order[0] = &people[current_idx];
    int infected_count = 1;

    while (infected_count < total_people) {
        int next_x, next_y;
        
        do {
            int move_x = -1 + (myrand() % 3);
            int move_y = -1 + (myrand() % 3);
            next_x = current_x + move_x;
            next_y = current_y + move_y;
        } while (next_x < 0 || next_x >= width || next_y < 0 || next_y >= height || infected[next_y * width + next_x]);

        int victim_idx = next_y * width + next_x;
        infected[victim_idx] = true;
        
        infection_order[infected_count] = &people[victim_idx];
        infected_count++;

        union_sets(&people[current_idx], &people[victim_idx]);

        current_x = next_x;
        current_y = next_y;
        current_idx = victim_idx;
    }

    // --- OTIMIZAÇÃO DE I/O ---
    fprintf(outputFile, "%d:", region_id);
    for (int j = 0; j < total_people; ++j) {
        fprintf(outputFile, "(%d,%d)", infection_order[j]->x, infection_order[j]->y);
        if (j < total_people - 1) {
            fprintf(outputFile, ";");
        }
    }
    fprintf(outputFile, "\n");

    // Apenas um delete para todo o bloco de memória
    delete[] buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: ./executavel <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    // --- OTIMIZAÇÃO DE I/O: Usando <cstdio> ---
    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) {
        std::cerr << "Erro ao abrir o arquivo de entrada: " << argv[1] << std::endl;
        return 1;
    }

    FILE* outputFile = fopen(argv[2], "w");
    if (!outputFile) {
        std::cerr << "Erro ao abrir o arquivo de saida: " << argv[2] << std::endl;
        fclose(inputFile);
        return 1;
    }

    int numRegions;
    fscanf(inputFile, "%d", &numRegions);

    fprintf(outputFile, "Propagação de doenças entre as pessoas\n");

    for (int i = 1; i <= numRegions; ++i) {
        int height, width, x0, y0;
        fscanf(inputFile, "%d %d %d %d", &height, &width, &x0, &y0);
        run_simulation_final(i, height, width, x0, y0, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
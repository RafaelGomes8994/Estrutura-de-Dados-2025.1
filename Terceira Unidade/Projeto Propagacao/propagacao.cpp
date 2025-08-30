#include <cstdio> // Para I/O mais rápido (fscanf, fprintf)
#include <stdint.h>
#include <iostream> // Apenas para std::cerr em caso de erro

// Função myrand() (sem alterações)
uint32_t myrand() {
    static uint32_t next = 1;
    next = next * 1103515245 + 12345;
    return next;
}

// ADICIONADO: A função g(z) como mostrado na imagem do professor
int g(int z) {
    return (z + (-1 + (myrand() % 3)));
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

// *** FUNÇÃO DE SIMULAÇÃO COM A LÓGICA CORRETA DO PROFESSOR ***
void run_simulation_final(int region_id, int height, int width, int x0, int y0, FILE* outputFile) {
    int total_people = height * width;
    if (total_people == 0) {
        fprintf(outputFile, "%d:\n", region_id);
        return;
    }
    
    // --- Otimizações de memória e I/O mantidas ---
    size_t people_bytes = total_people * sizeof(Node);
    size_t infected_bytes = total_people * sizeof(bool);
    size_t order_bytes = total_people * sizeof(Node*);
    char* buffer = new char[people_bytes + infected_bytes + order_bytes];
    Node* people = (Node*)buffer;
    bool* infected = (bool*)(buffer + people_bytes);
    Node** infection_order = (Node**)(buffer + people_bytes + infected_bytes);
    
    for(int i = 0; i < total_people; ++i) infected[i] = false;

    for (int r = 0; r < height; ++r) {
        for (int c = 0; c < width; ++c) {
            int index = r * width + c;
            make_set(&people[index], c, r);
        }
    }

    // Ponto de partida: Paciente Zero
    int p0_idx = y0 * width + x0;
    infected[p0_idx] = true;
    infection_order[0] = &people[p0_idx];
    int infected_count = 1;
    
    // MUDANÇA: Criamos um ponteiro para qualquer nó do conjunto dos infectados.
    // Ele servirá como ponto de entrada para encontrar o representante.
    Node* any_node_in_infected_set = &people[p0_idx];

    while (infected_count < total_people) {
        // --- INÍCIO DA LÓGICA CORRIGIDA ---

        // 1. O espalhador é SEMPRE o representante do conjunto dos infectados.
        Node* spreader = find_set_iterative(any_node_in_infected_set);
        
        int next_x, next_y;
        
        // 2. A busca pela vítima parte das coordenadas do REPRESENTANTE.
        do {
            next_x = g(spreader->x);
            next_y = g(spreader->y);
        } while (next_x < 0 || next_x >= width || next_y < 0 || next_y >= height || infected[next_y * width + next_x]);

        int victim_idx = next_y * width + next_x;
        Node* victim = &people[victim_idx];

        infected[victim_idx] = true;
        infection_order[infected_count++] = victim;

        // 3. Unimos o conjunto do espalhador (representante) com o da vítima.
        // A união pode fazer com que o representante do conjunto mude para a próxima iteração.
        union_sets(spreader, victim);

        // 4. Atualizamos nosso "ponto de entrada" para o conjunto. 
        // A vítima agora pertence ao conjunto, então podemos usá-la.
        any_node_in_infected_set = victim;
        
        // --- FIM DA LÓGICA CORRIGIDA ---
    }

    fprintf(outputFile, "%d:", region_id);
    for (int j = 0; j < total_people; ++j) {
        fprintf(outputFile, "(%d,%d)", infection_order[j]->x, infection_order[j]->y);
        if (j < total_people - 1) {
            fprintf(outputFile, ";");
        }
    }
    fprintf(outputFile, "\n");

    delete[] buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: ./executavel <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

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
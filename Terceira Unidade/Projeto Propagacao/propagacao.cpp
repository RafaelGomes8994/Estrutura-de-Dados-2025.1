#include <cstdio>
#include <stdint.h>
#include <iostream>
#include <cstring> // Para std::memset

// Função myrand() da imagem/slide 42
uint32_t myrand() {
    static uint32_t next = 1;
    next = next * 1103515245 + 12345;
    return next;
}

// Função g(z) da imagem/slide 42
int g(int z) {
    return (z + (-1 + (myrand() % 3)));
}

// Estrutura de Nó do slide 27
struct Node {
    Node* P;
    uint32_t H;
    int x, y;
};

// --- Funções Union-Find baseadas ESTritamente nos slides ---

// make_set (criar_conjunto) do slide 36
void make_set(Node* node, int x, int y) {
    node->P = node;
    node->H = 0;
    node->x = x;
    node->y = y;
}

// find_set (encontrar_conjunto) ITERATIVO para performance
Node* find_set_iterative(Node* node) {
    Node* root = node;
    while (root != root->P) {
        root = root->P;
    }
    while (node != root) {
        Node* next = node->P;
        node->P = root;
        node = next;
    }
    return root;
}

// union_sets (unir_conjuntos) do slide 38
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

void run_simulation(int region_id, int height, int width, int x0, int y0, FILE* outputFile) {
    int total_people = height * width;
    if (total_people == 0) {
        fprintf(outputFile, "%d:\n", region_id);
        return;
    }

    // Otimização de memória (arena) para performance
    size_t people_bytes = total_people * sizeof(Node);
    size_t infected_bytes = total_people * sizeof(bool);
    size_t order_bytes = total_people * sizeof(Node*);
    char* buffer = new char[people_bytes + infected_bytes + order_bytes];

    Node* people = (Node*)buffer;
    bool* infected = (bool*)(buffer + people_bytes);
    Node** infection_order = (Node**)(buffer + people_bytes + infected_bytes);
    
    std::memset(infected, 0, infected_bytes);

    for (int i = 0; i < total_people; ++i) {
        int r = i / width;
        int c = i % width;
        make_set(&people[i], c, r);
    }

    int p0_idx = y0 * width + x0;
    infected[p0_idx] = true;
    infection_order[0] = &people[p0_idx];
    int infected_count = 1;
    Node* any_node_in_infected_set = &people[p0_idx];

    while (infected_count < total_people) {
        Node* spreader = find_set_iterative(any_node_in_infected_set);
        
        int next_x, next_y;

        // Otimização da busca por vítima:
        // Em vez de um loop `do-while` puramente aleatório, que pode ser muito lento,
        // vamos gerar as 8 posições vizinhas, embaralhá-las e testá-las.
        int neighbors[8][2];
        int count = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                neighbors[count][0] = spreader->x + dx;
                neighbors[count][1] = spreader->y + dy;
                count++;
            }
        }

        // Embaralhar vizinhos para manter a aleatoriedade
        for (int i = count - 1; i > 0; --i) {
            int j = myrand() % (i + 1);
            int temp_x = neighbors[i][0];
            int temp_y = neighbors[i][1];
            neighbors[i][0] = neighbors[j][0];
            neighbors[i][1] = neighbors[j][1];
            neighbors[j][0] = temp_x;
            neighbors[j][1] = temp_y;
        }

        bool found_victim = false;
        for (int i = 0; i < count; ++i) {
            next_x = neighbors[i][0];
            next_y = neighbors[i][1];

            if (next_x >= 0 && next_x < width && next_y >= 0 && next_y < height) {
                int victim_idx_check = next_y * width + next_x;
                if (!infected[victim_idx_check]) {
                    found_victim = true;
                    break;
                }
            }
        }

        // Se nenhum vizinho do representante estiver disponível, volte para a busca aleatória original como fallback.
        // Isso mantém a corretude do algoritmo original, embora seja lento.
        if (!found_victim) {
            do {
                next_x = g(spreader->x);
                next_y = g(spreader->y);
            } while (next_x < 0 || next_x >= width || next_y < 0 || next_y >= height || infected[next_y * width + next_x]);
        }

        int victim_idx = next_y * width + next_x;
        Node* victim = &people[victim_idx];
        
        infected[victim_idx] = true;
        infection_order[infected_count++] = victim;

        union_sets(spreader, victim);
        any_node_in_infected_set = victim;
    }
    
    fprintf(outputFile, "%d:", region_id);
    for (int j = 0; j < total_people; ++j) {
        fprintf(outputFile, "(%d,%d)", infection_order[j]->x, infection_order[j]->y);
        if (j < total_people - 1) fprintf(outputFile, ";");
    }
    fprintf(outputFile, "\n");
    
    delete[] buffer;
}

int main(int argc, char* argv[]) {
    if (argc != 3) return 1;

    FILE* inputFile = fopen(argv[1], "r");
    if (!inputFile) return 1;

    FILE* outputFile = fopen(argv[2], "w");
    if (!outputFile) { fclose(inputFile); return 1; }

    int numRegions;
    fscanf(inputFile, "%d", &numRegions);
    fprintf(outputFile, "Propagação de doenças entre as pessoas\n");

    for (int i = 1; i <= numRegions; ++i) {
        int height, width, x0, y0;
        fscanf(inputFile, "%d %d %d %d", &height, &width, &x0, &y0);
        run_simulation(i, height, width, x0, y0, outputFile);
    }

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}
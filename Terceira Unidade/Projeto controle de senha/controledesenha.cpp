#include <stdio.h>
#include <string.h> // Para strcmp, strcpy, strlen

// --- ESTRUTURAS ---
struct Pessoa {
    char nome[51];
    int idade;
    int ordemChegada;
    int prioridade; // 1 para preferencial (>= 60), 0 para convencional
};

struct Heap {
    Pessoa* pessoas;
    int tamanho;
    int capacidade;
};

struct Orgao {
    char nome[51];
    int numAtendentes;
    Heap fila;
};

// --- FUNÇÕES DO HEAP (IMPLEMENTAÇÃO COMPLETA) ---
int pai(int i) { return (i - 1) / 2; }
int esquerdo(int i) { return 2 * i + 1; }
int direito(int i) { return 2 * i + 2; }

void trocar(Pessoa& a, Pessoa& b) {
    Pessoa temp = a;
    a = b;
    b = temp;
}

// Compara duas pessoas para o Max-Heap
bool temMaiorPrioridade(Pessoa p1, Pessoa p2) {
    if (p1.prioridade != p2.prioridade) {
        return p1.prioridade > p2.prioridade;
    }
    return p1.ordemChegada < p2.ordemChegada; // Desempate: quem chegou antes
}

// Função Heapify "para baixo" (usada na remoção)
void heapify_down(Heap& heap, int i) {
    int e = esquerdo(i);
    int d = direito(i);
    int maior = i;

    if (e < heap.tamanho && temMaiorPrioridade(heap.pessoas[e], heap.pessoas[maior])) {
        maior = e;
    }
    if (d < heap.tamanho && temMaiorPrioridade(heap.pessoas[d], heap.pessoas[maior])) {
        maior = d;
    }

    if (maior != i) {
        trocar(heap.pessoas[i], heap.pessoas[maior]);
        heapify_down(heap, maior);
    }
}

// Função Heapify "para cima" (usada na inserção)
void heapify_up(Heap& heap, int i) {
    while (i > 0 && temMaiorPrioridade(heap.pessoas[i], heap.pessoas[pai(i)])) {
        trocar(heap.pessoas[i], heap.pessoas[pai(i)]);
        i = pai(i);
    }
}

void inserirPessoa(Heap& heap, Pessoa p) {
    if (heap.tamanho == heap.capacidade) return;
    heap.tamanho++;
    heap.pessoas[heap.tamanho - 1] = p;
    heapify_up(heap, heap.tamanho - 1);
}

Pessoa removerPessoa(Heap& heap) {
    Pessoa raiz = heap.pessoas[0];
    heap.pessoas[0] = heap.pessoas[heap.tamanho - 1];
    heap.tamanho--;
    heapify_down(heap, 0);
    return raiz;
}

// --- FUNÇÃO PRINCIPAL ---
int main(int argc, char* argv[]) {
    if (argc > 2) {
        freopen(argv[1], "r", stdin);
        freopen(argv[2], "w", stdout);
    }

    int numOrgaos;
    scanf("%d", &numOrgaos);

    Orgao* orgaos = new Orgao[numOrgaos];

    for (int i = 0; i < numOrgaos; ++i) {
        scanf("%s %d", orgaos[i].nome, &orgaos[i].numAtendentes);
        orgaos[i].fila.capacidade = 200;
        orgaos[i].fila.tamanho = 0;
        orgaos[i].fila.pessoas = new Pessoa[orgaos[i].fila.capacidade];
    }

    int numPessoas;
    scanf("%d", &numPessoas);
    
    int ordemAtual = 0;
    for (int i = 0; i < numPessoas; ++i) {
        char nomeOrgao[51], nomePessoa[52];
        int idade;
        
        scanf("%s | %51[^|] | %d", nomeOrgao, nomePessoa, &idade);
        
        Pessoa p;
        if (nomePessoa[0] == ' ') {
            strcpy(p.nome, nomePessoa + 1);
        } else {
            strcpy(p.nome, nomePessoa);
        }
        
        p.idade = idade;
        p.ordemChegada = ordemAtual++;
        p.prioridade = (idade >= 60) ? 1 : 0;

        for (int j = 0; j < numOrgaos; ++j) {
            if (strcmp(orgaos[j].nome, nomeOrgao) == 0) {
                inserirPessoa(orgaos[j].fila, p);
                break;
            }
        }
    }

    bool continuarAtendimento = true;
    while(continuarAtendimento) {
        continuarAtendimento = false;
        
        for (int i = 0; i < numOrgaos; ++i) {
            bool imprimiuNomeOrgao = false;
            for(int j = 0; j < orgaos[i].numAtendentes; ++j) {
                if(orgaos[i].fila.tamanho > 0) {
                    if(!imprimiuNomeOrgao) {
                        printf("%s:", orgaos[i].nome);
                        imprimiuNomeOrgao = true;
                    } else {
                        printf(",");
                    }

                    Pessoa atendida = removerPessoa(orgaos[i].fila);
                    
                    for(size_t k = 0; k < strlen(atendida.nome); ++k){
                        if(atendida.nome[k] == ' ') atendida.nome[k] = '_';
                    }
                    printf(" %s", atendida.nome);
                    
                    continuarAtendimento = true;
                }
            }
            if(imprimiuNomeOrgao) {
                printf("\n");
            }
        }
    }

    for (int i = 0; i < numOrgaos; ++i) {
        delete[] orgaos[i].fila.pessoas;
    }
    delete[] orgaos;

    return 0;
}
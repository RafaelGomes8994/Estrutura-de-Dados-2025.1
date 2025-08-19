#include <cstdio>
#include <cstring>
#include <cstdlib>

#define MAX_LINE_SIZE 512

// --- ESTRUTURAS DE DADOS ---

// Estrutura para armazenar informações de um arquivo
struct ArquivoInfo {
    char* nome;
    long long tamanho;
    char* hash;

    // Construtor: copia os parâmetros para os campos
    ArquivoInfo(const char* n, long long s, const char* h) : tamanho(s) {
        nome = new char[strlen(n) + 1];
        strcpy(nome, n);
        hash = new char[strlen(h) + 1];
        strcpy(hash, h);
    }
    // Destrutor: libera memória alocada
    ~ArquivoInfo() {
        delete[] nome;
        delete[] hash;
    }
    // Imprime informações do arquivo
    void imprimir(FILE* out) const {
        fprintf(out, "%s:size=%lld,hash=%s", nome, tamanho, hash);
    }
};

// Nó da árvore B+
struct No {
    bool folha;      // Indica se é folha
    int n;           // Número de chaves
    int k;           // Ordem da árvore
    char** chaves;   // Vetor de chaves
    void** ptrs;     // Ponteiros para filhos ou dados
    No* prox;        // Ponteiro para próxima folha
    No* pai;         // Ponteiro para pai

    // Construtor: inicializa vetores e campos
    No(int ordem, bool ehFolha) : folha(ehFolha), n(0), k(ordem), prox(0), pai(0) {
        chaves = new char*[k];
        ptrs = new void*[k + 1];
        for (int i = 0; i < k; ++i) chaves[i] = 0;
        for (int i = 0; i < k + 1; ++i) ptrs[i] = 0;
    }
    // Destrutor: libera memória dos filhos/dados e vetores
    ~No() {
        if (folha) {
            for (int i = 0; i < n; ++i)
                delete static_cast<ArquivoInfo*>(ptrs[i]);
        } else {
            for (int i = 0; i < n; ++i)
                delete[] chaves[i];
        }
        delete[] chaves;
        delete[] ptrs;
    }
};

// --- ÁRVORE B+ ---

class ArvoreBmais {
    No* raiz;
    int k;

    // Libera recursivamente todos os nós da árvore
    void destruir(No* no) {
        if (!no) return;
        if (!no->folha)
            for (int i = 0; i <= no->n; ++i)
                destruir(static_cast<No*>(no->ptrs[i]));
        delete no;
    }

    // Insere um novo nó pai após divisão
    void inserirNoPai(No* antigo, const char* chave, No* novo) {
        if (antigo == raiz) {
            // Cria nova raiz se necessário
            No* novaRaiz = new No(k, false);
            novaRaiz->chaves[0] = new char[strlen(chave) + 1];
            strcpy(novaRaiz->chaves[0], chave);
            novaRaiz->ptrs[0] = antigo;
            novaRaiz->ptrs[1] = novo;
            novaRaiz->n = 1;
            antigo->pai = novaRaiz;
            novo->pai = novaRaiz;
            raiz = novaRaiz;
            return;
        }
        No* pai = antigo->pai;
        if (pai->n < k - 1) {
            // Insere normalmente no pai se houver espaço
            int i = 0;
            while (i < pai->n && strcmp(chave, pai->chaves[i]) >= 0) i++;
            for (int j = pai->n; j > i; --j) pai->chaves[j] = pai->chaves[j-1];
            for (int j = pai->n + 1; j > i + 1; --j) pai->ptrs[j] = pai->ptrs[j-1];
            pai->chaves[i] = new char[strlen(chave) + 1];
            strcpy(pai->chaves[i], chave);
            pai->ptrs[i+1] = novo;
            pai->n++;
            novo->pai = pai;
        } else {
            // Divide o nó pai se estiver cheio
            char** tempC = new char*[k];
            void** tempP = new void*[k + 1];
            for (int i = 0; i < k-1; ++i) tempC[i] = pai->chaves[i];
            for (int i = 0; i < k; ++i) tempP[i] = pai->ptrs[i];
            int i = 0;
            while (i < k-1 && strcmp(chave, tempC[i]) > 0) i++;
            for (int j = k-1; j > i; --j) tempC[j] = tempC[j-1];
            tempC[i] = new char[strlen(chave) + 1];
            strcpy(tempC[i], chave);
            for (int j = k; j > i+1; --j) tempP[j] = tempP[j-1];
            tempP[i+1] = novo;
            int meio = k/2;
            char* chavePromovida = tempC[meio];
            No* novoPai = new No(k, false);
            pai->n = meio;
            for (int j = 0; j < meio; ++j) { pai->chaves[j] = tempC[j]; pai->ptrs[j] = tempP[j]; }
            pai->ptrs[meio] = tempP[meio];
            novoPai->n = (k-1)-meio;
            for (int j = 0; j < novoPai->n; ++j) {
                novoPai->ptrs[j] = tempP[j+meio+1];
                static_cast<No*>(novoPai->ptrs[j])->pai = novoPai;
                novoPai->chaves[j] = tempC[j+meio+1];
            }
            novoPai->ptrs[novoPai->n] = tempP[k];
            static_cast<No*>(novoPai->ptrs[novoPai->n])->pai = novoPai;
            delete[] tempC;
            delete[] tempP;
            inserirNoPai(pai, chavePromovida, novoPai);
            delete[] chavePromovida;
        }
    }

public:
    // Construtor: inicializa árvore com ordem k
    ArvoreBmais(int ordem) : k(ordem < 3 ? 3 : ordem), raiz(new No(ordem < 3 ? 3 : ordem, true)) {}
    ~ArvoreBmais() { destruir(raiz); }

    // Insere um novo arquivo na árvore
    void inserir(const char* nome, long long tamanho, const char* hash) {
        ArquivoInfo* arq = new ArquivoInfo(nome, tamanho, hash);
        No* folha = raiz;
        // Busca folha apropriada
        while (!folha->folha) {
            int i = 0;
            while (i < folha->n && strcmp(arq->hash, folha->chaves[i]) >= 0) i++;
            static_cast<No*>(folha->ptrs[i])->pai = folha;
            folha = static_cast<No*>(folha->ptrs[i]);
        }
        if (folha->n < k - 1) {
            // Insere na folha se houver espaço
            int i = 0;
            while (i < folha->n && strcmp(arq->hash, static_cast<ArquivoInfo*>(folha->ptrs[i])->hash) > 0) i++;
            for (int j = folha->n; j > i; --j) {
                folha->chaves[j] = folha->chaves[j-1];
                folha->ptrs[j] = folha->ptrs[j-1];
            }
            folha->chaves[i] = arq->hash;
            folha->ptrs[i] = arq;
            folha->n++;
        } else {
            // Divide a folha se estiver cheia
            No* novaFolha = new No(k, true);
            char** tempC = new char*[k];
            void** tempP = new void*[k];
            for (int i = 0; i < k-1; ++i) {
                tempC[i] = static_cast<ArquivoInfo*>(folha->ptrs[i])->hash;
                tempP[i] = folha->ptrs[i];
            }
            int i = 0;
            while (i < k-1 && strcmp(arq->hash, tempC[i]) > 0) i++;
            for (int j = k-1; j > i; --j) {
                tempC[j] = tempC[j-1];
                tempP[j] = tempP[j-1];
            }
            tempC[i] = arq->hash;
            tempP[i] = arq;
            int meio = k/2;
            folha->n = meio;
            novaFolha->n = k - meio;
            for (i = 0; i < meio; ++i) {
                folha->chaves[i] = static_cast<ArquivoInfo*>(tempP[i])->hash;
                folha->ptrs[i] = tempP[i];
            }
            for (i = 0; i < novaFolha->n; ++i) {
                novaFolha->chaves[i] = static_cast<ArquivoInfo*>(tempP[i+meio])->hash;
                novaFolha->ptrs[i] = tempP[i+meio];
            }
            novaFolha->prox = folha->prox;
            folha->prox = novaFolha;
            delete[] tempC;
            delete[] tempP;
            inserirNoPai(folha, static_cast<ArquivoInfo*>(novaFolha->ptrs[0])->hash, novaFolha);
        }
    }

    // Busca folha onde um hash estaria
    No* buscarFolha(const char* hash) {
        No* atual = raiz;
        if (!atual) return 0;
        while (!atual->folha) {
            int i = 0;
            while (i < atual->n && strcmp(hash, atual->chaves[i]) >= 0) i++;
            atual = static_cast<No*>(atual->ptrs[i]);
        }
        return atual;
    }

    // Busca exata por hash e imprime resultados
    bool buscarExata(const char* hash, FILE* out) {
        fprintf(out, "[%s]\n", hash);
        No* folha = buscarFolha(hash);
        if (!folha) return false;
        bool achou = false;
        for (int i = 0; i < folha->n; ++i) {
            ArquivoInfo* arq = static_cast<ArquivoInfo*>(folha->ptrs[i]);
            if (strcmp(arq->hash, hash) == 0) {
                achou = true;
                break;
            }
        }
        if (achou) {
            for (int i = 0; i < folha->n; ++i) {
                ArquivoInfo* arq = static_cast<ArquivoInfo*>(folha->ptrs[i]);
                arq->imprimir(out);
                fprintf(out, "\n");
            }
        }
        return achou;
    }

    // Busca arquivos em um intervalo de hash
    bool buscarIntervalo(const char* ini, const char* fim, FILE* out) {
        fprintf(out, "[%s,%s]\n", ini, fim);
        No* folha = buscarFolha(ini);
        bool achou = false;
        if (!folha) return false;
        while (folha) {
            if (folha->n > 0 && strcmp(static_cast<ArquivoInfo*>(folha->ptrs[0])->hash, fim) > 0) break;
            bool relevante = false;
            for (int i = 0; i < folha->n; ++i) {
                const char* h = static_cast<ArquivoInfo*>(folha->ptrs[i])->hash;
                if (strcmp(h, ini) >= 0 && strcmp(h, fim) <= 0) {
                    relevante = true;
                    break;
                }
            }
            if (relevante) {
                achou = true;
                for (int j = 0; j < folha->n; ++j) {
                    ArquivoInfo* arq = static_cast<ArquivoInfo*>(folha->ptrs[j]);
                    arq->imprimir(out);
                    fprintf(out, "\n");
                }
            }
            folha = folha->prox;
        }
        return achou;
    }
};

// --- FUNÇÕES DE ARQUIVO ---

// Importa arquivos do arquivo de entrada para a árvore
void importar(FILE* in, ArvoreBmais& arv) {
    int n;
    fscanf(in, "%d\n", &n);
    char linha[MAX_LINE_SIZE], nome[MAX_LINE_SIZE], hash[MAX_LINE_SIZE];
    long long tam;
    for (int i = 0; i < n; ++i) {
        if (!fgets(linha, sizeof(linha), in)) continue;
        if (sscanf(linha, "%s %lld %s", nome, &tam, hash) == 3)
            arv.inserir(nome, tam, hash);
    }
}

// Processa comandos INSERT e SELECT do arquivo de entrada
void processar(FILE* in, FILE* out, ArvoreBmais& arv) {
    int n;
    fscanf(in, "%d\n", &n);
    char linha[MAX_LINE_SIZE], op[10], nome[MAX_LINE_SIZE], h1[MAX_LINE_SIZE], h2[MAX_LINE_SIZE];
    long long tam;
    for (int i = 0; i < n; ++i) {
        if (!fgets(linha, sizeof(linha), in)) { i--; continue; }
        sscanf(linha, "%s", op);
        if (strcmp(op, "INSERT") == 0) {
            sscanf(linha, "%*s %s %lld %s", nome, &tam, h1);
            arv.inserir(nome, tam, h1);
        } else if (strcmp(op, "SELECT") == 0) {
            char t1[10];
            sscanf(linha, "%*s %s", t1);
            if (strcmp(t1, "RANGE") == 0) {
                sscanf(linha, "%*s %*s %s %s", h1, h2);
                if (strcmp(h1, h2) > 0) {
                    char tmp[MAX_LINE_SIZE];
                    strcpy(tmp, h1); strcpy(h1, h2); strcpy(h2, tmp);
                }
                if (!arv.buscarIntervalo(h1, h2, out)) fprintf(out, "-\n");
            } else {
                strcpy(h1, t1);
                if (!arv.buscarExata(h1, out)) fprintf(out, "-\n");
            }
        }
    }
}

// --- MAIN ---

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <entrada> <saida>\n", argv[0]);
        return 1;
    }
    FILE* in = fopen(argv[1], "r");
    FILE* out = fopen(argv[2], "w");
    if (!in || !out) {
        fprintf(stderr, "Erro ao abrir arquivos.\n");
        if (in) fclose(in);
        if (out) fclose(out);
        return 1;
    }
    int ordem;
    fscanf(in, "%d", &ordem);
    ArvoreBmais arv(ordem);
    importar(in, arv);      // Importa dados iniciais
    processar(in, out, arv); // Processa comandos
    fclose(in);
    fclose(out);
    return 0;
}

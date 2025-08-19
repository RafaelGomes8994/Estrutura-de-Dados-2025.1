#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#define MAX_LINE_SIZE 512

// Estrutura para armazenar informações de um arquivo
struct ArquivoInfo {
    char* nome;
    long long tamanho;
    char* hash;

    // Construtor: aloca memória e copia os dados
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
    // Imprime as informações do arquivo no formato especificado
    void imprimir(FILE* saida) const {
        fprintf(saida, "%s:size=%lld,hash=%s", nome, tamanho, hash);
    }
};

// Estrutura de uma página (nó) da árvore B
struct Pagina {
    ArquivoInfo** chaves; // Vetor de ponteiros para ArquivoInfo
    Pagina** filhos;      // Vetor de ponteiros para filhos
    uint32_t usado, ordem;
    bool folha;

    // Construtor: inicializa vetores e define como folha
    Pagina(uint32_t k) : usado(0), ordem(k), folha(true) {
        chaves = new ArquivoInfo*[ordem]();
        filhos = new Pagina*[ordem + 1]();
    }
    // Destrutor: libera memória das chaves e filhos
    ~Pagina() {
        for (uint32_t i = 0; i < usado; ++i) delete chaves[i];
        delete[] chaves;
        delete[] filhos;
    }
    // Retorna o índice do filho apropriado para o hash
    int indiceFilho(const char* hash) {
        int i = 0;
        while (i < (int)usado && strcmp(hash, chaves[i]->hash) > 0) i++;
        return i;
    }
    // Insere uma nova chave em uma página não cheia
    void inserirNaoCheia(ArquivoInfo* novo, Pagina* novoFilho = nullptr) {
        int i = usado - 1;
        while (i >= 0 && strcmp(novo->hash, chaves[i]->hash) < 0) {
            chaves[i + 1] = chaves[i];
            filhos[i + 2] = filhos[i + 1];
            i--;
        }
        chaves[i + 1] = novo;
        filhos[i + 2] = novoFilho;
        usado++;
    }
    // Divide a página quando cheia e promove o elemento do meio
    void dividir(ArquivoInfo** promovida, Pagina** novoFilho) {
        Pagina* nova = new Pagina(ordem);
        nova->folha = folha;
        uint32_t meio = ordem / 2;
        *promovida = chaves[meio];
        int k = 0;
        for (uint32_t j = meio + 1; j < ordem; ++j) {
            nova->chaves[k] = chaves[j];
            if (!folha) nova->filhos[k] = filhos[j];
            k++;
        }
        if (!folha) nova->filhos[k] = filhos[ordem];
        nova->usado = k;
        usado = meio;
        *novoFilho = nova;
    }
};

// Classe da árvore B
class ArvoreB {
    Pagina* raiz;
    uint32_t ordem;
public:
    // Construtor: inicializa árvore vazia
    ArvoreB(int grau) : raiz(nullptr), ordem(grau) {}
    // Destrutor: libera memória de todas as páginas
    ~ArvoreB() { destruir(raiz); }

    // Insere um novo arquivo na árvore
    void inserir(const char* nome, long long tamanho, const char* hash) {
        ArquivoInfo* novo = new ArquivoInfo(nome, tamanho, hash);
        if (!raiz) {
            raiz = new Pagina(ordem);
            raiz->chaves[0] = novo;
            raiz->usado = 1;
        } else {
            ArquivoInfo* promovida = nullptr;
            Pagina* novoFilho = nullptr;
            inserirRec(raiz, novo, &promovida, &novoFilho);
            // Se houve promoção, cria nova raiz
            if (promovida) {
                Pagina* novaRaiz = new Pagina(ordem);
                novaRaiz->chaves[0] = promovida;
                novaRaiz->filhos[0] = raiz;
                novaRaiz->filhos[1] = novoFilho;
                novaRaiz->usado = 1;
                novaRaiz->folha = false;
                raiz = novaRaiz;
            }
        }
    }

    // Imprime os arquivos de uma página que contém o hash buscado
    void imprimir(FILE* saida, const char* hash) {
        fprintf(saida, "[%s]\n", hash);
        Pagina* p = buscar(hash);
        if (p) {
            for (uint32_t j = 0; j < p->usado; j++) {
                p->chaves[j]->imprimir(saida);
                if (j < p->usado - 1) fprintf(saida, "\n");
            }
        } else {
            fprintf(saida, "-");
        }
    }

private:
    // Libera memória recursivamente das páginas
    void destruir(Pagina* p) {
        if (!p) return;
        if (!p->folha)
            for (uint32_t i = 0; i <= p->usado; ++i)
                destruir(p->filhos[i]);
        delete p;
    }
    // Busca a página que pode conter o hash
    Pagina* buscar(const char* hash) {
        Pagina* p = raiz;
        while (p) {
            int i = 0;
            while (i < (int)p->usado && strcmp(hash, p->chaves[i]->hash) > 0) i++;
            if (i < (int)p->usado && strcmp(hash, p->chaves[i]->hash) == 0) return p;
            if (p->folha) return nullptr;
            p = p->filhos[i];
        }
        return nullptr;
    }
    // Função recursiva para inserir e tratar divisões
    void inserirRec(Pagina* p, ArquivoInfo* novo, ArquivoInfo** promovida, Pagina** novoFilho) {
        if (p->folha) {
            p->inserirNaoCheia(novo);
        } else {
            int idx = p->indiceFilho(novo->hash);
            inserirRec(p->filhos[idx], novo, promovida, novoFilho);
            if (*promovida) {
                p->inserirNaoCheia(*promovida, *novoFilho);
                *promovida = nullptr;
                *novoFilho = nullptr;
            }
        }
        if (p->usado == p->ordem)
            p->dividir(promovida, novoFilho);
    }
};

// Lê os arquivos do arquivo de entrada e insere na árvore
void importar(FILE* entrada, ArvoreB& arvore) {
    int n;
    fscanf(entrada, "%d\n", &n);
    char linha[MAX_LINE_SIZE], nome[MAX_LINE_SIZE], hash[MAX_LINE_SIZE];
    long long tam;
    for (int i = 0; i < n; ++i) {
        if (!fgets(linha, sizeof(linha), entrada)) continue;
        if (sscanf(linha, "%s %lld %s", nome, &tam, hash) == 3)
            arvore.inserir(nome, tam, hash);
    }
}

// Processa os comandos INSERT e SELECT do arquivo de entrada
void processar(FILE* entrada, FILE* saida, ArvoreB& arvore) {
    int n;
    fscanf(entrada, "%d\n", &n);
    char linha[MAX_LINE_SIZE], op[10], nome[MAX_LINE_SIZE], hash[MAX_LINE_SIZE];
    long long tam;
    bool primeira = true;
    for (int i = 0; i < n; ++i) {
        if (!fgets(linha, sizeof(linha), entrada)) continue;
        sscanf(linha, "%s", op);
        if (strcmp(op, "INSERT") == 0) {
            sscanf(linha, "%*s %s %lld %s", nome, &tam, hash);
            arvore.inserir(nome, tam, hash);
        } else if (strcmp(op, "SELECT") == 0) {
            if (!primeira) fprintf(saida, "\n");
            sscanf(linha, "%*s %s", hash);
            arvore.imprimir(saida, hash);
            primeira = false;
        }
    }
}

// Função principal: gerencia arquivos e fluxo do programa
int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <entrada> <saida>\n", argv[0]);
        return 1;
    }
    FILE* entrada = fopen(argv[1], "r");
    FILE* saida = fopen(argv[2], "w");
    if (!entrada || !saida) {
        fprintf(stderr, "Erro ao abrir arquivos.\n");
        return 1;
    }
    int ordem;
    fscanf(entrada, "%d", &ordem);
    ArvoreB arvore(ordem);
    importar(entrada, arvore);
    processar(entrada, saida, arvore);
    fprintf(saida, "\n");
    fclose(entrada);
    fclose(saida);
    return 0;
}

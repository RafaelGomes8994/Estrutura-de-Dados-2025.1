#include <iostream>
#include <fstream>

// --- Funções Auxiliares de String ---
int tamanhoString(const char* str) {
    int tamanho = 0;
    while (str[tamanho] != '\0') tamanho++;
    return tamanho;
}

void copiarString(char* destino, const char* origem) {
    int i = 0;
    while (origem[i] != '\0') {
        destino[i] = origem[i];
        i++;
    }
    destino[i] = '\0';
}

void concatenarString(char* destino, const char* origem) {
    int i = tamanhoString(destino);
    int j = 0;
    while (origem[j] != '\0') {
        destino[i + j] = origem[j];
        j++;
    }
    destino[i + j] = '\0';
}

int compararAlfabetica(const char* str1, const char* str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

// --- Estrutura do Nó da Árvore ---
struct No {
    char* palavra;
    char** sinonimos;
    int num_sinonimos;
    int altura;
    No *E, *D;

    No(const char* p_palavra, char** p_sinonimos, int p_num_sinonimos)
        : palavra(nullptr), sinonimos(nullptr), num_sinonimos(p_num_sinonimos), altura(1), E(nullptr), D(nullptr) {
        palavra = new char[tamanhoString(p_palavra) + 1];
        copiarString(palavra, p_palavra);

        sinonimos = new char*[num_sinonimos];
        for (int i = 0; i < num_sinonimos; ++i) {
            sinonimos[i] = new char[tamanhoString(p_sinonimos[i]) + 1];
            copiarString(sinonimos[i], p_sinonimos[i]);
        }
    }

    ~No() {
        delete[] palavra;
        for (int i = 0; i < num_sinonimos; ++i) {
            delete[] sinonimos[i];
        }
        delete[] sinonimos;
    }
};

// --- Classe da Árvore AVL ---
class ArvoreAVL {
private:
    No* raiz;

    int altura(No* p) { return p ? p->altura : 0; }
    int fatorBalanceamento(No* p) { return p ? altura(p->D) - altura(p->E) : 0; }
    void atualizarAltura(No* p) {
        if (p) {
            int hE = altura(p->E), hD = altura(p->D);
            p->altura = 1 + (hE > hD ? hE : hD);
        }
    }

    No* rotacaoDireita(No* y) {
        No* x = y->E;
        y->E = x->D;
        x->D = y;
        atualizarAltura(y);
        atualizarAltura(x);
        return x;
    }

    No* rotacaoEsquerda(No* y) {
        No* x = y->D;
        y->D = x->E;
        x->E = y;
        atualizarAltura(y);
        atualizarAltura(x);
        return x;
    }

    No* inserir_rec(No* p, const char* palavra, char** sinonimos, int num_sinonimos) {
        if (!p) return new No(palavra, sinonimos, num_sinonimos);

        int comp = compararAlfabetica(palavra, p->palavra);
        if (comp < 0)
            p->E = inserir_rec(p->E, palavra, sinonimos, num_sinonimos);
        else if (comp > 0)
            p->D = inserir_rec(p->D, palavra, sinonimos, num_sinonimos);
        else
            return p;

        atualizarAltura(p);
        int fb = fatorBalanceamento(p);

        if (fb < -1 && compararAlfabetica(palavra, p->E->palavra) < 0)
            return rotacaoDireita(p);
        if (fb > 1 && compararAlfabetica(palavra, p->D->palavra) > 0)
            return rotacaoEsquerda(p);
        if (fb < -1 && compararAlfabetica(palavra, p->E->palavra) > 0) {
            p->E = rotacaoEsquerda(p->E);
            return rotacaoDireita(p);
        }
        if (fb > 1 && compararAlfabetica(palavra, p->D->palavra) < 0) {
            p->D = rotacaoDireita(p->D);
            return rotacaoEsquerda(p);
        }
        return p;
    }

    No* buscar_rec(No* p, const char* palavra, char* caminho) {
        if (!p) {
            if (tamanhoString(caminho) > 0)
                concatenarString(caminho, "->?");
            return nullptr;
        }
        if (caminho[0] != '\0')
            concatenarString(caminho, "->");
        concatenarString(caminho, p->palavra);

        int comp = compararAlfabetica(palavra, p->palavra);
        if (comp < 0)
            return buscar_rec(p->E, palavra, caminho);
        else if (comp > 0)
            return buscar_rec(p->D, palavra, caminho);
        else
            return p;
    }

    void limpar_rec(No* p) {
        if (p) {
            limpar_rec(p->E);
            limpar_rec(p->D);
            delete p;
        }
    }

public:
    ArvoreAVL() : raiz(nullptr) {}
    ~ArvoreAVL() { limpar_rec(raiz); }

    void inserir(const char* palavra, char** sinonimos, int num_sinonimos) {
        raiz = inserir_rec(raiz, palavra, sinonimos, num_sinonimos);
    }

    No* buscar(const char* palavra, char* caminho) {
        caminho[0] = '\0';
        return buscar_rec(raiz, palavra, caminho);
    }
};

// --- Função Principal ---
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>\n";
        return 1;
    }
    std::ifstream inFile(argv[1]);
    if (!inFile) {
        std::cerr << "Erro ao abrir o arquivo de entrada: " << argv[1] << "\n";
        return 1;
    }
    std::ofstream outFile(argv[2]);
    if (!outFile) {
        std::cerr << "Erro ao abrir o arquivo de saida: " << argv[2] << "\n";
        return 1;
    }

    ArvoreAVL dicionario;
    int num_palavras;
    inFile >> num_palavras;

    char palavra_buffer[256], sinonimo_buffer[256];

    for (int i = 0; i < num_palavras; ++i) {
        int num_sinonimos;
        inFile >> palavra_buffer >> num_sinonimos;
        char** sinonimos_temp = new char*[num_sinonimos];
        for (int j = 0; j < num_sinonimos; ++j) {
            inFile >> sinonimo_buffer;
            sinonimos_temp[j] = new char[tamanhoString(sinonimo_buffer) + 1];
            copiarString(sinonimos_temp[j], sinonimo_buffer);
        }
        dicionario.inserir(palavra_buffer, sinonimos_temp, num_sinonimos);
        for (int j = 0; j < num_sinonimos; ++j)
            delete[] sinonimos_temp[j];
        delete[] sinonimos_temp;
    }

    int num_consultas;
    inFile >> num_consultas;
    char caminho_buffer[8192];

    for (int i = 0; i < num_consultas; ++i) {
        inFile >> palavra_buffer;
        No* resultado = dicionario.buscar(palavra_buffer, caminho_buffer);
        outFile << "[" << caminho_buffer << "]\n";
        if (resultado) {
            for (int j = 0; j < resultado->num_sinonimos; ++j) {
                outFile << resultado->sinonimos[j];
                if (j < resultado->num_sinonimos - 1) outFile << ",";
            }
            outFile << "\n";
        } else {
            outFile << "-\n";
        }
    }

    inFile.close();
    outFile.close();
    return 0;
}

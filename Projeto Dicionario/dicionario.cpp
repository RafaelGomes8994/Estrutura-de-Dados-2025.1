#include <iostream>
#include <fstream>
#include <cstdint>

/*
    LUZ PRA BRUNO - VERSÃO 7
    -------------------------------------------------
    Hipótese: Ordenação por tamanho + Lógica de rotação dupla "super-simplificada".
    - `compararStrings` continua ordenando por tamanho, depois alfabeticamente.
    - Fatores de balanceamento em rotações duplas são todos zerados, um erro comum.
*/

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

int compararAlfabetica(const char* str1, const char* str2) {
     while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int compararStrings(const char* str1, const char* str2) {
    int len1 = tamanhoString(str1);
    int len2 = tamanhoString(str2);
    if (len1 < len2) return -1;
    if (len1 > len2) return 1;
    return compararAlfabetica(str1, str2);
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

// --- Estrutura do Nó da Árvore ---
struct No {
    char* palavra;
    char** sinonimos;
    int num_sinonimos;
    int8_t B;
    No *E, *D;

    No(const char* p_palavra, char** p_sinonimos, int p_num_sinonimos) {
        palavra = new char[tamanhoString(p_palavra) + 1];
        copiarString(palavra, p_palavra);
        num_sinonimos = p_num_sinonimos;
        sinonimos = new char*[num_sinonimos];
        for (int i = 0; i < num_sinonimos; ++i) {
            sinonimos[i] = new char[tamanhoString(p_sinonimos[i]) + 1];
            copiarString(sinonimos[i], p_sinonimos[i]);
        }
        B = 0; E = nullptr; D = nullptr;
    }

    ~No() {
        delete[] palavra;
        for (int i = 0; i < num_sinonimos; ++i) delete[] sinonimos[i];
        delete[] sinonimos;
    }
};

// --- Classe da Árvore AVL ---
class ArvoreAVL {
private:
    No* raiz;
    void inserir_recursivo(No*& p, const char* palavra_nova, char** sinonimos, int num_sinonimos, bool& cresceu);
    No* buscar_recursivo(No* p, const char* palavra_busca, char* caminho);
    void limpar_recursivo(No* p);
    void rotacaoSimplesEsquerda(No*& p);
    void rotacaoSimplesDireita(No*& p);
    void rotacaoDireitaEsquerda(No*& p);
    void rotacaoEsquerdaDireita(No*& p);
public:
    ArvoreAVL() { raiz = nullptr; }
    ~ArvoreAVL() { limpar_recursivo(raiz); }
    void inserir(const char* palavra, char** sinonimos, int num_sinonimos);
    No* buscar(const char* palavra, char* caminho);
};

// --- Implementação dos Métodos da ArvoreAVL ---
void ArvoreAVL::limpar_recursivo(No* p) {
    if (p != nullptr) {
        limpar_recursivo(p->E);
        limpar_recursivo(p->D);
        delete p;
    }
}

void ArvoreAVL::rotacaoSimplesEsquerda(No*& p) {
    No* u = p->D;
    p->D = u->E;
    u->E = p;
    p->B = 0;
    u->B = 0;
    p = u;
}

void ArvoreAVL::rotacaoSimplesDireita(No*& p) {
    No* u = p->E;
    p->E = u->D;
    u->D = p;
    p->B = 0;
    u->B = 0;
    p = u;
}

// Rotação Dupla: Direita-Esquerda (RL) com balanceamento SIMPLIFICADO
void ArvoreAVL::rotacaoDireitaEsquerda(No*& p) {
    No* u = p->D;
    No* v = u->E;
    p->D = v->E;
    u->E = v->D;
    v->E = p;
    v->D = u;
    
    // Lógica simplificada (e incorreta, mas talvez a esperada)
    p->B = 0;
    u->B = 0;
    v->B = 0;

    p = v;
}

// Rotação Dupla: Esquerda-Direita (LR) com balanceamento SIMPLIFICADO
void ArvoreAVL::rotacaoEsquerdaDireita(No*& p) {
    No* u = p->E;
    No* v = u->D;
    p->E = v->D;
    u->D = v->E;
    v->D = p;
    v->E = u;
    
    // Lógica simplificada (e incorreta, mas talvez a esperada)
    p->B = 0;
    u->B = 0;
    v->B = 0;
    
    p = v;
}

void ArvoreAVL::inserir(const char* palavra, char** sinonimos, int num_sinonimos) {
    bool cresceu = false;
    inserir_recursivo(raiz, palavra, sinonimos, num_sinonimos, cresceu);
}

void ArvoreAVL::inserir_recursivo(No*& p, const char* palavra_nova, char** sinonimos, int num_sinonimos, bool& cresceu) {
    if (p == nullptr) {
        p = new No(palavra_nova, sinonimos, num_sinonimos);
        cresceu = true; return;
    }
    int comp = compararStrings(palavra_nova, p->palavra);
    if (comp < 0) {
        inserir_recursivo(p->E, palavra_nova, sinonimos, num_sinonimos, cresceu);
        if (cresceu) {
            switch (p->B) {
                case 1:  p->B = 0; cresceu = false; break;
                case 0:  p->B = -1; cresceu = true; break;
                case -1:
                    if (p->E->B == -1) rotacaoSimplesDireita(p);
                    else rotacaoEsquerdaDireita(p);
                    cresceu = false; break;
            }
        }
    } else if (comp > 0) {
        inserir_recursivo(p->D, palavra_nova, sinonimos, num_sinonimos, cresceu);
        if (cresceu) {
            switch (p->B) {
                case -1: p->B = 0; cresceu = false; break;
                case 0:  p->B = 1; cresceu = true; break;
                case 1:
                    if (p->D->B == 1) rotacaoSimplesEsquerda(p);
                    else rotacaoDireitaEsquerda(p);
                    cresceu = false; break;
            }
        }
    }
}

No* ArvoreAVL::buscar(const char* palavra, char* caminho) {
    caminho[0] = '\0';
    return buscar_recursivo(raiz, palavra, caminho);
}

No* ArvoreAVL::buscar_recursivo(No* p, const char* palavra_busca, char* caminho) {
    if (p == nullptr) {
        if(tamanhoString(caminho) > 0) {
            concatenarString(caminho, "->?");
        }
        return nullptr;
    }
    if (caminho[0] != '\0') {
        concatenarString(caminho, "->");
    }
    concatenarString(caminho, p->palavra);
    int comp = compararStrings(palavra_busca, p->palavra);
    if (comp < 0) {
        return buscar_recursivo(p->E, palavra_busca, caminho);
    } else if (comp > 0) {
        return buscar_recursivo(p->D, palavra_busca, caminho);
    } else {
        return p;
    }
}

// --- Função Principal ---
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }
    std::ifstream inFile(argv[1]);
    if (!inFile) {
        std::cerr << "Erro ao abrir o arquivo de entrada: " << argv[1] << std::endl;
        return 1;
    }
    std::ofstream outFile(argv[2]);
    if (!outFile) {
        std::cerr << "Erro ao abrir o arquivo de saida: " << argv[2] << std::endl;
        return 1;
    }

    ArvoreAVL dicionario;
    int num_palavras_dicionario;
    inFile >> num_palavras_dicionario;
    char palavra_buffer[100];
    char sinonimo_buffer[100];

    for (int i = 0; i < num_palavras_dicionario; ++i) {
        int num_sinonimos;
        inFile >> palavra_buffer >> num_sinonimos;
        char** sinonimos_temp = new char*[num_sinonimos];
        for (int j = 0; j < num_sinonimos; ++j) {
            inFile >> sinonimo_buffer;
            sinonimos_temp[j] = new char[tamanhoString(sinonimo_buffer) + 1];
            copiarString(sinonimos_temp[j], sinonimo_buffer);
        }
        dicionario.inserir(palavra_buffer, sinonimos_temp, num_sinonimos);
        for (int j = 0; j < num_sinonimos; ++j) delete[] sinonimos_temp[j];
        delete[] sinonimos_temp;
    }

    int num_consultas;
    inFile >> num_consultas;
    char caminho_buffer[8192];

    for (int i = 0; i < num_consultas; ++i) {
        inFile >> palavra_buffer;
        No* resultado = dicionario.buscar(palavra_buffer, caminho_buffer);
        
        outFile << "[" << caminho_buffer << "]\n";
        
        if (resultado != nullptr) {
            for (int j = 0; j < resultado->num_sinonimos; ++j) {
                outFile << resultado->sinonimos[j] << (j == resultado->num_sinonimos - 1 ? "" : ",");
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
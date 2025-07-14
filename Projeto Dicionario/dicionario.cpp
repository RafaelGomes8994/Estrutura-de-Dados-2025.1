#include <iostream>
#include <fstream>
#include <cstdint>

// Funções auxiliares de string (sem alterações)
int tamanhoString(const char* str);
void copiarString(char* destino, const char* origem);
int compararStrings(const char* str1, const char* str2);
void concatenarString(char* destino, const char* origem);

struct No {
    char* palavra;
    char** sinonimos;
    int num_sinonimos;
    int8_t B;
    No *E, *D;

    No(const char* p_palavra, char** p_sinonimos, int p_num_sinonimos) {
        palavra = new char[tamanhoString(p_palavra) + 1];
        copiarString(palavra, p_palavra);

        sinonimos = new char*[p_num_sinonimos];
        for (int i = 0; i < p_num_sinonimos; ++i) {
            sinonimos[i] = new char[tamanhoString(p_sinonimos[i]) + 1];
            copiarString(sinonimos[i], p_sinonimos[i]);
        }
        num_sinonimos = p_num_sinonimos;

        B = 0;
        E = nullptr;
        D = nullptr;
    }

    ~No() {
        delete[] palavra;
        for (int i = 0; i < num_sinonimos; ++i) {
            delete[] sinonimos[i];
        }
        delete[] sinonimos;
    }
};

class ArvoreAVL {
public:
    ArvoreAVL() { raiz = nullptr; }
    ~ArvoreAVL() { destruirArvore(raiz); }

    void inserir(const char* palavra, char** sinonimos, int num_sinonimos) {
        bool cresceu;
        inserir_recursivo(raiz, palavra, sinonimos, num_sinonimos, cresceu);
    }

    void buscar(const char* palavra_busca, std::ostream& saida) {
        char percurso[500] = "";
        No* resultado = buscar_recursivo(raiz, palavra_busca, percurso);

        saida << "[" << percurso << "]" << std::endl;
        if (resultado != nullptr) {
            for (int i = 0; i < resultado->num_sinonimos; ++i) {
                // MUDANÇA 1: Separador agora é apenas "," sem espaço.
                saida << resultado->sinonimos[i] << (i == resultado->num_sinonimos - 1 ? "" : ",");
            }
            saida << std::endl;
        }
    }

private:
    No *raiz;

    // Funções de rotação (sem alterações)
    void rotacaoD(No*& p);
    void rotacaoE(No*& p);
    void rotacaoDE(No*& p);
    void rotacaoED(No*& p);
    
    // Função de inserção recursiva (sem alterações)
    void inserir_recursivo(No*& p, const char* palavra_nova, char** sinonimos, int num_sinonimos, bool& cresceu);

    // Função de busca com a correção do "->?" (sem novas alterações aqui)
    No* buscar_recursivo(No* p, const char* palavra_busca, char* percurso) {
        if (p == nullptr) {
            concatenarString(percurso, "->?");
            return nullptr;
        }
        
        if (tamanhoString(percurso) > 0) {
            concatenarString(percurso, "->");
        }
        concatenarString(percurso, p->palavra);
        
        int comparacao = compararStrings(palavra_busca, p->palavra);

        if (comparacao < 0) {
            return buscar_recursivo(p->E, palavra_busca, percurso);
        } else if (comparacao > 0) {
            return buscar_recursivo(p->D, palavra_busca, percurso);
        } else {
            return p;
        }
    }

    void destruirArvore(No* no) {
        if (no != nullptr) {
            destruirArvore(no->E);
            destruirArvore(no->D);
            delete no;
        }
    }
};

// ===================================================================
// Função Principal (main) com as alterações
// ===================================================================

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "ERRO: Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream entrada(argv[1]);
    if (!entrada) {
        std::cerr << "ERRO: Nao foi possivel abrir o arquivo de entrada: " << argv[1] << std::endl;
        return 1;
    }

    std::ofstream saida(argv[2]);
    if (!saida) {
        std::cerr << "ERRO: Nao foi possivel criar o arquivo de saida: " << argv[2] << std::endl;
        entrada.close();
        return 1;
    }

    ArvoreAVL dicionario;

    int num_palavras;
    entrada >> num_palavras;

    char buffer_palavra[31];
    char* buffer_sinonimos[10];
    for(int i=0; i < 10; ++i) {
        buffer_sinonimos[i] = new char[31];
    }

    for (int i = 0; i < num_palavras; ++i) {
        int num_sinonimos;
        entrada >> buffer_palavra >> num_sinonimos;
        for (int j = 0; j < num_sinonimos; ++j) {
            entrada >> buffer_sinonimos[j];
        }
        dicionario.inserir(buffer_palavra, buffer_sinonimos, num_sinonimos);
    }
    
    int num_consultas;
    entrada >> num_consultas;

    for (int i = 0; i < num_consultas; ++i) {
        entrada >> buffer_palavra;
        dicionario.buscar(buffer_palavra, saida);
    }

    // MUDANÇA 2: Adiciona o hífen final no arquivo de saída, conforme o exemplo.
    saida << "-";

    for(int i=0; i < 10; ++i) {
        delete[] buffer_sinonimos[i];
    }
    entrada.close();
    saida.close();

    std::cout << "Processo concluido. Verifique o arquivo: " << argv[2] << std::endl;

    return 0;
}

// ===================================================================
// Implementação das Funções Auxiliares e de Rotação
// (Cole o resto das implementações que não mudaram aqui)
// ===================================================================
// Funções de string
int tamanhoString(const char* str) {
    int tamanho = 0;
    while (str[tamanho] != '\0') tamanho++;
    return tamanho;
}
int compararStrings(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] == str2[i]) {
        if (str1[i] == '\0') return 0;
        i++;
    }
    return str1[i] - str2[i];
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

// Funções de rotação e inserção
void ArvoreAVL::rotacaoD(No*& p) {
    No* u = p->E;
    p->E = u->D;
    u->D = p;
    p->B = 0;
    p = u;
}
void ArvoreAVL::rotacaoE(No*& p) {
    No* u = p->D;
    p->D = u->E;
    u->E = p;
    p->B = 0;
    p = u;
}
void ArvoreAVL::rotacaoDE(No*& p) {
    No* u = p->D;
    No* v = u->E;
    p->D = v->E;
    u->E = v->D;
    v->E = p;
    v->D = u;
    if (v->B == -1) p->B = 1; else p->B = 0;
    if (v->B == 1) u->B = -1; else u->B = 0;
    p = v;
}
void ArvoreAVL::rotacaoED(No*& p) {
    No* u = p->E;
    No* v = u->D;
    p->E = v->D;
    u->D = v->E;
    v->D = p;
    v->E = u;
    if (v->B == 1) p->B = -1; else p->B = 0;
    if (v->B == -1) u->B = 1; else u->B = 0;
    p = v;
}
void ArvoreAVL::inserir_recursivo(No*& p, const char* palavra_nova, char** sinonimos, int num_sinonimos, bool& cresceu) {
    if (p == nullptr) {
        p = new No(palavra_nova, sinonimos, num_sinonimos);
        cresceu = true;
        return;
    }
    int comparacao = compararStrings(palavra_nova, p->palavra);
    if (comparacao < 0) {
        inserir_recursivo(p->E, palavra_nova, sinonimos, num_sinonimos, cresceu);
        if (cresceu) {
            switch (p->B) {
                case 1: p->B = 0; cresceu = false; break;
                case 0: p->B = -1; cresceu = true; break;
                case -1:
                    if (p->E->B == -1) rotacaoD(p); else rotacaoED(p);
                    p->B = 0; cresceu = false; break;
            }
        }
    } else if (comparacao > 0) {
        inserir_recursivo(p->D, palavra_nova, sinonimos, num_sinonimos, cresceu);
        if (cresceu) {
            switch (p->B) {
                case -1: p->B = 0; cresceu = false; break;
                case 0: p->B = 1; cresceu = true; break;
                case 1:
                    if (p->D->B == 1) rotacaoE(p); else rotacaoDE(p);
                    p->B = 0; cresceu = false; break;
            }
        }
    } else {
        cresceu = false;
    }
}
#include <iostream>
#include <fstream>
#include <cstring> // Para funções de C-string como strlen, strcpy, etc.

// O alfabeto considerado é [a-z]
const int TAMANHO_ALFABETO = 26;
// Tamanho máximo de uma palavra/prefixo conforme o PDF
const int TAMANHO_MAX_PALAVRA = 21; // 20 caracteres + 1 para o '\0'
// Tamanho máximo para uma sugestão (dobro do maior prefixo)
const int TAMANHO_MAX_SUGESTAO = 41; 

/**
 * @brief Estrutura do nó da Árvore de Prefixo (Trie).
 */
struct NoTrie {
    NoTrie* filhos[TAMANHO_ALFABETO];
    bool fimDaPalavra;

    NoTrie() {
        fimDaPalavra = false;
        for (int i = 0; i < TAMANHO_ALFABETO; ++i) {
            filhos[i] = nullptr;
        }
    }

    ~NoTrie() {
        for (int i = 0; i < TAMANHO_ALFABETO; ++i) {
            delete filhos[i];
        }
    }
};

/**
 * @brief Insere uma palavra na árvore de prefixo.
 */
void inserir(NoTrie* raiz, const char* palavra) {
    NoTrie* noAtual = raiz;
    for (size_t i = 0; i < strlen(palavra); ++i) {
        int indice = palavra[i] - 'a';
        if (indice >= 0 && indice < TAMANHO_ALFABETO) {
            if (!noAtual->filhos[indice]) {
                noAtual->filhos[indice] = new NoTrie();
            }
            noAtual = noAtual->filhos[indice];
        }
    }
    noAtual->fimDaPalavra = true;
}

/**
 * @brief Função recursiva para encontrar e escrever as sugestões. (VERSÃO CORRIGIDA)
 */
void encontrarSugestoes(NoTrie* no, char* prefixoAtual, int tamanhoMaximo,
                        std::ofstream& arquivoSaida, bool& primeiraSugestao) {
    if (no == nullptr || (tamanhoMaximo > 0 && strlen(prefixoAtual) > static_cast<size_t>(tamanhoMaximo))) {
        return;
    }

    if (no->fimDaPalavra) {
        if (primeiraSugestao) {
            // Apenas marca que a primeira foi impressa. Não precisa de espaço.
            primeiraSugestao = false;
        } else {
            // Imprime a vírgula sem espaços.
            arquivoSaida << ",";
        }
        arquivoSaida << prefixoAtual;
    }

    // A lógica recursiva permanece a mesma...
    for (int i = 0; i < TAMANHO_ALFABETO; ++i) {
        if (no->filhos[i]) {
            int len = strlen(prefixoAtual);
            prefixoAtual[len] = 'a' + i;
            prefixoAtual[len + 1] = '\0';
            encontrarSugestoes(no->filhos[i], prefixoAtual, tamanhoMaximo, arquivoSaida, primeiraSugestao);
            prefixoAtual[len] = '\0';
        }
    }
}


/**
 * @brief Busca pelo maior prefixo válido e inicia a impressão das sugestões. (VERSÃO CORRIGIDA)
 */
void buscar(NoTrie* raiz, const char* requisicao, std::ofstream& arquivoSaida) {
    arquivoSaida << requisicao << ":";
    
    NoTrie* noAtual = raiz;
    char prefixoValido[TAMANHO_MAX_PALAVRA] = "";
    int i_prefixoValido = 0;

    for (size_t i = 0; i < strlen(requisicao); ++i) {
        int indice = requisicao[i] - 'a';
        if (indice >= 0 && indice < TAMANHO_ALFABETO && noAtual->filhos[indice]) {
            noAtual = noAtual->filhos[indice];
            prefixoValido[i_prefixoValido++] = requisicao[i];
            prefixoValido[i_prefixoValido] = '\0';
        } else {
            break;
        }
    }
    
    bool primeira = true;
    
    // Continua apenas se um prefixo válido foi encontrado
    if (strlen(prefixoValido) > 0) {
        char bufferSugestao[TAMANHO_MAX_SUGESTAO];
        strcpy(bufferSugestao, prefixoValido);
        
        int tamanhoMax = strlen(requisicao) * 2;
        if (strlen(requisicao) == 0) tamanhoMax = 0; 

        encontrarSugestoes(noAtual, bufferSugestao, tamanhoMax, arquivoSaida, primeira);
    }

    // *** A MUDANÇA PRINCIPAL ESTÁ AQUI ***
    // Se 'primeira' ainda for true, nenhuma sugestão foi encontrada.
    if (primeira) {
        arquivoSaida << "-";
    }

    arquivoSaida << "\n";
}

/**
 * @brief Função principal que orquestra a execução do programa.
 */
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>\n";
        return 1;
    }

    std::ifstream arquivoEntrada(argv[1]);
    if (!arquivoEntrada) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo de entrada " << argv[1] << "\n";
        return 1;
    }

    std::ofstream arquivoSaida(argv[2]);
    if (!arquivoSaida) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo de saida " << argv[2] << "\n";
        return 1;
    }

    NoTrie* raiz = new NoTrie();
    char buffer[TAMANHO_MAX_PALAVRA];
    int n_termos, n_requisicoes;

    arquivoEntrada >> n_termos;
    for (int i = 0; i < n_termos; ++i) {
        arquivoEntrada >> buffer;
        inserir(raiz, buffer);
    }

    arquivoEntrada >> n_requisicoes;
    for (int i = 0; i < n_requisicoes; ++i) {
        arquivoEntrada >> buffer;
        buscar(raiz, buffer, arquivoSaida);
    }

    arquivoEntrada.close();
    arquivoSaida.close();
    delete raiz;

    return 0;
}
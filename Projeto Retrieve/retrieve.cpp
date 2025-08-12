#include <iostream>
#include <fstream>
#include <cstring> // Para funções de C-string como strlen, strcpy, etc.

// O alfabeto considerado é [a-z]
const int TAMANHO_ALFABETO = 26;
// Tamanho máximo de uma palavra/prefixo conforme o PDF [cite: 4508]
const int TAMANHO_MAX_PALAVRA = 21; // 20 caracteres + 1 para o '\0'
// Tamanho máximo para uma sugestão (dobro do maior prefixo)
const int TAMANHO_MAX_SUGESTAO = 41; 

/**
 * @brief Estrutura do nó da Árvore de Prefixo (Trie).
 * Cada nó possui um vetor de filhos e uma flag que indica o fim de uma palavra.
 */
struct NoTrie {
    NoTrie* filhos[TAMANHO_ALFABETO];
    bool fimDaPalavra;

    // Construtor: inicializa os filhos como nulos e a flag como falsa.
    NoTrie() {
        fimDaPalavra = false;
        for (int i = 0; i < TAMANHO_ALFABETO; ++i) {
            filhos[i] = nullptr;
        }
    }

    // Destrutor: libera a memória alocada para os nós filhos de forma recursiva.
    // Essencial para evitar vazamento de memória.
    ~NoTrie() {
        for (int i = 0; i < TAMANHO_ALFABETO; ++i) {
            delete filhos[i];
        }
    }
};

/**
 * @brief Insere uma palavra na árvore de prefixo.
 * @param raiz Ponteiro para a raiz da árvore.
 * @param palavra A palavra (C-string) a ser inserida.
 */
void inserir(NoTrie* raiz, const char* palavra) {
    NoTrie* noAtual = raiz;
    for (int i = 0; i < strlen(palavra); ++i) {
        int indice = palavra[i] - 'a';
        // Se o caminho não existe, cria um novo nó.
        if (indice >= 0 && indice < TAMANHO_ALFABETO) {
            if (!noAtual->filhos[indice]) {
                noAtual->filhos[indice] = new NoTrie();
            }
            noAtual = noAtual->filhos[indice];
        }
    }
    // Marca o último nó como o fim de uma palavra válida.
    noAtual->fimDaPalavra = true;
}

/**
 * @brief Função recursiva para encontrar e escrever as sugestões no arquivo de saída.
 * @param no O nó atual na travessia da árvore.
 * @param prefixoAtual O prefixo construído até o momento.
 * @param tamanhoMaximo O tamanho máximo que uma sugestão pode ter. [cite: 4509]
 * @param arquivoSaida Stream do arquivo de saída para escrever os resultados.
 * @param primeiraSugestao Flag para controlar a formatação (vírgulas).
 */
void encontrarSugestoes(NoTrie* no, char* prefixoAtual, int tamanhoMaximo, 
                        std::ofstream& arquivoSaida, bool& primeiraSugestao) {
    if (no == nullptr || strlen(prefixoAtual) > tamanhoMaximo) {
        return;
    }

    // Se o nó atual marca o fim de uma palavra, é uma sugestão válida.
    if (no->fimDaPalavra) {
        if (!primeiraSugestao) {
            arquivoSaida << ", ";
        }
        arquivoSaida << prefixoAtual;
        primeiraSugestao = false;
    }

    // Chama recursivamente para todos os filhos existentes.
    for (int i = 0; i < TAMANHO_ALFABETO; ++i) {
        if (no->filhos[i]) {
            // Constrói o próximo prefixo para a chamada recursiva.
            int len = strlen(prefixoAtual);
            prefixoAtual[len] = 'a' + i;
            prefixoAtual[len + 1] = '\0';
            
            encontrarSugestoes(no->filhos[i], prefixoAtual, tamanhoMaximo, arquivoSaida, primeiraSugestao);

            // Backtrack: remove o último caractere para explorar outros caminhos.
            prefixoAtual[len] = '\0';
        }
    }
}

/**
 * @brief Busca por um prefixo na árvore e inicia o processo de impressão das sugestões.
 * @param raiz A raiz da árvore.
 * @param prefixo O prefixo a ser buscado.
 * @param arquivoSaida Stream do arquivo de saída.
 */
void buscar(NoTrie* raiz, const char* prefixo, std::ofstream& arquivoSaida) {
    NoTrie* noAtual = raiz;
    
    // 1. Navega até o nó que representa o fim do prefixo.
    for (int i = 0; i < strlen(prefixo); ++i) {
        int indice = prefixo[i] - 'a';
        if (indice < 0 || indice >= TAMANHO_ALFABETO || !noAtual->filhos[indice]) {
            // Se o caminho não existe, não há sugestões.
            arquivoSaida << prefixo << ":\n";
            return;
        }
        noAtual = noAtual->filhos[indice];
    }
    
    // 2. Inicia a busca recursiva a partir do nó encontrado.
    arquivoSaida << prefixo << ":";

    // Buffer para construir as palavras durante a busca recursiva.
    char bufferSugestao[TAMANHO_MAX_SUGESTAO];
    strcpy(bufferSugestao, prefixo);

    bool primeira = true;
    
    // A regra é que as sugestões devem ter ATÉ o dobro do tamanho do prefixo.
    int tamanhoMax = strlen(prefixo) * 2;
    if (tamanhoMax == 0) { // Caso de prefixo vazio, não há limite prático
        tamanhoMax = TAMANHO_MAX_SUGESTAO -1;
    }


    // Imprime um espaço inicial apenas se houver sugestões
    bool temSugestao = false;
    if (noAtual->fimDaPalavra) temSugestao = true;
    if (!temSugestao) {
        for(int i = 0; i < TAMANHO_ALFABETO; ++i) {
            if(noAtual->filhos[i]) {
                temSugestao = true;
                break;
            }
        }
    }
    if (temSugestao) {
        arquivoSaida << " ";
    }
    
    encontrarSugestoes(noAtual, bufferSugestao, tamanhoMax, arquivoSaida, primeira);

    arquivoSaida << "\n";
}

/**
 * @brief Função principal que orquestra a execução do programa.
 */
int main(int argc, char* argv[]) {
    // Validação dos argumentos de linha de comando
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>\n";
        return 1;
    }

    // Abertura dos arquivos
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

    // Leitura e inserção dos termos na árvore [cite: 4519]
    arquivoEntrada >> n_termos;
    for (int i = 0; i < n_termos; ++i) {
        arquivoEntrada >> buffer;
        inserir(raiz, buffer);
    }

    // Leitura das requisições e busca por sugestões [cite: 4521]
    arquivoEntrada >> n_requisicoes;
    for (int i = 0; i < n_requisicoes; ++i) {
        arquivoEntrada >> buffer;
        buscar(raiz, buffer, arquivoSaida);
    }

    // Fechamento dos arquivos
    arquivoEntrada.close();
    arquivoSaida.close();

    // Liberação da memória alocada para a árvore
    delete raiz;

    return 0;
}
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::ios_base;
using std::ofstream;
using std::stoi;
using std::string;

ifstream entrada;
ofstream saida;

constexpr int MAX_COMPRIMENTO_PALAVRA = 30;

struct No {
    char valor;
    bool ePalavra;
    uint32_t ultimoIdBusca;
    No* ponteiros[26];

    No(char letra, bool epal) {
        valor = letra;
        ePalavra = epal;
        ultimoIdBusca = 0;
        for (int i = 0; i < 26; i++) {
            ponteiros[i] = nullptr;
        }
    }
};

struct Trie {
    No* raiz;
    uint32_t idBuscaAtual;

    Trie() {
        raiz = new No(' ', false);
        idBuscaAtual = 1;
    }
    
    ~Trie() {
        // Implementar destrutor recursivo se houver preocupação com vazamento de memória
    }
    
    int32_t determinarIndice(const string& palavra, uint32_t posicao) {
        return palavra[posicao] - 'a';
    }

    void adicionar(const string& palavra) {
        if (!palavra.empty()) {
            adicionarPalavra(raiz, palavra, 0);
        }
    }

    void adicionarPalavra(No* noAtual, const string& palavra, uint32_t d) {
        if (d == palavra.length()) {
            noAtual->ePalavra = true;
            return;
        }
        int indice = determinarIndice(palavra, d);
        if (indice < 0 || indice >= 26) return;

        if (noAtual->ponteiros[indice] == nullptr) {
            noAtual->ponteiros[indice] = new No(palavra[d], false);
        }
        adicionarPalavra(noAtual->ponteiros[indice], palavra, d + 1);
    }

    void searchStart(const string& palavra) {
        string linhaResultado = palavra + ":";
        // --- OTIMIZAÇÃO 1: Pré-alocação de memória ---
        // Aloca 16KB de uma vez para evitar múltiplas realocações.
        // Ajuste este valor se as linhas de saída forem maiores.
        linhaResultado.reserve(16384);

        bool encontrouAlgo = false;
        char buffer[MAX_COMPRIMENTO_PALAVRA];
        No* atual = raiz;
        
        for (uint32_t i = 0; i < palavra.length(); ++i) {
            char c = palavra[i];
            int index = c - 'a';
            if (index < 0 || index >= 26 || !atual->ponteiros[index]) {
                break;
            }
            atual = atual->ponteiros[index];
            buffer[i] = c;
            int comprimentoMaximo = (i + 1) * 2;
            procurarNaTrie(atual, buffer, i + 1, comprimentoMaximo, linhaResultado, encontrouAlgo);
        }
        
        if (encontrouAlgo) {
            linhaResultado.pop_back();
        } else {
            linhaResultado += "-";
        }

        // --- OTIMIZAÇÃO 2: Usar '\n' em vez de endl ---
        // Evita forçar a descarga (flush) do buffer de saída a cada linha.
        saida << linhaResultado << '\n';
        idBuscaAtual++;
    }

private:
    void procurarNaTrie(No* subarvore, char* buffer, int profundidade, uint16_t comprimentoMaximo, string& linha, bool& encontrouAlgo) {
        if (profundidade > comprimentoMaximo) {
            return;
        }
        
        if (subarvore->ePalavra && subarvore->ultimoIdBusca != idBuscaAtual) {
            buffer[profundidade] = '\0';
            linha += buffer;
            linha += ",";
            subarvore->ultimoIdBusca = idBuscaAtual;
            encontrouAlgo = true;
        }

        if (profundidade >= MAX_COMPRIMENTO_PALAVRA - 1) {
            return;
        }

        for (int i = 0; i < 26; i++) {
            if (subarvore->ponteiros[i]) {
                No* filho = subarvore->ponteiros[i];
                buffer[profundidade] = filho->valor;
                procurarNaTrie(filho, buffer, profundidade + 1, comprimentoMaximo, linha, encontrouAlgo);
            }
        }
    }
};


// A função principal recebe os argumentos da linha de comando.
// argc: contém o número de argumentos passados para o programa.
// argv: é um array de strings (char*) contendo os argumentos.
// argv[0] é o nome do programa.
// argv[1] é o primeiro argumento (arquivo de entrada).
// argv[2] é o segundo argumento (arquivo de saída).
int main(int argc, char** argv) {
    // --- OTIMIZAÇÃO 3: Acelerar toda a Leitura/Escrita ---
    // Desativa a sincronização com os fluxos de E/S do C padrão para maior velocidade.
    ios_base::sync_with_stdio(false);
    // Desvincula `cin` de `cout` para evitar flushes desnecessários.
    entrada.tie(NULL);

    // Verifica se o número de argumentos é suficiente (nome do programa + arq. entrada + arq. saída).
    if (argc < 3) {
        cout << "Argumentos insuficientes. Uso: ./programa <arquivo_entrada> <arquivo_saida>" << endl;
        return EXIT_FAILURE;
    }

    // Abertura dos arquivos
    // O primeiro argumento (argv[1]) é o caminho para o arquivo de entrada.
    entrada.open(argv[1]);
    // O segundo argumento (argv[2]) é o caminho para o arquivo de saída.
    saida.open(argv[2]);

    Trie arvore;
    string linha;

    // Lê o número de palavras a serem adicionadas.
    getline(entrada, linha);
    int req_add = stoi(linha);
    // Lê cada palavra do arquivo de entrada e a adiciona na Trie.
    for (int i = 0; i < req_add; i++) {
        getline(entrada, linha);
        arvore.adicionar(linha);
    }

    // Lê o número de buscas a serem realizadas.
    getline(entrada, linha);
    int req_search = stoi(linha);
    // Lê cada prefixo de busca e executa a busca na Trie.
    for (int i = 0; i < req_search; i++) {
        getline(entrada, linha);
        arvore.searchStart(linha);
    }

    // Fecha os arquivos após o uso.
    entrada.close();
    saida.close();

    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Estrutura para armazenar os dados do arquivo
struct Arquivo {
    std::string nome;
    long long tamanho;
    std::string hash;
};

// Estrutura do nó da Árvore B
class No {
public:
    int t;      // Grau Mínimo da árvore
    bool folha;
    int n;      // Número atual de chaves
    Arquivo *chaves;
    No **filhos;

    No(int _t, bool _folha) {
        t = _t;
        folha = _folha;
        n = 0;
        chaves = new Arquivo[2 * t - 1];
        filhos = new No *[2 * t];
    }

    ~No() {
        delete[] chaves;
        delete[] filhos;
    }

    No* searchNode(std::string k) {
        int i = 0;
        while (i < n && k > chaves[i].hash) {
            i++;
        }
        if (i < n && k == chaves[i].hash) {
            return this;
        }
        if (folha) {
            return nullptr;
        }
        return filhos[i]->searchNode(k);
    }
    
    void insertNonFull(Arquivo arq) {
        int i = n - 1;
        if (folha) {
            while (i >= 0 && chaves[i].hash > arq.hash) {
                chaves[i + 1] = chaves[i];
                i--;
            }
            chaves[i + 1] = arq;
            n = n + 1;
        } else {
            while (i >= 0 && chaves[i].hash > arq.hash) {
                i--;
            }
            if (filhos[i + 1]->n == 2 * t - 1) {
                splitChild(i + 1, filhos[i + 1]);
                if (chaves[i + 1].hash < arq.hash) {
                    i++;
                }
            }
            filhos[i + 1]->insertNonFull(arq);
        }
    }

    void splitChild(int i, No *y) {
        No *z = new No(y->t, y->folha);
        z->n = t - 1;
        for (int j = 0; j < t - 1; j++) {
            z->chaves[j] = y->chaves[j + t];
        }
        if (!y->folha) {
            for (int j = 0; j < t; j++) {
                z->filhos[j] = y->filhos[j + t];
            }
        }
        y->n = t - 1;
        for (int j = n; j >= i + 1; j--) {
            filhos[j + 1] = filhos[j];
        }
        filhos[i + 1] = z;
        for (int j = n - 1; j >= i; j--) {
            chaves[j + 1] = chaves[j];
        }
        chaves[i] = y->chaves[t - 1];
        n = n + 1;
    }
};

// Classe da Árvore B
class ArvoreB {
public:
    No *raiz;
    int t; // Grau Mínimo

    ArvoreB(int _t) {
        raiz = nullptr;
        t = _t;
    }

    No* searchNode(std::string k) {
        return (raiz == nullptr) ? nullptr : raiz->searchNode(k);
    }

    void insert(Arquivo k) {
        if (raiz == nullptr) {
            raiz = new No(t, true);
            raiz->chaves[0] = k;
            raiz->n = 1;
        } else {
            if (raiz->n == 2 * t - 1) {
                No *s = new No(t, false);
                s->filhos[0] = raiz;
                s->splitChild(0, raiz);
                int i = 0;
                if (s->chaves[0].hash < k.hash) {
                    i++;
                }
                s->filhos[i]->insertNonFull(k);
                raiz = s;
            } else {
                raiz->insertNonFull(k);
            }
        }
    }
};

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    std::ofstream outputFile(argv[2]);

    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Erro ao abrir os arquivos." << std::endl;
        return 1;
    }

    int ordem_entrada, nArquivos, nOperacoes;
    std::string linha;

    std::getline(inputFile, linha);
    ordem_entrada = std::stoi(linha);
    
    int grau_minimo_t = ordem_entrada - 1;
    if (grau_minimo_t < 2) {
        grau_minimo_t = 2;
    }
    ArvoreB arvore(grau_minimo_t);

    std::getline(inputFile, linha);
    nArquivos = std::stoi(linha);

    for (int i = 0; i < nArquivos; ++i) {
        std::getline(inputFile, linha);
        if (linha.empty()) continue;
        std::stringstream ss(linha);
        Arquivo arq;
        ss >> arq.nome >> arq.tamanho >> arq.hash;
        arvore.insert(arq);
    }
    
    std::getline(inputFile, linha);
    nOperacoes = std::stoi(linha);

    bool primeiraSaida = true; // Flag para controlar a primeira escrita no arquivo

    for (int i = 0; i < nOperacoes; ++i) {
        std::getline(inputFile, linha);
        if (linha.empty()) continue;
        
        std::stringstream ss(linha);
        std::string operacao;
        ss >> operacao;

        if (operacao == "INSERT") {
            Arquivo arq;
            ss >> arq.nome >> arq.tamanho >> arq.hash;
            arvore.insert(arq);
        } else if (operacao == "SELECT") {
            // Se não for a primeira vez que escrevemos no arquivo,
            // adiciona uma quebra de linha ANTES.
            if (!primeiraSaida) {
                outputFile << "\n";
            }

            std::string hashBusca;
            ss >> hashBusca;
            
            No* noResultado = arvore.searchNode(hashBusca);
            
            outputFile << "[" << hashBusca << "]\n";
            
            if (noResultado != nullptr) {
                for (int j = 0; j < noResultado->n; j++) {
                    Arquivo arq = noResultado->chaves[j];
                    outputFile << arq.nome << ":size=" << arq.tamanho << ",hash=" << arq.hash;
                    if (j < noResultado->n - 1) {
                         outputFile << "\n";
                    }
                }
            } else {
                outputFile << "-";
            }
            primeiraSaida = false; // Marca que já escrevemos no arquivo.
        }
    }

    inputFile.close();
    outputFile.close();

    return 0;
}
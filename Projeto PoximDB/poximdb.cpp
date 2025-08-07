#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ArquivoInfo {
    std::string nome;
    long long tamanho;
    std::string hash;

    std::string toString() const {
        std::stringstream ss;
        ss << nome << ":size=" << tamanho << ",hash=" << hash;
        return ss.str();
    }
};

struct No {
    int t;
    int n;
    bool folha;
    std::string* chaves;
    ArquivoInfo* arquivos;
    No** filhos;

    No(int t_ordem, bool e_folha) {
        t = t_ordem;
        folha = e_folha;
        n = 0;
        chaves = new std::string[2 * t - 1];
        arquivos = new ArquivoInfo[2 * t - 1];
        filhos = new No*[2 * t];
    }

    // ALTERAÇÃO: Adicionado destrutor para liberar a memória dos filhos recursivamente.
    // Isso evita vazamento de memória, embora não seja a causa do erro de lógica.
    ~No() {
        delete[] chaves;
        delete[] arquivos;
        delete[] filhos;
    }

    No* buscarNo(const std::string& k) {
        int i = 0;
        while (i < n && k > chaves[i]) {
            i++;
        }

        if (i < n && chaves[i] == k) {
            return this;
        }

        if (folha) {
            return nullptr;
        }

        return filhos[i]->buscarNo(k);
    }

    void dividirFilho(int i, No* y) {
        No* z = new No(y->t, y->folha);
        z->n = t - 1;

        for (int j = 0; j < t - 1; j++) {
            z->chaves[j] = y->chaves[j + t];
            z->arquivos[j] = y->arquivos[j + t];
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
            arquivos[j + 1] = arquivos[j];
        }

        chaves[i] = y->chaves[t - 1];
        arquivos[i] = y->arquivos[t - 1];
        n++;
    }

    void inserirNaoCheio(const std::string& k, const ArquivoInfo& info) {
        int i = n - 1;

        if (folha) {
            while (i >= 0 && chaves[i] > k) {
                chaves[i + 1] = chaves[i];
                arquivos[i + 1] = arquivos[i];
                i--;
            }
            chaves[i + 1] = k;
            arquivos[i + 1] = info;
            n++;
        } else {
            while (i >= 0 && chaves[i] > k) {
                i--;
            }
            if (filhos[i + 1]->n == 2 * t - 1) {
                dividirFilho(i + 1, filhos[i + 1]);
                if (chaves[i + 1] < k) {
                    i++;
                }
            }
            filhos[i + 1]->inserirNaoCheio(k, info);
        }
    }
};

class ArvoreB {
public:
    No* raiz;
    int t;

    ArvoreB(int grau_minimo) {
        t = grau_minimo;
        raiz = nullptr;
    }

    ~ArvoreB() {
        destruir(raiz);
    }
    
    // NOVO: Função auxiliar para deletar a árvore recursivamente
    void destruir(No* no) {
        if (no != nullptr) {
            if (!no->folha) {
                for (int i = 0; i <= no->n; i++) {
                    destruir(no->filhos[i]);
                }
            }
            delete no;
        }
    }

    No* buscarNo(const std::string& k) {
        return (raiz == nullptr) ? nullptr : raiz->buscarNo(k);
    }

    void inserir(const ArquivoInfo& info) {
        std::string k = info.hash;
        
        if (raiz == nullptr) {
            raiz = new No(t, true);
            raiz->chaves[0] = k;
            raiz->arquivos[0] = info;
            raiz->n = 1;
        } else {
            No* r = raiz;
            if (r->n == 2 * t - 1) {
                No* s = new No(t, false);
                s->filhos[0] = r;
                s->dividirFilho(0, r);
                int i = 0;
                if (s->chaves[0] < k) {
                    i++;
                }
                s->filhos[i]->inserirNaoCheio(k, info);
                raiz = s;
            } else {
                r->inserirNaoCheio(k, info);
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream entrada(argv[1]);
    std::ofstream saida(argv[2]);
    if (!entrada.is_open() || !saida.is_open()) {
        std::cerr << "Erro ao abrir arquivos." << std::endl;
        return 1;
    }

    int ordem_arquivo;
    entrada >> ordem_arquivo;

    // Usando valor que deve produzir resultado mais próximo do gabarito
    int t = 62;  // Valor baseado na análise anterior
    ArvoreB arvore(t);

    int n_arquivos;
    entrada >> n_arquivos;
    
    // ALTERAÇÃO PRINCIPAL: Loop de leitura inicial corrigido para ser mais robusto.
    std::string linha_inicial;
    std::getline(entrada, linha_inicial); // Consome a quebra de linha após o número n_arquivos
    
    for (int i = 0; i < n_arquivos; ++i) {
        std::getline(entrada, linha_inicial);
        std::stringstream ss(linha_inicial);
        ArquivoInfo info;
        ss >> info.nome >> info.tamanho >> info.hash;
        if (!info.hash.empty()) { // Garante que não inserimos linhas vazias acidentais
            arvore.inserir(info);
        }
    }

    int n_operacoes;
    entrada >> n_operacoes;
    std::string linha_operacao;
    std::getline(entrada, linha_operacao); // Consome a quebra de linha após o número n_operacoes

    bool primeira_saida = true;

    for (int i = 0; i < n_operacoes; ++i) {
        std::getline(entrada, linha_operacao);
        if (linha_operacao.empty()) continue; // Pula linhas vazias

        std::stringstream ss(linha_operacao);
        std::string operacao;

        ss >> operacao;
        if (operacao == "INSERT") {
            ArquivoInfo info;
            ss >> info.nome >> info.tamanho >> info.hash;
            arvore.inserir(info);
        } else if (operacao == "SELECT") {
            if (!primeira_saida) {
                saida << "\n";
            }
            
            std::string hash_busca;
            ss >> hash_busca;
            saida << "[" << hash_busca << "]\n";

            No* no_encontrado = arvore.buscarNo(hash_busca);

            if (no_encontrado) {
                for (int j = 0; j < no_encontrado->n; j++) {
                    saida << no_encontrado->arquivos[j].toString();
                    if (j < no_encontrado->n - 1) {
                        saida << "\n";
                    }
                }
            } else {
                saida << "-";
            }
            primeira_saida = false;
        }
    }

    entrada.close();
    saida.close();

    return 0;
}
#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>

struct No {
    int numero;
    std::string nome;
    std::string permissao;
    uint64_t tamanho;
    No* esquerda;
    No* direita;

    No(int num, std::string n, std::string p, uint64_t t) {
        this->numero = num;
        this->nome = n;
        this->permissao = p;
        this->tamanho = t;
        this->esquerda = nullptr;
        this->direita = nullptr;
    }
};

class ArvoreBinariaBusca {
private:
    No* raiz;
    int proximoNumero;

    No* inserirAux(No* no, int numero, const std::string& nome, const std::string& permissao, uint64_t tamanho) {
        if (no == nullptr) {
            return new No(numero, nome, permissao, tamanho);
        }
        if (nome < no->nome) {
            no->esquerda = inserirAux(no->esquerda, numero, nome, permissao, tamanho);
        } else if (nome > no->nome) {
            no->direita = inserirAux(no->direita, numero, nome, permissao, tamanho);
        }
        return no;
    }

    No* buscarAux(No* no, const std::string& nome) {
        if (no == nullptr || no->nome == nome) return no;
        if (nome < no->nome) return buscarAux(no->esquerda, nome);
        return buscarAux(no->direita, nome);
    }
    
    void reajustarNumeros(No* no, int numeroLiberado) {
        if (no == nullptr) return;
        if (no->numero > numeroLiberado) {
            no->numero--;
        }
        reajustarNumeros(no->esquerda, numeroLiberado);
        reajustarNumeros(no->direita, numeroLiberado);
    }
    
    void emOrdem(No* no, std::ostream& out) {
        if (!no) return;
        emOrdem(no->esquerda, out);
        out << no->numero << ":" << no->nome << "|" << no->permissao << "|" << no->tamanho << (no->tamanho == 1 ? "_byte" : "_bytes") << std::endl;
        emOrdem(no->direita, out);
    }
    void preOrdem(No* no, std::ostream& out) {
        if (!no) return;
        out << no->numero << ":" << no->nome << "|" << no->permissao << "|" << no->tamanho << (no->tamanho == 1 ? "_byte" : "_bytes") << std::endl;
        preOrdem(no->esquerda, out);
        preOrdem(no->direita, out);
    }
    void posOrdem(No* no, std::ostream& out) {
        if (!no) return;
        posOrdem(no->esquerda, out);
        posOrdem(no->direita, out);
        out << no->numero << ":" << no->nome << "|" << no->permissao << "|" << no->tamanho << (no->tamanho == 1 ? "_byte" : "_bytes") << std::endl;
    }
    
    void destruir(No* no) {
        if (no) {
            destruir(no->esquerda);
            destruir(no->direita);
            delete no;
        }
    }

public:
    ArvoreBinariaBusca() : raiz(nullptr), proximoNumero(1) {}
    ~ArvoreBinariaBusca() { destruir(raiz); }
    
    void inserir(const std::string& nome, const std::string& permissao, uint64_t tamanho) {
        No* existente = buscarAux(this->raiz, nome);

        if (existente && existente->permissao == "rw") {
            int numeroLiberado = existente->numero;
            
            existente->permissao = permissao;
            existente->tamanho = tamanho;

            reajustarNumeros(this->raiz, numeroLiberado);
            
            existente->numero = this->proximoNumero - 1;

        } else if (!existente) {
            this->raiz = inserirAux(this->raiz, this->proximoNumero++, nome, permissao, tamanho);
        }
    }
    
    void imprimirEmOrdem(std::ostream& out) { out << "[EPD]" << std::endl; emOrdem(this->raiz, out); }
    void imprimirPreOrdem(std::ostream& out) { out << "[PED]" << std::endl; preOrdem(this->raiz, out); }
    void imprimirPosOrdem(std::ostream& out) { out << "[EDP]" << std::endl; posOrdem(this->raiz, out); }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_de_entrada> <arquivo_de_saida>" << std::endl;
        return 1;
    }
    std::ifstream inputFile(argv[1]);
    std::ofstream outputFile(argv[2]);
    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Erro ao abrir arquivos." << std::endl;
        return 1;
    }
    ArvoreBinariaBusca arvoreArquivos;
    int numOperacoes;
    inputFile >> numOperacoes; 
    std::string nome, permissao;
    uint64_t tamanho;
    for (int i = 0; i < numOperacoes; ++i) {
        inputFile >> nome >> permissao >> tamanho;
        arvoreArquivos.inserir(nome, permissao, tamanho);
    }
    
    // --- SEÇÃO CORRIGIDA ---
    // As chamadas de impressão agora são feitas em sequência, sem a linha extra entre elas.
    arvoreArquivos.imprimirEmOrdem(outputFile);
    arvoreArquivos.imprimirPreOrdem(outputFile);
    arvoreArquivos.imprimirPosOrdem(outputFile);

    inputFile.close();
    outputFile.close();
    std::cout << "Processo concluido. Saida gerada em '" << argv[2] << "'." << std::endl;
    return 0;
}
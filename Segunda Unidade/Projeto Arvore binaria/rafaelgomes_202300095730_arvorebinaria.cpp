#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct No {
    int id;
    string nome;
    string tipo;
    int tamanho;
    No* esquerda;
    No* direita;

    No(int id, string nome, string tipo, int tamanho) {
        this->id = id;
        this->nome = nome;
        this->tipo = tipo;
        this->tamanho = tamanho;
        this->esquerda = nullptr;
        this->direita = nullptr;
    }
};

class Arvore {
private:
    No* raiz;
    int proximo_id;

    void destruir_no(No* no) {
        if (no) {
            destruir_no(no->esquerda);
            destruir_no(no->direita);
            delete no;
        }
    }

    No* inserir_no(No* no, int id, string nome, string tipo, int tamanho) {
        if (no == nullptr) {
            return new No(id, nome, tipo, tamanho);
        }
        if (nome < no->nome) {
            no->esquerda = inserir_no(no->esquerda, id, nome, tipo, tamanho);
        } else {
            no->direita = inserir_no(no->direita, id, nome, tipo, tamanho);
        }
        return no;
    }

    No* buscar(No* no, string nome) {
        if (no == nullptr || no->nome == nome) {
            return no;
        }
        if (nome < no->nome) {
            return buscar(no->esquerda, nome);
        }
        return buscar(no->direita, nome);
    }

    void atualizar_id(No* no, int id_removido) {
        if (no == nullptr) return;
        if (no->id > id_removido) {
            no->id--;
        }
        atualizar_id(no->esquerda, id_removido);
        atualizar_id(no->direita, id_removido);
    }

    void em_ordem(No* no, ostream& out) {
        if (no == nullptr) return;
        em_ordem(no->esquerda, out);
        out << no->id << ":" << no->nome << "|" << no->tipo << "|" << no->tamanho << (no->tamanho == 1 ? "_byte" : "_bytes") << endl;
        em_ordem(no->direita, out);
    }

    void pre_ordem(No* no, ostream& out) {
        if (no == nullptr) return;
        out << no->id << ":" << no->nome << "|" << no->tipo << "|" << no->tamanho << (no->tamanho == 1 ? "_byte" : "_bytes") << endl;
        pre_ordem(no->esquerda, out);
        pre_ordem(no->direita, out);
    }
    
    void pos_ordem(No* no, ostream& out) {
        if (no == nullptr) return;
        pos_ordem(no->esquerda, out);
        pos_ordem(no->direita, out);
        out << no->id << ":" << no->nome << "|" << no->tipo << "|" << no->tamanho << (no->tamanho == 1 ? "_byte" : "_bytes") << endl;
    }

public:
    // CORREÇÃO 1: Garantir que o contador de ID começa em 0.
    Arvore() : raiz(nullptr), proximo_id(0) {}
    
    ~Arvore() {
        destruir_no(this->raiz);
    }

// Esta é a lógica correta que deve estar no seu método inserir
void inserir(string nome, string tipo, int tamanho) {
    No* no = buscar(this->raiz, nome);

    // Este bloco IF é o que executa a lógica correta de atualização
    if (no != nullptr && no->tipo == "rw") {
        int id_antigo = no->id;
        
        // 1. Atualiza os dados no lugar
        no->tipo = tipo;
        no->tamanho = tamanho;
        
        // 2. Reajusta os IDs de todos os outros nós
        atualizar_id(this->raiz, id_antigo);
        
        // 3. Atribui o novo ID mais alto ao nó atualizado
        no->id = this->proximo_id - 1;

        // 4. Corrige o contador para a próxima inserção
        this->proximo_id--; 

    } else if (no == nullptr) {
        // Bloco para inserir um nó novo
        this->raiz = inserir_no(this->raiz, this->proximo_id, nome, tipo, tamanho);
        this->proximo_id++;
    }
}

    void imprimir_em_ordem(ostream& out) {
        out << "[EPD]" << endl;
        em_ordem(this->raiz, out);
    }
    void imprimir_pre_ordem(ostream& out) {
        out << "[PED]" << endl;
        pre_ordem(this->raiz, out);
    }
    void imprimir_pos_ordem(ostream& out) {
        out << "[EDP]" << endl;
        pos_ordem(this->raiz, out);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        // Mensagem de erro para o usuário caso não passe os argumentos corretos
        cerr << "Uso: " << argv[0] << " <arquivo_de_entrada> <arquivo_de_saida>" << endl;
        return 1;
    }
    ifstream inputFile(argv[1]);
    ofstream outputFile(argv[2]);
    if (!inputFile.is_open() || !outputFile.is_open()) {
        cerr << "Erro ao abrir arquivos." << endl;
        return 1;
    }

    Arvore arvore;
    int n;
    inputFile >> n;
    string nome, tipo;
    int tamanho;

    for (int i = 0; i < n; ++i) {
        inputFile >> nome >> tipo >> tamanho;
        arvore.inserir(nome, tipo, tamanho);
    }

    arvore.imprimir_em_ordem(outputFile);
    arvore.imprimir_pre_ordem(outputFile);
    arvore.imprimir_pos_ordem(outputFile);

    inputFile.close();
    outputFile.close();

    // Adiciona uma mensagem no terminal para avisar que terminou.
    cout << "Processo concluido. Saida gerada em '" << argv[2] << "'." << endl;

    return 0;
}
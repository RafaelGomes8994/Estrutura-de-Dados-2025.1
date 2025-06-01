#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cctype>

using namespace std;

// Estrutura que representa um usuário na lista duplamente ligada circular
struct Usuario {
    string nome;
    Usuario* anterior;
    Usuario* proximo;
    Usuario(const string& nome) : nome(nome), anterior(nullptr), proximo(nullptr) {}
};

// Estrutura para lista encadeada na tabela hash
struct NoHash {
    string chave;
    Usuario* usuario;
    NoHash* prox;
    NoHash(const string& chave, Usuario* usuario) : chave(chave), usuario(usuario), prox(nullptr) {}
};

// Tabela hash simples
const int TAM_HASH = 1031; // Primo para melhor distribuição
NoHash* tabela_hash[TAM_HASH] = {nullptr};

// Ponteiro para o último usuário inserido na lista
Usuario* ultimo = nullptr;

// Função de hash simples para strings
unsigned int hash_string(const string& s) {
    unsigned int h = 0;
    for (size_t i = 0; i < s.size(); ++i)
        h = h * 31 + (unsigned char)s[i];
    return h % TAM_HASH;
}

// Busca usuário na tabela hash
Usuario* buscar_usuario(const string& nome) {
    unsigned int h = hash_string(nome);
    NoHash* no = tabela_hash[h];
    while (no) {
        if (no->chave == nome) return no->usuario;
        no = no->prox;
    }
    return nullptr;
}

// Insere usuário na tabela hash
void inserir_usuario(const string& nome, Usuario* usuario) {
    unsigned int h = hash_string(nome);
    NoHash* novo = new NoHash(nome, usuario);
    novo->prox = tabela_hash[h];
    tabela_hash[h] = novo;
}

// Remove usuário da tabela hash
void remover_usuario(const string& nome) {
    unsigned int h = hash_string(nome);
    NoHash* no = tabela_hash[h];
    NoHash* ant = nullptr;
    while (no) {
        if (no->chave == nome) {
            if (ant) ant->prox = no->prox;
            else tabela_hash[h] = no->prox;
            delete no;
            return;
        }
        ant = no;
        no = no->prox;
    }
}

// Função para substituir espaços por underscores em uma string
string substituir_espacos(const string& s) {
    string resultado = s;
    for (size_t i = 0; i < resultado.size(); ++i) {
        if (resultado[i] == ' ') resultado[i] = '_';
    }
    return resultado;
}

// Função para remover espaços em branco do início e fim de uma string
string aparar(const string &s) {
    size_t inicio = 0;
    while (inicio < s.size() && (s[inicio] == ' ' || s[inicio] == '\t')) ++inicio;
    if (inicio == s.size()) return "";
    size_t fim = s.size() - 1;
    while (fim > inicio && (s[fim] == ' ' || s[fim] == '\t')) --fim;
    return s.substr(inicio, fim - inicio + 1);
}

// Função que processa os comandos de entrada
void processar_comando(const string& linha) {
    istringstream iss(linha);
    string comando;
    iss >> comando;
    size_t pos_cmd = linha.find(comando);
    string parte_nome = linha.substr(pos_cmd + comando.length());
    string nome = aparar(parte_nome);

    if (comando == "ADD") {
        if (buscar_usuario(nome)) {
            cout << "[FAILURE]ADD=" << substituir_espacos(nome) << endl;
        } else {
            Usuario* novo_usuario = new Usuario(nome);
            if (ultimo == nullptr) {
                novo_usuario->anterior = novo_usuario;
                novo_usuario->proximo = novo_usuario;
                ultimo = novo_usuario;
            } else {
                novo_usuario->anterior = ultimo;
                novo_usuario->proximo = ultimo->proximo;
                ultimo->proximo->anterior = novo_usuario;
                ultimo->proximo = novo_usuario;
                ultimo = novo_usuario;
            }
            inserir_usuario(nome, novo_usuario);
            cout << "[SUCCESS]ADD=" << substituir_espacos(nome) << endl;
        }
    } else if (comando == "REMOVE") {
        Usuario* usuario = buscar_usuario(nome);
        if (!usuario) {
            cout << "[FAILURE]REMOVE=" << substituir_espacos(nome) << endl;
        } else {
            Usuario* anterior_usuario = usuario->anterior;
            Usuario* proximo_usuario = usuario->proximo;
            anterior_usuario->proximo = proximo_usuario;
            proximo_usuario->anterior = anterior_usuario;
            if (ultimo == usuario) {
                ultimo = (anterior_usuario == usuario) ? nullptr : anterior_usuario;
            }
            remover_usuario(nome);
            delete usuario;
            cout << "[SUCCESS]REMOVE=" << substituir_espacos(nome) << endl;
        }
    } else if (comando == "SHOW") {
        Usuario* usuario = buscar_usuario(nome);
        if (!usuario) {
            cout << "[FAILURE]SHOW=?<-" << substituir_espacos(nome) << "->?" << endl;
        } else {
            string nome_anterior = usuario->anterior->nome;
            string nome_proximo = usuario->proximo->nome;
            cout << "[SUCCESS]SHOW=" << substituir_espacos(nome_anterior) << "<-"
                 << substituir_espacos(usuario->nome) << "->" << substituir_espacos(nome_proximo) << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <entrada> <saida>" << endl;
        return 1;
    }
    ifstream entrada(argv[1]);
    ofstream saida(argv[2]);
    if (!entrada.is_open() || !saida.is_open()) {
        cerr << "Erro ao abrir arquivos." << endl;
        return 1;
    }
    streambuf* cout_buf = cout.rdbuf();
    cout.rdbuf(saida.rdbuf());

    string linha;
    while (getline(entrada, linha)) {
        if (linha.empty()) continue;
        processar_comando(linha);
    }

    // Libera a memória dos usuários criados e da tabela hash
    for (int i = 0; i < TAM_HASH; ++i) {
        NoHash* no = tabela_hash[i];
        while (no) {
            delete no->usuario;
            NoHash* prox = no->prox;
            delete no;
            no = prox;
        }
    }

    cout.rdbuf(cout_buf);
    return 0;
}

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

// Ponteiro para o último usuário inserido na lista
Usuario* ultimo = nullptr;

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

// Busca usuário na lista circular
Usuario* buscar_usuario(const string& nome) {
    if (!ultimo) return nullptr;
    Usuario* atual = ultimo;
    do {
        if (atual->nome == nome) return atual;
        atual = atual->proximo;
    } while (atual != ultimo);
    return nullptr;
}

// Remove usuário da lista circular
void remover_usuario_lista(Usuario* usuario) {
    if (!usuario) return;
    if (usuario->proximo == usuario) {
        // Só há um usuário na lista
        ultimo = nullptr;
    } else {
        usuario->anterior->proximo = usuario->proximo;
        usuario->proximo->anterior = usuario->anterior;
        if (ultimo == usuario)
            ultimo = usuario->anterior;
    }
    delete usuario;
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
            cout << "[SUCCESS]ADD=" << substituir_espacos(nome) << endl;
        }
    } else if (comando == "REMOVE") {
        Usuario* usuario = buscar_usuario(nome);
        if (!usuario) {
            cout << "[FAILURE]REMOVE=" << substituir_espacos(nome) << endl;
        } else {
            remover_usuario_lista(usuario);
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

// Libera todos os usuários da lista circular
void liberar_lista() {
    if (!ultimo) return;
    Usuario* atual = ultimo->proximo;
    while (atual != ultimo) {
        Usuario* prox = atual->proximo;
        delete atual;
        atual = prox;
    }
    delete ultimo;
    ultimo = nullptr;
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

    liberar_lista();

    cout.rdbuf(cout_buf);
    return 0;
}
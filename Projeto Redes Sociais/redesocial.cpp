#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <algorithm>
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

// Mapa para acesso rápido aos usuários pelo nome
unordered_map<string, Usuario*> mapaUsuarios;
// Ponteiro para o último usuário inserido na lista
Usuario* ultimo = nullptr;

// Função para substituir espaços por underscores em uma string
string substituir_espacos(const string& s) {
    string resultado = s;
    replace(resultado.begin(), resultado.end(), ' ', '_');
    return resultado;
}

// Função para remover espaços em branco do início e fim de uma string
string aparar(const string &s) {
    size_t inicio = s.find_first_not_of(" \t");
    if (inicio == string::npos) return "";
    size_t fim = s.find_last_not_of(" \t");
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
        // Adiciona um novo usuário se ele não existir
        if (mapaUsuarios.find(nome) != mapaUsuarios.end()) {
            cout << "[FAILURE]ADD=" << substituir_espacos(nome) << endl;
        } else {
            Usuario* novo_usuario = new Usuario(nome);
            if (ultimo == nullptr) {
                // Primeiro usuário: aponta para si mesmo
                novo_usuario->anterior = novo_usuario;
                novo_usuario->proximo = novo_usuario;
                ultimo = novo_usuario;
            } else {
                // Insere novo usuário após o último
                novo_usuario->anterior = ultimo;
                novo_usuario->proximo = ultimo->proximo;
                ultimo->proximo->anterior = novo_usuario;
                ultimo->proximo = novo_usuario;
                ultimo = novo_usuario;
            }
            mapaUsuarios[nome] = novo_usuario;
            cout << "[SUCCESS]ADD=" << substituir_espacos(nome) << endl;
        }
    } else if (comando == "REMOVE") {
        // Remove um usuário se ele existir
        if (mapaUsuarios.find(nome) == mapaUsuarios.end()) {
            cout << "[FAILURE]REMOVE=" << substituir_espacos(nome) << endl;
        } else {
            Usuario* usuario = mapaUsuarios[nome];
            Usuario* anterior_usuario = usuario->anterior;
            Usuario* proximo_usuario = usuario->proximo;

            anterior_usuario->proximo = proximo_usuario;
            proximo_usuario->anterior = anterior_usuario;

            // Atualiza o ponteiro 'ultimo' se necessário
            if (ultimo == usuario) {
                ultimo = (anterior_usuario == usuario) ? nullptr : anterior_usuario;
            }

            mapaUsuarios.erase(nome);
            delete usuario;
            cout << "[SUCCESS]REMOVE=" << substituir_espacos(nome) << endl;
        }
    } else if (comando == "SHOW") {
        // Mostra o usuário e seus vizinhos na lista
        auto it = mapaUsuarios.find(nome);
        if (it == mapaUsuarios.end()) {
            cout << "[FAILURE]SHOW=?<-" << substituir_espacos(nome) << "->?" << endl;
        } else {
            Usuario* usuario = it->second;
            string nome_anterior = usuario->anterior->nome;
            string nome_proximo = usuario->proximo->nome;
            cout << "[SUCCESS]SHOW=" << substituir_espacos(nome_anterior) << "<-" 
                 << substituir_espacos(usuario->nome) << "->" << substituir_espacos(nome_proximo) << endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // Verifica se o número de argumentos está correto (programa, entrada, saída)
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " <entrada> <saida>" << endl;
        return 1;
    }

    // Abre o arquivo de entrada para leitura e o de saída para escrita
    ifstream entrada(argv[1]);
    ofstream saida(argv[2]);

    // Verifica se os arquivos foram abertos corretamente
    if (!entrada.is_open() || !saida.is_open()) {
        cerr << "Erro ao abrir arquivos." << endl;
        return 1;
    }

    // Redireciona o cout para o arquivo de saída
    streambuf* cout_buf = cout.rdbuf();
    cout.rdbuf(saida.rdbuf());

    string linha;
    // Lê cada linha do arquivo de entrada e processa o comando
    while (getline(entrada, linha)) {
        if (linha.empty()) continue; // Ignora linhas vazias
        processar_comando(linha);
    }

    // Libera a memória dos usuários criados
    for (auto& par : mapaUsuarios) {
        delete par.second;
    }

    // Restaura o cout para o console padrão
    cout.rdbuf(cout_buf);

    return 0;
}

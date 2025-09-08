#include <iostream>
#include <fstream>
#include <string>
#include <sstream> // Usada para facilitar o parsing da entrada

// --- Estrutura de Dados ---
// Estrutura do nó (Pessoa), baseada na lógica de lista duplamente encadeada.
// Cada pessoa tem um ponteiro para o amigo anterior e o próximo no círculo.
struct Pessoa {
    std::string nome;
    Pessoa* proximo;
    Pessoa* anterior;

    // Construtor para facilitar a criação de uma nova pessoa.
    Pessoa(const std::string& n) : nome(n), proximo(nullptr), anterior(nullptr) {}
};

// --- Classe de Gerenciamento ---
// Classe que gerencia a nossa lista circular, implementando as operações da rede social.
class RedeSocial {
private:
    Pessoa* cabeca; // Ponteiro para um elemento qualquer do círculo. Serve como ponto de entrada.
    int tamanho;    // Mantém o controle do número de pessoas na rede.

public:
    // Construtor: inicializa uma rede vazia.
    RedeSocial() : cabeca(nullptr), tamanho(0) {}

    // Destrutor: libera toda a memória alocada para evitar memory leaks.
    ~RedeSocial() {
        while (tamanho > 0) {
            remover(cabeca->nome);
        }
    }

    // Função para buscar uma pessoa na rede pelo nome.
    // Complexidade: O(n)
    Pessoa* buscar(const std::string& nome) {
        if (!cabeca) {
            return nullptr; // A rede está vazia.
        }
        Pessoa* atual = cabeca;
        do {
            if (atual->nome == nome) {
                return atual; // Encontrou a pessoa.
            }
            atual = atual->proximo;
        } while (atual != cabeca); // Para quando der a volta completa no círculo.
        
        return nullptr; // Não encontrou.
    }

    // Função para adicionar uma nova pessoa ao círculo.
    // Complexidade: O(n) devido à busca de verificação.
    bool adicionar(const std::string& nome) {
        if (buscar(nome) != nullptr) {
            return false; // Falha: a pessoa já existe na rede.
        }

        Pessoa* novaPessoa = new Pessoa(nome);
        
        if (!cabeca) { // Caso especial: adicionando a primeira pessoa.
            cabeca = novaPessoa;
            novaPessoa->proximo = novaPessoa; // Aponta para si mesmo.
            novaPessoa->anterior = novaPessoa;
        } else { // Adicionando em um círculo já existente.
            Pessoa* cauda = cabeca->anterior; // O último elemento do círculo.
            
            // Refazendo os "apertos de mão".
            cauda->proximo = novaPessoa;
            novaPessoa->anterior = cauda;
            novaPessoa->proximo = cabeca;
            cabeca->anterior = novaPessoa;
        }
        tamanho++;
        return true;
    }

    // Função para remover uma pessoa do círculo.
    // Complexidade: O(n) devido à busca.
    bool remover(const std::string& nome) {
        Pessoa* pessoaParaRemover = buscar(nome);
        if (!pessoaParaRemover) {
            return false; // Falha: a pessoa não foi encontrada.
        }

        if (tamanho == 1) { // Caso especial: removendo a única pessoa.
            cabeca = nullptr;
        } else {
            Pessoa* amigoAnterior = pessoaParaRemover->anterior;
            Pessoa* amigoProximo = pessoaParaRemover->proximo;

            // Os amigos do usuário removido agora são amigos entre si.
            amigoAnterior->proximo = amigoProximo;
            amigoProximo->anterior = amigoAnterior;

            // Se removemos a "cabeça", precisamos de um novo ponto de entrada.
            if (pessoaParaRemover == cabeca) {
                cabeca = amigoProximo;
            }
        }
        
        delete pessoaParaRemover; // Libera a memória.
        tamanho--;
        return true;
    }

    // Função para formatar o nome para o padrão de saída (substitui espaços por '_').
    std::string formatarNome(const std::string& nome) {
        std::string formatado = nome;
        for (char& ch : formatado) {
            if (ch == ' ') {
                ch = '_';
            }
        }
        return formatado;
    }
    
    // Função que executa o comando SHOW e escreve no arquivo de saída.
    void mostrarAmigos(const std::string& nome, std::ofstream& outFile) {
        Pessoa* pessoa = buscar(nome);
        std::string nomeFormatado = formatarNome(nome);

        if (pessoa) {
            std::string amigoAnterior = formatarNome(pessoa->anterior->nome);
            std::string amigoProximo = formatarNome(pessoa->proximo->nome);
            // Saída formatada sem espaço após ']'
            outFile << "[SUCCESS]SHOW=" << amigoAnterior << "<-" << nomeFormatado << "->" << amigoProximo << std::endl;
        } else {
            // Saída formatada sem espaço após ']'
            outFile << "[FAILURE]SHOW=?<-" << nomeFormatado << "->?" << std::endl;
        }
    }
};

// --- Função Principal ---
// Responsável por ler os argumentos (arquivos), processar os comandos e chamar as funções da classe.
int main(int argc, char* argv[]) {
    // Validação dos argumentos de linha de comando (arg1 e arg2).
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream inFile(argv[1]);
    std::ofstream outFile(argv[2]);

    if (!inFile.is_open() || !outFile.is_open()) {
        std::cerr << "Erro ao abrir os arquivos de entrada/saida." << std::endl;
        return 1;
    }

    RedeSocial rede;
    std::string linha, comando, nome;

    // Loop para ler cada linha do arquivo de entrada.
    while (std::getline(inFile, linha)) {
        if (linha.empty()) continue; // Ignora linhas em branco

        std::stringstream ss(linha);
        ss >> comando; // Extrai a primeira palavra (o comando).
        
        // Pega o resto da linha como o nome, que pode conter espaços.
        std::getline(ss, nome);
        
        // Remove o espaço em branco inicial que o getline deixa.
        if (!nome.empty() && nome[0] == ' ') {
            nome = nome.substr(1);
        }

        // Executa a operação correspondente ao comando.
        if (comando == "ADD") {
            if (rede.adicionar(nome)) {
                // Saída formatada sem espaço após ']'
                outFile << "[SUCCESS]ADD=" << rede.formatarNome(nome) << std::endl;
            } else {
                // Saída formatada sem espaço após ']'
                outFile << "[FAILURE]ADD=" << rede.formatarNome(nome) << std::endl;
            }
        } else if (comando == "REMOVE") {
            if (rede.remover(nome)) {
                // Saída formatada sem espaço após ']'
                outFile << "[SUCCESS]REMOVE=" << rede.formatarNome(nome) << std::endl;
            } else {
                // Saída formatada sem espaço após ']'
                outFile << "[FAILURE]REMOVE=" << rede.formatarNome(nome) << std::endl;
            }
        } else if (comando == "SHOW") {
            rede.mostrarAmigos(nome, outFile);
        }
    }

    inFile.close();
    outFile.close();

    return 0;
}
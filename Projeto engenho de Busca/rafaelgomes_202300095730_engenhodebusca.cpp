#include <iostream>
#include <string>
#include <fstream>

// Função para calcular o checksum de uma string usando XOR
int checksum(const std::string& texto) {
    int chk = 0;
    for (char c : texto) {
        chk = chk ^ c;
    }
    return chk;
}

int main(int argc, char* argv[]) {
    // Verifica se os argumentos de entrada estão corretos
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    std::ofstream outputFile(argv[2]);

    // Verifica se os arquivos foram abertos corretamente
    if (!inputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo de entrada '" << argv[1] << "'" << std::endl;
        return 1;
    }
    if (!outputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo de saida '" << argv[2] << "'" << std::endl;
        return 1;
    }

    int T, C;
    inputFile >> T >> C; // Lê o número de servidores (T) e a capacidade de cada servidor (C)

    int N;
    inputFile >> N; // Lê o número de conjuntos de padrões

    // Aloca arrays para controlar a carga e os pedidos de cada servidor
    int* server_load = new int[T];
    std::string** server_requests = new std::string*[T];

    for (int i = 0; i < T; ++i) {
        server_load[i] = 0; // Inicializa a carga de cada servidor
        server_requests[i] = new std::string[C]; // Inicializa o array de pedidos de cada servidor
    }

    // Processa cada conjunto de padrões
    for (int i = 0; i < N; ++i) {
        int m;
        inputFile >> m; // Lê a quantidade de padrões no conjunto
        for (int j = 0; j < m; ++j) {
            std::string padrao;
            inputFile >> padrao; // Lê o padrão

            int cs = checksum(padrao); // Calcula o checksum do padrão

            // Calcula os hashes para determinar o servidor inicial e o incremento
            long long h1 = (7919LL * cs) % T;
            long long h2 = (104729LL * cs + 123) % T;
            if (h2 == 0) {
                h2 = 1;
            }

            int tentativas = 0;
            long long servidor_alocado = -1;
            long long servidor_anterior = -1;

            // Tenta alocar o padrão em um servidor usando double hashing
            while (tentativas < T) {
                long long servidor_atual = (h1 + tentativas * h2) % T;
                
                // Verifica se o servidor tem capacidade disponível
                if (server_load[servidor_atual] < C) {
                    servidor_alocado = servidor_atual;
                    
                    // Se não for a primeira tentativa, registra a transferência
                    if (tentativas > 0 && servidor_anterior != -1) {
                        outputFile << "S" << servidor_anterior << "->" << "S" << servidor_alocado << std::endl;
                    }
                    
                    // Adiciona o padrão ao servidor e incrementa a carga
                    server_requests[servidor_alocado][server_load[servidor_alocado]] = padrao;
                    server_load[servidor_alocado]++;

                    // Escreve o estado atual do servidor no arquivo de saída
                    outputFile << "S" << servidor_alocado << ":";
                    for (int k = 0; k < server_load[servidor_alocado]; ++k) {
                        outputFile << (k == 0 ? "" : ",") << server_requests[servidor_alocado][k];
                    }
                    outputFile << std::endl;
                    
                    break;
                }
                
                servidor_anterior = servidor_atual;
                tentativas++;
            }
        }
    }

    // Libera a memória alocada
    for (int i = 0; i < T; ++i) {
        delete[] server_requests[i];
    }
    delete[] server_requests;
    delete[] server_load;

    return 0;
}

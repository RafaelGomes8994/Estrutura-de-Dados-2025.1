#include <iostream>
#include <string>
#include <fstream> // NOVO: Biblioteca para manipulação de arquivos

// A função checksum permanece exatamente a mesma.
int checksum(const std::string& texto) {
    int chk = 0;
    for (char c : texto) {
        chk = chk ^ c;
    }
    return chk;
}

// NOVO: A assinatura da main foi alterada para aceitar argumentos
int main(int argc, char* argv[]) {
    // NOVO: Checagem do número de argumentos
    // O programa espera 3 argumentos: ./programa <arquivo_entrada> <arquivo_saida>
    if (argc != 3) {
        // Imprime uma mensagem de erro no console se o uso for incorreto
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1; // Retorna 1 para indicar um erro
    }

    // NOVO: Abre os arquivos de entrada e saída
    std::ifstream inputFile(argv[1]); // Objeto para ler do arquivo de entrada
    std::ofstream outputFile(argv[2]); // Objeto para escrever no arquivo de saída

    // NOVO: Verifica se os arquivos foram abertos com sucesso
    if (!inputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo de entrada '" << argv[1] << "'" << std::endl;
        return 1;
    }
    if (!outputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo de saida '" << argv[2] << "'" << std::endl;
        return 1;
    }

    int T, C;
    // MODIFICADO: Lê de 'inputFile' em vez de 'std::cin'
    inputFile >> T >> C;

    int N;
    // MODIFICADO: Lê de 'inputFile' em vez de 'std::cin'
    inputFile >> N;

    int* server_load = new int[T];
    std::string** server_requests = new std::string*[T];

    for (int i = 0; i < T; ++i) {
        server_load[i] = 0;
        server_requests[i] = new std::string[C];
    }

    for (int i = 0; i < N; ++i) {
        int m;
        // MODIFICADO: Lê de 'inputFile' em vez de 'std::cin'
        inputFile >> m;
        for (int j = 0; j < m; ++j) {
            std::string padrao;
            // MODIFICADO: Lê de 'inputFile' em vez de 'std::cin'
            inputFile >> padrao;

            int cs = checksum(padrao);

            long long h1 = (7919LL * cs) % T;
            long long h2 = (104729LL * cs + 123) % T;
            if (h2 == 0) {
                h2 = 1;
            }

            int tentativas = 0;
            long long servidor_alocado = -1;
            long long servidor_anterior = -1;

            while (tentativas < T) {
                long long servidor_atual = (h1 + tentativas * h2) % T;
                
                if (server_load[servidor_atual] < C) {
                    servidor_alocado = servidor_atual;
                    
                    if (tentativas > 0 && servidor_anterior != -1) {
                        // MODIFICADO: Escreve em 'outputFile' em vez de 'std::cout'
                        outputFile << "S" << servidor_anterior << "->" << "S" << servidor_alocado << std::endl;
                    }
                    
                    server_requests[servidor_alocado][server_load[servidor_alocado]] = padrao;
                    server_load[servidor_alocado]++;

                    // MODIFICADO: Escreve em 'outputFile' em vez de 'std::cout'
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

    for (int i = 0; i < T; ++i) {
        delete[] server_requests[i];
    }
    delete[] server_requests;
    delete[] server_load;

    // Os arquivos são fechados automaticamente quando 'inputFile' e 'outputFile' saem de escopo.
    return 0;
}
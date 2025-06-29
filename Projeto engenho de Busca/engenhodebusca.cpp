#include <iostream>
#include <string>
#include <fstream>
#include <limits>
#include <numeric>      // Incluído para std::accumulate
#include <functional>   // Incluído para std::bit_xor

/**
 * @brief Calcula o checksum de uma string usando o algoritmo std::accumulate.
 * @param texto A string de entrada para o cálculo.
 * @return O valor do checksum resultante.
 */
long long checksum(const std::string& texto) {
    return std::accumulate(texto.begin(), texto.end(), 0LL, std::bit_xor<long long>());
}

/**
 * @brief Função principal que implementa o balanceamento de carga com a lógica de saída condicional.
 */
int main(int argc, char* argv[]) {
    // 1. Validação dos argumentos e abertura dos ficheiros
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }
    std::ifstream inputFile(argv[1]);
    std::ofstream outputFile(argv[2]);
    if (!inputFile.is_open() || !outputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir um dos ficheiros." << std::endl;
        return 1;
    }

    // 2. Leitura dos parâmetros T e C
    int T, C;
    inputFile >> T >> C;

    // 3. Alocação dinâmica da memória para os servidores
    int* server_load = new int[T]();
    std::string** server_requests = new std::string*[T];
    for (int i = 0; i < T; ++i) {
        server_requests[i] = new std::string[C];
    }

    // Lê e descarta o número total de requisições, pois o loop while já controla o fim do arquivo.
    int total_requests_lines;
    inputFile >> total_requests_lines;
    
    int num_parts;
    while (inputFile >> num_parts) {
        // Monta a string completa unindo as partes com '_'
        std::string request_string = "";
        std::string temp_part;
        for (int j = 0; j < num_parts; ++j) {
            inputFile >> temp_part;
            request_string += (j == 0 ? "" : "_") + temp_part;
        }

        // 4.1. Cálculo do checksum sobre a string já montada
        int chk = checksum(request_string);

        // 4.2. Cálculo das funções de hash H1 e H2 com as correções necessárias
        long long h1 = (7919 * chk) % T;
        long long h2 = (104729 * chk + 123) % T;
        if (h2 == 0) { // Garante que a sondagem não fique presa
            h2 = 1;
        }

        // 4.3. Procura um servidor com capacidade disponível
        int tentativas = 0;
        while (tentativas < T) {
            long long servidor_atual = (h1 + ((long long)tentativas * h2)) % T;
            
            if (server_load[servidor_atual] < C) {
                // LÓGICA FINAL DE FORMATAÇÃO DA SAÍDA
                // Verifica se o servidor já tem carga ANTES de adicionar a nova requisição.
                bool servidor_ja_usado = (server_load[servidor_atual] > 0);

                // Aloca a requisição e incrementa a carga do servidor.
                server_requests[servidor_atual][server_load[servidor_atual]] = request_string;
                server_load[servidor_atual]++;

                // Escreve no arquivo de saída usando a formatação condicional.
                outputFile << "S" << servidor_atual << ":";
                if (servidor_ja_usado) {
                    // Se já foi usado, imprime a lista completa com vírgulas.
                    for (int k = 0; k < server_load[servidor_atual]; ++k) {
                        outputFile << (k == 0 ? "" : ",") << server_requests[servidor_atual][k];
                    }
                } else {
                    // Se é a primeira vez, imprime apenas a requisição atual.
                    outputFile << request_string;
                }
                outputFile << std::endl;
                
                break; // Encontrou um lugar, sai do loop de tentativas.
            }
            
            tentativas++; // Tenta o próximo servidor na sequência de sondagem.
        }
    }

    // 5. Libertação da memória alocada dinamicamente
    for (int i = 0; i < T; ++i) {
        delete[] server_requests[i];
    }
    delete[] server_requests;
    delete[] server_load;

    inputFile.close();
    outputFile.close();

    return 0;
}
#include <iostream>
#include <string>
#include <fstream>
#include <cstdint>
#include <numeric>

// --- NÃO MEXER NESTA FUNÇÃO ---
uint8_t checksum(const std::string& texto) {
    uint8_t resultado = 0;
    for (char c : texto) {
        resultado ^= static_cast<uint8_t>(c);
    }
    return resultado;
}

// --- NÃO MEXER NESTA FUNÇÃO ---
void print_server_state(std::ofstream& outputFile, int server_id, int load, const std::string* requests) {
    outputFile << "S" << server_id << ":";
    for (int k = 0; k < load; ++k) {
        outputFile << (k == 0 ? "" : ",") << requests[k];
    }
    outputFile << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de entrada: " << argv[1] << std::endl;
        return 1;
    }

    std::ofstream outputFile(argv[2]);
    if (!outputFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de saida: " << argv[2] << std::endl;
        return 1;
    }

    int T, C, N;
    inputFile >> T >> C >> N;

    int* server_load = new int[T]();
    std::string** server_requests = new std::string*[T];
    for (int i = 0; i < T; ++i) {
        server_requests[i] = new std::string[C];
    }

    for (int i = 0; i < N; ++i) {
        int m;
        inputFile >> m;

        std::string request_part;
        std::string full_request_for_print = "";
        uint8_t accumulated_checksum = 0;

        // *** AQUI ESTÁ A CORREÇÃO ***
        // Loop para ler as 'm' partes da requisição
        for (int j = 0; j < m; ++j) {
            inputFile >> request_part;
            
            // Adiciona o underscore ANTES da parte, exceto na primeira (j=0)
            if (j > 0) {
                full_request_for_print += "_";
            }
            full_request_for_print += request_part;

            // O checksum continua sendo calculado por parte, como no enunciado
            accumulated_checksum ^= checksum(request_part);
        }

        uint8_t final_checksum = accumulated_checksum;
        long long h1 = (7919LL * final_checksum) % T;
        long long h2 = (104729LL * final_checksum + 123) % T;
        if (h2 == 0) {
            h2 = 1;
        }

        int attempts = 0;
        int first_collided_server = -1;

        while (attempts < T) {
            int current_server = (h1 + (long long)attempts * h2) % T;

            if (server_load[current_server] < C) {
                if (first_collided_server != -1) {
                    outputFile << "S" << first_collided_server << "->S" << current_server << std::endl;
                }

                server_requests[current_server][server_load[current_server]] = full_request_for_print;
                server_load[current_server]++;

                print_server_state(outputFile, current_server, server_load[current_server], server_requests[current_server]);
                
                break;

            } else {
                if (attempts == 0) {
                    first_collided_server = current_server;
                }
                attempts++;
            }
        }
    }

    // Liberação de memória
    for (int i = 0; i < T; ++i) {
        delete[] server_requests[i];
    }
    delete[] server_requests;
    delete[] server_load;

    inputFile.close();
    outputFile.close();

    return 0;
}
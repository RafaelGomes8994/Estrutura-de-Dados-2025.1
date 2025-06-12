#include <iostream>
#include <string>

// --- Documentação da Função checksum ---
// Função para calcular o checksum de 8 bits de uma string.
// Realiza a operação XOR (ou-exclusivo) com os valores ASCII de todos os caracteres.
// Parâmetros:
//   - texto: A string de entrada para o cálculo.
// Retorna:
//   - Um inteiro representando o valor do checksum.
int checksum(const std::string& texto) {
    int chk = 0;
    for (char c : texto) {
        chk = chk ^ c;
    }
    return chk;
}

int main() {
    // Desativa a sincronização com stdio para acelerar a leitura de dados
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    int T, C; // T: Número de servidores, C: Capacidade máxima por servidor
    std::cin >> T >> C;

    int N; // Número total de grupos de requisições
    std::cin >> N;

    // --- Estrutura de Dados dos Servidores ---
    // Alocação dinâmica para contornar a restrição de não usar std::vector
    // server_load: armazena a carga atual (nº de padrões) de cada servidor.
    int* server_load = new int[T];
    // server_requests: um array 2D para armazenar as strings dos padrões de cada servidor.
    std::string** server_requests = new std::string*[T];

    // Inicialização das estruturas
    for (int i = 0; i < T; ++i) {
        server_load[i] = 0; // Carga inicial é 0
        server_requests[i] = new std::string[C]; // Aloca espaço para C padrões
    }

    // Loop principal para processar cada grupo de requisições
    for (int i = 0; i < N; ++i) {
        int m; // Número de padrões neste grupo
        std::cin >> m;
        for (int j = 0; j < m; ++j) {
            std::string padrao;
            std::cin >> padrao;

            int cs = checksum(padrao);

            // --- Lógica de Hashing Duplo ---
            long long h1 = (7919LL * cs) % T;
            long long h2 = (104729LL * cs + 123) % T;
            
            // Garante que h2 não seja 0 para evitar loops infinitos se T for um divisor.
            if (h2 == 0) {
                h2 = 1;
            }

            int tentativas = 0;
            long long servidor_alocado = -1;
            long long servidor_anterior = -1;

            while (tentativas < T) { // Tenta no máximo T vezes para evitar loop infinito
                long long servidor_atual = (h1 + tentativas * h2) % T;
                
                if (server_load[servidor_atual] < C) { // Encontrou um servidor com espaço
                    servidor_alocado = servidor_atual;
                    
                    // Se houve realocação (tentativas > 0), imprime a mudança
                    if (tentativas > 0 && servidor_anterior != -1) {
                        std::cout << "S" << servidor_anterior << "->" << "S" << servidor_alocado << std::endl;
                    }
                    
                    // Adiciona o padrão ao servidor
                    server_requests[servidor_alocado][server_load[servidor_alocado]] = padrao;
                    server_load[servidor_alocado]++;

                    // Imprime o estado atual do servidor
                    std::cout << "S" << servidor_alocado << ":";
                    for (int k = 0; k < server_load[servidor_alocado]; ++k) {
                        std::cout << (k == 0 ? "" : ",") << server_requests[servidor_alocado][k];
                    }
                    std::cout << std::endl;
                    
                    break; // Sai do loop de tentativas
                }
                
                servidor_anterior = servidor_atual;
                tentativas++;
            }
        }
    }

    // --- Limpeza da Memória ---
    // Libera a memória alocada dinamicamente para evitar memory leaks
    for (int i = 0; i < T; ++i) {
        delete[] server_requests[i];
    }
    delete[] server_requests;
    delete[] server_load;

    return 0;
}
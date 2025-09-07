#include <iostream>
#include <fstream>
#include <cstdint>
#include <string>
#include <sstream> // Usaremos para a construção eficiente da string de saída

// --- Estruturas e Funções de Union-Find ---
// Deixamos a estrutura do nó mais simples, focada apenas nos dados.
//
struct No {
    No* pai;
    uint32_t altura;
};

// Funções Union-Find agora são independentes, como nos slides.
//
void criar_conjunto(No* no) {
    no->pai = no;
    no->altura = 0;
}

//
No* encontrar_conjunto_raiz(No* no) {
    if (no->pai != no) {
        no->pai = encontrar_conjunto_raiz(no->pai); // Compressão de Caminho
    }
    return no->pai;
}

//
void unir_conjuntos(No* a, No* b) {
    No* raizA = encontrar_conjunto_raiz(a);
    No* raizB = encontrar_conjunto_raiz(b);

    if (raizA == raizB) return;

    if (raizA->altura > raizB->altura) {
        raizB->pai = raizA;
    } else {
        raizA->pai = raizB;
        if (raizA->altura == raizB->altura) {
            raizB->altura++;
        }
    }
}

// --- Classe Principal da Simulação ---
// Renomeada para maior clareza e agora gerencia todos os recursos.
class Simulacao {
private:
    uint32_t largura, altura;
    No* nos_uniao; // Array 1D para os nós do Union-Find
    bool* infectados; // Array 1D para rastrear infecção
    uint32_t total_infectados;
    std::stringstream resultado_ss; // Para construir a string de saída eficientemente

    // Função g(z) dos slides
    static int32_t calcular_proxima_coordenada(uint32_t z) {
        static uint32_t next = 1; // myrand() embutido e reiniciado para cada simulação
        next = next * 1103515245 + 12345;
        return z + (-1 + (next % 3));
    }

    // Funções auxiliares (inline para performance)
    inline bool coordenada_valida(int32_t x, int32_t y) const {
        return x >= 0 && x < largura && y >= 0 && y < altura;
    }

    inline No* obter_no(uint32_t x, uint32_t y) {
        return &nos_uniao[y * largura + x];
    }
    
    inline bool esta_infectado(uint32_t x, uint32_t y) const {
        return infectados[y * largura + x];
    }

public:
    // Construtor: aloca e inicializa tudo (RAII)
    Simulacao(uint32_t w, uint32_t h, uint32_t pz_y, uint32_t pz_x, uint32_t indice)
        : largura(w), altura(h), total_infectados(0) {

        uint32_t total_pessoas = largura * altura;
        if (total_pessoas == 0) return;

        nos_uniao = new No[total_pessoas];
        infectados = new bool[total_pessoas];

        for (uint32_t i = 0; i < total_pessoas; ++i) {
            criar_conjunto(&nos_uniao[i]);
            infectados[i] = false;
        }

        // Configura o paciente zero
        infectados[pz_y * largura + pz_x] = true;
        total_infectados = 1;

        // Inicia a string de resultado
        resultado_ss << indice << ":(" << pz_y << "," << pz_x << ")";
    }

    // Destrutor: libera a memória automaticamente
    ~Simulacao() {
        delete[] nos_uniao;
        delete[] infectados;
    }

    // Executa a simulação completa
    void executar(uint32_t pz_y, uint32_t pz_x) {
        uint32_t total_pessoas = largura * altura;
        if (total_pessoas <= 1) return;

        uint32_t propagador_y = pz_y;
        uint32_t propagador_x = pz_x;

        while (total_infectados < total_pessoas) {
            uint32_t propagador_inicial_y = propagador_y;
            uint32_t propagador_inicial_x = propagador_x;

            // Loop para encontrar a próxima vítima (lógica corrigida)
            while (true) {
                int32_t novo_y = calcular_proxima_coordenada(propagador_y);
                int32_t novo_x = calcular_proxima_coordenada(propagador_x);

                if (coordenada_valida(novo_x, novo_y)) {
                    if (esta_infectado(novo_x, novo_y)) {
                        // "Pulo do gato": salta para o infectado e tenta de novo
                        propagador_y = novo_y;
                        propagador_x = novo_x;
                    } else {
                        // Vítima encontrada!
                        infectados[novo_y * largura + novo_x] = true;
                        total_infectados++;
                        
                        resultado_ss << ";(" << novo_y << "," << novo_x << ")";

                        unir_conjuntos(obter_no(propagador_inicial_x, propagador_inicial_y), obter_no(novo_x, novo_y));
                        
                        // O novo infectado se torna o próximo propagador
                        propagador_y = novo_y;
                        propagador_x = novo_x;
                        break; // Sai do loop de busca
                    }
                }
            }
        }
    }

    // Retorna o resultado final
    std::string obter_resultado() const {
        return resultado_ss.str();
    }
};

int main(int argc, char* argv[]) {
    // Boas práticas de C++ para I/O rápido
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);

    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>\n";
        return 1;
    }

    std::ifstream entrada(argv[1]);
    std::ofstream saida(argv[2]);

    if (!entrada.is_open() || !saida.is_open()) {
        std::cerr << "Erro ao abrir os arquivos.\n";
        return 1;
    }

    uint32_t quantidade_simulacoes;
    entrada >> quantidade_simulacoes;

    for (uint32_t i = 1; i <= quantidade_simulacoes; ++i) {
        uint32_t largura, altura, pz_y, pz_x;
        entrada >> altura >> largura >> pz_y >> pz_x; // Ordem do arquivo: Altura Largura Y X

        // Objeto criado na stack: memória é liberada automaticamente no fim do loop
        Simulacao sim(largura, altura, pz_y, pz_x, i);
        sim.executar(pz_y, pz_x);

        saida << sim.obter_resultado() << "\n";
    }

    return 0;
}
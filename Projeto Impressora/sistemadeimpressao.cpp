#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Estrutura para informações do documento
struct Documento {
    std::string nome;
    int paginas;
    std::string nome_formatado;

    Documento() : paginas(0) {}
};

// Estrutura para informações da impressora
struct Impressora {
    std::string nome;
    Documento** historico;
    int qtd_historico;
    int capacidade_historico;
    int ocupada_ate; // Agora representa o tempo de conclusão do último trabalho nesta impressora

    Impressora() : historico(nullptr), qtd_historico(0), capacidade_historico(0), ocupada_ate(0) {}

    void inicializar_historico(int total_docs) {
        capacidade_historico = total_docs;
        historico = new Documento*[capacidade_historico];
        qtd_historico = 0;
        ocupada_ate = 0;
    }

    void adicionar_historico(Documento* doc) {
        if (qtd_historico < capacidade_historico) {
            historico[qtd_historico] = doc;
            qtd_historico++;
        }
    }

    void limpar_historico() {
        delete[] historico;
        historico = nullptr;
    }
};

// A fila não é mais necessária com a lógica de rodízio.

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream arquivo_entrada(argv[1]);
    if (!arquivo_entrada.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo de entrada '" << argv[1] << "'" << std::endl;
        return 1;
    }

    std::ofstream arquivo_saida(argv[2]);
    if (!arquivo_saida.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo de saida '" << argv[2] << "'" << std::endl;
        return 1;
    }

    int qtd_impressoras;
    arquivo_entrada >> qtd_impressoras;
    Impressora* impressoras = new Impressora[qtd_impressoras];
    for (int i = 0; i < qtd_impressoras; ++i) {
        arquivo_entrada >> impressoras[i].nome;
    }

    int qtd_documentos;
    arquivo_entrada >> qtd_documentos;
    for (int i = 0; i < qtd_impressoras; ++i) {
        impressoras[i].inicializar_historico(qtd_documentos);
    }
    
    Documento* todos_documentos = new Documento[qtd_documentos];
    int soma_paginas = 0;

    std::ostringstream oss;
    
    for (int i = 0; i < qtd_documentos; ++i) {
        arquivo_entrada >> todos_documentos[i].nome >> todos_documentos[i].paginas;

        oss.str("");
        oss.clear();
        oss << todos_documentos[i].nome << "-" << todos_documentos[i].paginas << "p";
        todos_documentos[i].nome_formatado = oss.str();

        soma_paginas += todos_documentos[i].paginas;
        // A chamada para enfileirar() foi removida, pois a fila não é mais necessária.
    }

    // --- LÓGICA DE SIMULAÇÃO SIMPLIFICADA (RODÍZIO) ---
    // Substitui o laço while complexo por um for simples.
    for (int i = 0; i < qtd_documentos; ++i) {
        Documento* doc_para_imprimir = &todos_documentos[i];

        // Determina a impressora via rodízio (round-robin)
        int indice_impressora = i % qtd_impressoras;
        Impressora* impressora_alvo = &impressoras[indice_impressora];

        // Imprime a linha de alocação com o histórico *anterior*
        arquivo_saida << impressora_alvo->nome << ":" << doc_para_imprimir->nome_formatado;
        for (int h = impressora_alvo->qtd_historico - 1; h >= 0; --h) {
            arquivo_saida << ", " << impressora_alvo->historico[h]->nome_formatado;
        }
        arquivo_saida << std::endl;

        // Adiciona o documento atual ao histórico da impressora (para as próximas impressões)
        impressora_alvo->adicionar_historico(doc_para_imprimir);
        
        // A variável ocupada_ate não é estritamente necessária para gerar esta saída,
        // mas seria mantida para calcular o tempo total, se preciso.
        // A lógica de tempo seria:
        // impressora_alvo->ocupada_ate += doc_para_imprimir->paginas;
    }

    // --- Saída do Sumário ---
    arquivo_saida << soma_paginas << "p" << std::endl;
    for (int i = qtd_documentos - 1; i >= 0; --i) {
        arquivo_saida << todos_documentos[i].nome_formatado << std::endl;
    }

    // --- Limpeza ---
    arquivo_entrada.close();
    arquivo_saida.close();
    for (int i = 0; i < qtd_impressoras; ++i) {
        impressoras[i].limpar_historico();
    }
    delete[] impressoras;
    delete[] todos_documentos; 

    // A chamada para limpar_fila() foi removida.

    return 0;
}
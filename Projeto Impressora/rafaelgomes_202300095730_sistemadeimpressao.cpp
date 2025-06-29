#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Estrutura para informações do documento
struct Documento {
    std::string nome;
    int paginas;
    std::string nome_formatado;
    int indice_original; // Para critério de desempate na ordenação
    int tempo_termino;   // Momento em que a impressão termina

    Documento() : paginas(0), indice_original(0), tempo_termino(0) {}
};

// Estrutura para informações da impressora
struct Impressora {
    std::string nome;
    Documento** historico;
    int qtd_historico;
    int capacidade_historico;
    int ocupada_ate; // Carga de trabalho acumulada (em páginas)

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

// Função de ordenação manual (Bubble Sort) para os documentos
void ordenarDocumentos(Documento array[], int tamanho) {
    for (int i = 0; i < tamanho - 1; i++) {
        for (int j = 0; j < tamanho - i - 1; j++) {
            // Critério 1: Ordenar por tempo de término
            bool precisaTrocar = false;
            if (array[j].tempo_termino > array[j + 1].tempo_termino) {
                precisaTrocar = true;
            } 
            // Critério 2 (desempate): Se o tempo for igual, usa o índice original
            else if (array[j].tempo_termino == array[j + 1].tempo_termino) {
                if (array[j].indice_original > array[j + 1].indice_original) {
                    precisaTrocar = true;
                }
            }

            if (precisaTrocar) {
                Documento temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
            }
        }
    }
}

int main() {
    std::ifstream arquivo_entrada("Entrada_de_Bruno.txt");
    std::ofstream arquivo_saida("sistemadeimpressaoMEU.txt");

    if (!arquivo_entrada.is_open() || !arquivo_saida.is_open()) {
        std::cerr << "Erro ao abrir os arquivos." << std::endl;
        return 1;
    }

    int qtd_impressoras;
    arquivo_entrada >> qtd_impressoras;
    arquivo_entrada.ignore(); 

    Impressora* impressoras = new Impressora[qtd_impressoras];
    for (int i = 0; i < qtd_impressoras; ++i) {
        std::getline(arquivo_entrada, impressoras[i].nome);
    }

    int qtd_documentos;
    arquivo_entrada >> qtd_documentos;
    arquivo_entrada.ignore();

    // Usando array dinâmico em vez de std::vector
    Documento* todos_documentos = new Documento[qtd_documentos];
    long long soma_paginas = 0;

    for (int i = 0; i < qtd_documentos; ++i) {
        arquivo_entrada >> todos_documentos[i].nome >> todos_documentos[i].paginas;
        arquivo_entrada.ignore(); 
        
        std::stringstream ss;
        ss << todos_documentos[i].nome << "-" << todos_documentos[i].paginas << "p";
        todos_documentos[i].nome_formatado = ss.str();
        soma_paginas += todos_documentos[i].paginas;
        todos_documentos[i].indice_original = i;
    }

    for (int i = 0; i < qtd_impressoras; ++i) {
        impressoras[i].inicializar_historico(qtd_documentos);
    }

    // --- Lógica de Alocação ---
    for (int i = 0; i < qtd_documentos; ++i) {
        Documento* doc_para_imprimir = &todos_documentos[i];

        int indice_melhor_impressora = 0;
        for (int j = 1; j < qtd_impressoras; ++j) {
            if (impressoras[j].ocupada_ate < impressoras[indice_melhor_impressora].ocupada_ate) {
                indice_melhor_impressora = j;
            }
        }
        Impressora* impressora_alvo = &impressoras[indice_melhor_impressora];

        arquivo_saida << impressora_alvo->nome << ":" << doc_para_imprimir->nome_formatado;
        for (int h = impressora_alvo->qtd_historico - 1; h >= 0; --h) {
            arquivo_saida << ", " << impressora_alvo->historico[h]->nome_formatado;
        }
        arquivo_saida << std::endl;
        
        // CALCULAR TEMPO DE TÉRMINO
        doc_para_imprimir->tempo_termino = impressora_alvo->ocupada_ate + doc_para_imprimir->paginas;

        // Atualizar impressora
        impressora_alvo->adicionar_historico(doc_para_imprimir);
        impressora_alvo->ocupada_ate += doc_para_imprimir->paginas;
    }

    // --- Saída do Sumário (LÓGICA DA PILHA) ---
    arquivo_saida << soma_paginas << "p" << std::endl;
    
    // 1. Ordenar os documentos por tempo de término usando nossa função manual
    ordenarDocumentos(todos_documentos, qtd_documentos);
    
    // 2. Imprimir em ordem inversa (simulando a pilha)
    for (int i = qtd_documentos - 1; i >= 0; --i) {
        arquivo_saida << todos_documentos[i].nome_formatado << std::endl;
    }

    // --- Limpeza de Memória ---
    for (int i = 0; i < qtd_impressoras; ++i) {
        impressoras[i].limpar_historico();
    }
    delete[] impressoras;
    delete[] todos_documentos; // Importante liberar a memória do array de documentos
    
    arquivo_entrada.close();
    arquivo_saida.close();

    return 0;
}
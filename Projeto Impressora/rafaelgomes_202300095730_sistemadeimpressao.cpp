#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// --- ESTRUTURAS DE DADOS (sem alteração) ---
struct Documento {
    std::string nome;
    int paginas;
    std::string nome_formatado;
    int indice_original;
    int tempo_termino;
    Documento() : paginas(0), indice_original(0), tempo_termino(0) {}
};

struct Impressora {
    std::string nome;
    Documento** historico;
    int qtd_historico;
    int capacidade_historico;
    int ocupada_ate;
    Impressora() : historico(nullptr), qtd_historico(0), capacidade_historico(0), ocupada_ate(0) {}
    void inicializar_historico(int total_docs) {
        capacidade_historico = total_docs;
        historico = new Documento*[capacidade_historico];
        qtd_historico = 0;
        ocupada_ate = 0;
    }
    void adicionar_historico(Documento* doc) {
        if (qtd_historico < capacidade_historico) historico[qtd_historico++] = doc;
    }
    void limpar_historico() {
        delete[] historico;
        historico = nullptr;
    }
};


// --- OTIMIZAÇÃO 1: MIN-HEAP PARA GERENCIAR IMPRESSORAS ---
struct NoHeap {
    int carga_trabalho; // ocupada_ate
    int indice_impressora;
};

void heapify_down(NoHeap heap[], int tamanho, int i) {
    int menor = i;
    int esquerda = 2 * i + 1;
    int direita = 2 * i + 2;

    if (esquerda < tamanho && heap[esquerda].carga_trabalho < heap[menor].carga_trabalho)
        menor = esquerda;
    if (direita < tamanho && heap[direita].carga_trabalho < heap[menor].carga_trabalho)
        menor = direita;

    if (menor != i) {
        NoHeap temp = heap[i];
        heap[i] = heap[menor];
        heap[menor] = temp;
        heapify_down(heap, tamanho, menor);
    }
}

void construirHeap(NoHeap heap[], int tamanho) {
    for (int i = tamanho / 2 - 1; i >= 0; i--) {
        heapify_down(heap, tamanho, i);
    }
}


// --- OTIMIZAÇÃO 2: MERGESORT PARA ORDENAÇÃO FINAL ---
void merge(Documento array[], int const esq, int const meio, int const dir) {
    int const tam_esq = meio - esq + 1;
    int const tam_dir = dir - meio;

    Documento* array_esq = new Documento[tam_esq];
    Documento* array_dir = new Documento[tam_dir];

    for (int i = 0; i < tam_esq; i++) array_esq[i] = array[esq + i];
    for (int j = 0; j < tam_dir; j++) array_dir[j] = array[meio + 1 + j];

    int i = 0, j = 0, k = esq;

    while (i < tam_esq && j < tam_dir) {
        bool esq_menor = false;
        if (array_esq[i].tempo_termino < array_dir[j].tempo_termino) {
            esq_menor = true;
        } else if (array_esq[i].tempo_termino == array_dir[j].tempo_termino && array_esq[i].indice_original < array_dir[j].indice_original) {
            esq_menor = true;
        }

        if (esq_menor) {
            array[k] = array_esq[i];
            i++;
        } else {
            array[k] = array_dir[j];
            j++;
        }
        k++;
    }

    while (i < tam_esq) array[k++] = array_esq[i++];
    while (j < tam_dir) array[k++] = array_dir[j++];

    delete[] array_esq;
    delete[] array_dir;
}

void mergeSort(Documento array[], int const inicio, int const fim) {
    if (inicio >= fim) return;
    int meio = inicio + (fim - inicio) / 2;
    mergeSort(array, inicio, meio);
    mergeSort(array, meio + 1, fim);
    merge(array, inicio, meio, fim);
}


// --- FUNÇÃO PRINCIPAL OTIMIZADA ---
int main(int argc, char* argv[]) {
    // Micro-otimização de I/O
    std::ios_base::sync_with_stdio(false);
    std::ifstream::sync_with_stdio(false);
    
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>\n";
        return 1;
    }
    std::ifstream arquivo_entrada(argv[1]);
    std::ofstream arquivo_saida(argv[2]);
    if (!arquivo_entrada.is_open() || !arquivo_saida.is_open()) {
        std::cerr << "Erro ao abrir os arquivos.\n";
        return 1;
    }
    arquivo_entrada.tie(NULL);

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

    // --- Lógica de Alocação OTIMIZADA COM MIN-HEAP ---
    NoHeap* heap_impressoras = new NoHeap[qtd_impressoras];
    for(int i = 0; i < qtd_impressoras; ++i) {
        heap_impressoras[i] = {0, i};
    }
    construirHeap(heap_impressoras, qtd_impressoras);

    for (int i = 0; i < qtd_documentos; ++i) {
        Documento* doc_para_imprimir = &todos_documentos[i];
        
        // Pega a melhor impressora do topo do heap (O(1))
        NoHeap melhor_no = heap_impressoras[0];
        int indice_melhor_impressora = melhor_no.indice_impressora;
        Impressora* impressora_alvo = &impressoras[indice_melhor_impressora];

        // Escreve a saída (lógica inalterada)
        arquivo_saida << impressora_alvo->nome << ":" << doc_para_imprimir->nome_formatado;
        for (int h = impressora_alvo->qtd_historico - 1; h >= 0; --h) {
            arquivo_saida << ", " << impressora_alvo->historico[h]->nome_formatado;
        }
        arquivo_saida << std::endl;
        
        doc_para_imprimir->tempo_termino = impressora_alvo->ocupada_ate + doc_para_imprimir->paginas;
        impressora_alvo->adicionar_historico(doc_para_imprimir);
        impressora_alvo->ocupada_ate += doc_para_imprimir->paginas;

        // Atualiza a carga da impressora no heap e reorganiza (O(log N))
        heap_impressoras[0].carga_trabalho = impressora_alvo->ocupada_ate;
        heapify_down(heap_impressoras, qtd_impressoras, 0);
    }

    // --- Saída do Sumário OTIMIZADA COM MERGESORT ---
    arquivo_saida << soma_paginas << "p" << std::endl;
    
    // 1. Ordenar com Mergesort (O(M log M))
    mergeSort(todos_documentos, 0, qtd_documentos - 1);
    
    // 2. Imprimir (lógica inalterada)
    for (int i = qtd_documentos - 1; i >= 0; --i) {
        arquivo_saida << todos_documentos[i].nome_formatado << std::endl;
    }

    // --- Limpeza de Memória ---
    for (int i = 0; i < qtd_impressoras; ++i) {
        impressoras[i].limpar_historico();
    }
    delete[] impressoras;
    delete[] todos_documentos;
    delete[] heap_impressoras;
    
    arquivo_entrada.close();
    arquivo_saida.close();

    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Estrutura para informações do documento
struct DocumentInfo {
    std::string name;
    int pages;
    std::string formatted_name_pages;

    DocumentInfo() : pages(0) {}
};

// Estrutura para informações da impressora
struct Printer {
    std::string name;
    DocumentInfo** history_items; // Array de ponteiros para DocumentInfo
    int history_count;
    int max_history_capacity;
    int busy_until_time;

    Printer() : history_items(nullptr), history_count(0), max_history_capacity(0), busy_until_time(0) {}

    void init_history(int total_docs) {
        max_history_capacity = total_docs;
        history_items = new DocumentInfo*[max_history_capacity];
        history_count = 0;
        busy_until_time = 0;
    }

    // OTIMIZAÇÃO: Adiciona um ponteiro ao final do histórico. Operação O(1).
    void add_to_history(DocumentInfo* doc_ptr) {
        if (history_count < max_history_capacity) {
            history_items[history_count] = doc_ptr;
            history_count++;
        }
    }

    void cleanup_history() {
        delete[] history_items;
        history_items = nullptr;
    }
};

// --- Fila manual (armazena ponteiros para DocumentInfo) ---
DocumentInfo** docs_to_print_queue_arr;
int queue_capacity;
int queue_front_idx;
int queue_rear_idx;
int current_queue_size;

void init_queue(int capacity) {
    docs_to_print_queue_arr = new DocumentInfo*[capacity];
    queue_capacity = capacity;
    queue_front_idx = 0;
    queue_rear_idx = -1;
    current_queue_size = 0;
}

bool is_queue_empty() { return current_queue_size == 0; }

void enqueue(DocumentInfo* doc_ptr) {
    if (current_queue_size < queue_capacity) {
        queue_rear_idx = (queue_rear_idx + 1) % queue_capacity;
        docs_to_print_queue_arr[queue_rear_idx] = doc_ptr;
        current_queue_size++;
    }
}

DocumentInfo* dequeue() {
    DocumentInfo* doc_ptr = nullptr;
    if (!is_queue_empty()) {
        doc_ptr = docs_to_print_queue_arr[queue_front_idx];
        queue_front_idx = (queue_front_idx + 1) % queue_capacity;
        current_queue_size--;
    }
    return doc_ptr;
}

DocumentInfo* front_of_queue() {
    DocumentInfo* doc_ptr = nullptr;
    if (!is_queue_empty()) {
        doc_ptr = docs_to_print_queue_arr[queue_front_idx];
    }
    return doc_ptr;
}

void cleanup_queue() {
    delete[] docs_to_print_queue_arr;
    docs_to_print_queue_arr = nullptr;
}
// --- Fim da Fila Manual ---

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_de_entrada> <arquivo_de_saida>" << std::endl;
        return 1;
    }

    std::ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel abrir o arquivo de entrada '" << argv[1] << "'" << std::endl;
        return 1;
    }

    std::ofstream outputFile(argv[2]);
    if (!outputFile.is_open()) {
        std::cerr << "Erro: Nao foi possivel criar o arquivo de saida '" << argv[2] << "'" << std::endl;
        return 1;
    }

    int num_printers;
    inputFile >> num_printers;
    Printer* printers_list = new Printer[num_printers];
    for (int i = 0; i < num_printers; ++i) {
        inputFile >> printers_list[i].name;
    }

    int num_documents;
    inputFile >> num_documents;
    for (int i = 0; i < num_printers; ++i) {
        printers_list[i].init_history(num_documents);
    }
    
    init_queue(num_documents);
    DocumentInfo* all_docs_for_summary = new DocumentInfo[num_documents];
    int total_pages_sum = 0;

    // OTIMIZAÇÃO: Reutiliza o mesmo objeto ostringstream
    std::ostringstream oss;
    
    // OTIMIZAÇÃO: Combina a leitura e o enfileiramento em um único loop
    for (int i = 0; i < num_documents; ++i) {
        // Lê o documento para o armazenamento principal
        inputFile >> all_docs_for_summary[i].name >> all_docs_for_summary[i].pages;

        // Formata a string de saída, limpando o stream antes de usar
        oss.str("");
        oss.clear();
        oss << all_docs_for_summary[i].name << "-" << all_docs_for_summary[i].pages << "p";
        all_docs_for_summary[i].formatted_name_pages = oss.str();

        total_pages_sum += all_docs_for_summary[i].pages;

        // Adiciona um ponteiro para o documento recém-lido na fila
        enqueue(&all_docs_for_summary[i]);
    }

    // --- Simulação ---
    int current_time = 0;
    int documents_assigned_count = 0;
    while (documents_assigned_count < num_documents) {
        bool job_assigned_this_tick = false;

        for (int i = 0; i < num_printers; ++i) {
            if (!is_queue_empty() && printers_list[i].busy_until_time <= current_time) {
                DocumentInfo* doc_to_print_ptr = front_of_queue();
                dequeue();

                printers_list[i].busy_until_time = current_time + doc_to_print_ptr->pages;

                outputFile << printers_list[i].name << ":" << doc_to_print_ptr->formatted_name_pages;
                
                // OTIMIZAÇÃO: Imprime o histórico em ordem inversa para corresponder à saída.
                // O histórico agora está [mais antigo, ..., mais recente], então iteramos de trás para frente.
                for (int h = printers_list[i].history_count - 1; h >= 0; --h) {
                    outputFile << ", " << printers_list[i].history_items[h]->formatted_name_pages;
                }
                outputFile << std::endl;

                // Adiciona ao histórico DEPOIS de imprimir, para que não apareça na sua própria linha de histórico.
                printers_list[i].add_to_history(doc_to_print_ptr);

                documents_assigned_count++;
                job_assigned_this_tick = true;
                if (documents_assigned_count == num_documents) break;
            }
        }
        
        if (documents_assigned_count == num_documents) break;

        if (job_assigned_this_tick) {
            current_time++;
        } else {
            if (!is_queue_empty()) {
                int min_next_free_time = -1;
                for (int i = 0; i < num_printers; ++i) {
                    if (printers_list[i].busy_until_time > current_time) {
                        if (min_next_free_time == -1 || printers_list[i].busy_until_time < min_next_free_time) {
                            min_next_free_time = printers_list[i].busy_until_time;
                        }
                    }
                }
                if (min_next_free_time != -1) {
                    current_time = min_next_free_time;
                } else {
                    outputFile << "ERRO: Simulacao estagnada." << std::endl;
                    std::cerr << "ERRO: Simulacao estagnada. Verifique a logica ou a entrada." << std::endl;
                    break; 
                }
            } else {
                break;
            }
        }
    }

    // --- Saída do Sumário ---
    outputFile << total_pages_sum << "p" << std::endl;
    for (int i = num_documents - 1; i >= 0; --i) {
        outputFile << all_docs_for_summary[i].formatted_name_pages << std::endl;
    }

    // --- Limpeza ---
    inputFile.close();
    outputFile.close();
    for (int i = 0; i < num_printers; ++i) {
        printers_list[i].cleanup_history();
    }
    delete[] printers_list;
    delete[] all_docs_for_summary; 
    cleanup_queue();

    return 0;
}

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
    int ocupada_ate;

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

// Fila manual (armazena ponteiros para Documento)
Documento** fila_docs;
int capacidade_fila;
int inicio_fila;
int fim_fila;
int tamanho_fila;

void inicializar_fila(int capacidade) {
    fila_docs = new Documento*[capacidade];
    capacidade_fila = capacidade;
    inicio_fila = 0;
    fim_fila = -1;
    tamanho_fila = 0;
}

bool fila_vazia() { return tamanho_fila == 0; }

void enfileirar(Documento* doc) {
    if (tamanho_fila < capacidade_fila) {
        fim_fila = (fim_fila + 1) % capacidade_fila;
        fila_docs[fim_fila] = doc;
        tamanho_fila++;
    }
}

Documento* desenfileirar() {
    Documento* doc = nullptr;
    if (!fila_vazia()) {
        doc = fila_docs[inicio_fila];
        inicio_fila = (inicio_fila + 1) % capacidade_fila;
        tamanho_fila--;
    }
    return doc;
}

Documento* topo_fila() {
    Documento* doc = nullptr;
    if (!fila_vazia()) {
        doc = fila_docs[inicio_fila];
    }
    return doc;
}

void limpar_fila() {
    delete[] fila_docs;
    fila_docs = nullptr;
}

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
    
    inicializar_fila(qtd_documentos);
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

        enfileirar(&todos_documentos[i]);
    }

    int tempo_atual = 0;
    int documentos_processados = 0;
    while (documentos_processados < qtd_documentos) {
        bool documento_atribuido = false;

        for (int i = 0; i < qtd_impressoras; ++i) {
            if (!fila_vazia() && impressoras[i].ocupada_ate <= tempo_atual) {
                Documento* doc_para_imprimir = topo_fila();
                desenfileirar();

                impressoras[i].ocupada_ate = tempo_atual + doc_para_imprimir->paginas;

                arquivo_saida << impressoras[i].nome << ":" << doc_para_imprimir->nome_formatado;
                for (int h = impressoras[i].qtd_historico - 1; h >= 0; --h) {
                    arquivo_saida << ", " << impressoras[i].historico[h]->nome_formatado;
                }
                arquivo_saida << std::endl;

                impressoras[i].adicionar_historico(doc_para_imprimir);

                documentos_processados++;
                documento_atribuido = true;
                if (documentos_processados == qtd_documentos) break;
            }
        }
        
        if (documentos_processados == qtd_documentos) break;

        if (documento_atribuido) {
            tempo_atual++;
        } else {
            if (!fila_vazia()) {
                int proximo_tempo_livre = -1;
                for (int i = 0; i < qtd_impressoras; ++i) {
                    if (impressoras[i].ocupada_ate > tempo_atual) {
                        if (proximo_tempo_livre == -1 || impressoras[i].ocupada_ate < proximo_tempo_livre) {
                            proximo_tempo_livre = impressoras[i].ocupada_ate;
                        }
                    }
                }
                if (proximo_tempo_livre != -1) {
                    tempo_atual = proximo_tempo_livre;
                } else {
                    arquivo_saida << "ERRO: Simulacao estagnada." << std::endl;
                    std::cerr << "ERRO: Simulacao estagnada. Verifique a logica ou a entrada." << std::endl;
                    break; 
                }
            } else {
                break;
            }
        }
    }

    arquivo_saida << soma_paginas << "p" << std::endl;
    for (int i = qtd_documentos - 1; i >= 0; --i) {
        arquivo_saida << todos_documentos[i].nome_formatado << std::endl;
    }

    arquivo_entrada.close();
    arquivo_saida.close();
    for (int i = 0; i < qtd_impressoras; ++i) {
        impressoras[i].limpar_historico();
    }
    delete[] impressoras;
    delete[] todos_documentos; 
    limpar_fila();

    return 0;
}

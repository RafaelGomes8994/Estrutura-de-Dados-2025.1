#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdint>
#include <cctype>

struct Pessoa {
    char* nome;
    uint32_t idade;
    uint32_t prioridade;
    uint32_t ordemChegada;
    Pessoa() : nome(nullptr), idade(0), prioridade(0), ordemChegada(0) {}
    Pessoa(const Pessoa& other) {
        idade = other.idade; prioridade = other.prioridade; ordemChegada = other.ordemChegada;
        if (other.nome) { nome = new char[strlen(other.nome) + 1]; strcpy(nome, other.nome); } else { nome = nullptr; }
    }
    Pessoa& operator=(const Pessoa& other) {
        if (this == &other) return *this;
        delete[] nome;
        idade = other.idade; prioridade = other.prioridade; ordemChegada = other.ordemChegada;
        if (other.nome) { nome = new char[strlen(other.nome) + 1]; strcpy(nome, other.nome); } else { nome = nullptr; }
        return *this;
    }
    ~Pessoa() { delete[] nome; }
};
class FilaPrioridade {
private:
    Pessoa* heap; uint32_t capacidade; uint32_t tamanho;
    uint32_t pai(uint32_t i) { return (i - 1) / 2; }
    uint32_t esquerdo(uint32_t i) { return (2 * i) + 1; }
    uint32_t direito(uint32_t i) { return (2 * i) + 2; }
    void trocar(Pessoa& a, Pessoa& b) { Pessoa temp = a; a = b; b = temp; }
    void heapify(uint32_t i) {
        uint32_t e = esquerdo(i), d = direito(i), maior = i;
        if (e < tamanho) { if (heap[e].prioridade > heap[maior].prioridade || (heap[e].prioridade == heap[maior].prioridade && heap[e].ordemChegada < heap[maior].ordemChegada)) maior = e; }
        if (d < tamanho) { if (heap[d].prioridade > heap[maior].prioridade || (heap[d].prioridade == heap[maior].prioridade && heap[d].ordemChegada < heap[maior].ordemChegada)) maior = d; }
        if (maior != i) { trocar(heap[i], heap[maior]); heapify(maior); }
    }
public:
    FilaPrioridade(uint32_t cap = 10) : capacidade(cap), tamanho(0) { heap = new Pessoa[capacidade]; }
    ~FilaPrioridade() { delete[] heap; }
    bool vazia() const { return tamanho == 0; }
    void enfileirar(const Pessoa& p) {
        if (tamanho == capacidade) {
            capacidade *= 2; Pessoa* novoHeap = new Pessoa[capacidade];
            for (uint32_t i = 0; i < tamanho; ++i) novoHeap[i] = heap[i];
            delete[] heap; heap = novoHeap;
        }
        uint32_t i = tamanho; heap[i] = p; tamanho++;
        while (i != 0) {
            uint32_t p_idx = pai(i); bool deveSubir = false;
            if (heap[i].prioridade > heap[p_idx].prioridade || (heap[i].prioridade == heap[p_idx].prioridade && heap[i].ordemChegada < heap[p_idx].ordemChegada)) deveSubir = true;
            if (deveSubir) { trocar(heap[i], heap[p_idx]); i = p_idx; } else break;
        }
    }
    Pessoa desenfileirar() {
        if (vazia()) return Pessoa();
        Pessoa raiz = heap[0]; heap[0] = heap[tamanho - 1]; tamanho--; heapify(0); return raiz;
    }
};
struct Orgao { char nome[51]; uint32_t atendentes; FilaPrioridade fila; };

void trim(char* str) {
    if (str == nullptr) return;
    char* start = str;
    while (*start != '\0' && isspace((unsigned char)*start)) {
        start++;
    }
    char* end = start + strlen(start) - 1;
    while (end >= start && isspace((unsigned char)*end)) {
        end--;
    }
    *(end + 1) = '\0';
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

void formatarNome(char* nome) {
    if(!nome) return;
    for (uint32_t i = 0; nome[i] != '\0'; ++i) {
        if (nome[i] == ' ') {
            nome[i] = '_';
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_entrada> <arquivo_saida>" << std::endl;
        return 1;
    }

    std::ifstream arqEntrada(argv[1]);
    std::ofstream arqSaida(argv[2]);

    if (!arqEntrada.is_open() || !arqSaida.is_open()) {
        std::cerr << "Erro ao abrir arquivos." << std::endl;
        return 1;
    }

    uint32_t numOrgaos;
    arqEntrada >> numOrgaos;
    Orgao* orgaos = new Orgao[numOrgaos];
    for (uint32_t i = 0; i < numOrgaos; ++i) {
        arqEntrada >> orgaos[i].nome >> orgaos[i].atendentes;
        trim(orgaos[i].nome);
    }

    uint32_t numPessoas;
    arqEntrada >> numPessoas;
    char linha[200];
    arqEntrada.ignore(); 

    for (uint32_t i = 0; i < numPessoas; ++i) {
        arqEntrada.getline(linha, 200);

        char orgaoNome[51], pessoaNome[51];
        int idade;

        int linha_idx = 0;
        int token_idx = 0;

        while(linha[linha_idx] != '\0' && isspace(linha[linha_idx])) linha_idx++;
        while(linha[linha_idx] != '\0' && linha[linha_idx] != '|') orgaoNome[token_idx++] = linha[linha_idx++];
        orgaoNome[token_idx] = '\0';
        trim(orgaoNome);
        if(linha[linha_idx] == '|') linha_idx++;
        token_idx = 0;

        while(linha[linha_idx] != '\0' && isspace(linha[linha_idx])) linha_idx++;
        while(linha[linha_idx] != '\0' && linha[linha_idx] != '|') pessoaNome[token_idx++] = linha[linha_idx++];
        pessoaNome[token_idx] = '\0';
        trim(pessoaNome);
        if(linha[linha_idx] == '|') linha_idx++;
        
        while(linha[linha_idx] != '\0' && isspace(linha[linha_idx])) linha_idx++;
        idade = atoi(&linha[linha_idx]);
        
        Pessoa p;
        p.nome = new char[strlen(pessoaNome) + 1];
        strcpy(p.nome, pessoaNome);
        p.idade = idade;
        p.prioridade = (p.idade >= 60) ? 1 : 0;
        p.ordemChegada = i;

        for (uint32_t j = 0; j < numOrgaos; ++j) {
            if (strcmp(orgaos[j].nome, orgaoNome) == 0) {
                orgaos[j].fila.enfileirar(p);
                break;
            }
        }
    }

    bool atendimentoEmAndamento = true;
    while (atendimentoEmAndamento) {
        atendimentoEmAndamento = false;
        
        for (uint32_t i = 0; i < numOrgaos; ++i) {
            if (orgaos[i].fila.vazia()) {
                continue;
            }

            atendimentoEmAndamento = true;
            // ***** CORREÇÃO 1: Removido o espaço depois do ':' *****
            arqSaida << orgaos[i].nome << ":";

            uint32_t chamadas = 0;
            while (chamadas < orgaos[i].atendentes && !orgaos[i].fila.vazia()) {
                Pessoa atendida = orgaos[i].fila.desenfileirar();
                formatarNome(atendida.nome);
                
                if (chamadas > 0) {
                    // ***** CORREÇÃO 2: Removido o espaço depois da ',' *****
                    arqSaida << ",";
                }
                arqSaida << atendida.nome;
                chamadas++;
            }
            arqSaida << std::endl;
        }
    }

    delete[] orgaos;
    arqEntrada.close();
    arqSaida.close();

    return 0;
}
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// --- ESTRUTURAS DE DADOS (sem alterações) ---
struct Arquivo {
    std::string nome;
    long tamanho;
    std::string hash;
};

struct No {
    bool folha;
    int n;
    int k;
    std::string *C;
    void **P;
    No *prox;
    No *pai;

    No(int ordem, bool eh_folha) {
        k = ordem;
        folha = eh_folha;
        n = 0;
        C = new std::string[k]; 
        P = new void*[k + 1];
        for (int i = 0; i < k + 1; ++i) P[i] = nullptr;
        prox = nullptr;
        pai = nullptr;
    }
    ~No() {
        delete[] C;
        delete[] P;
    }
};

// --- CLASSE DA ÁRVORE B+ ---
class ArvoreBmais {
private:
    No *raiz;
    int k;

    void inserir_no_pai(No* antigo_no, std::string chave, No* novo_no) {
        if (antigo_no == raiz) {
            raiz = new No(k, false);
            raiz->C[0] = chave;
            raiz->P[0] = antigo_no;
            raiz->P[1] = novo_no;
            raiz->n = 1;
            antigo_no->pai = raiz;
            novo_no->pai = raiz;
            return;
        }
        No* pai = antigo_no->pai;
        if (pai->n < k - 1) {
            int i = 0;
            while(i < pai->n && chave >= pai->C[i]) i++;
            for(int j = pai->n; j > i; j--) pai->C[j] = pai->C[j-1];
            for(int j = pai->n + 1; j > i + 1; j--) pai->P[j] = pai->P[j-1];
            pai->C[i] = chave;
            pai->P[i+1] = novo_no;
            pai->n++;
            novo_no->pai = pai;
        } else {
            // ALTERAÇÃO: Alocação dinâmica para remover warning
            std::string* temp_C = new std::string[k]; 
            void** temp_P = new void*[k + 1];

            for (int i=0; i<k-1; i++) temp_C[i] = pai->C[i];
            for (int i=0; i<k; i++) temp_P[i] = pai->P[i];
            int i=0;
            while(i < k-1 && chave > temp_C[i]) i++;
            for(int j=k-1; j>i; j--) temp_C[j] = temp_C[j-1];
            temp_C[i] = chave;
            for(int j=k; j>i+1; j--) temp_P[j] = temp_P[j-1];
            temp_P[i+1] = novo_no;
            int meio = k/2; std::string chave_promovida = temp_C[meio];
            No* novo_pai = new No(k, false);
            pai->n = meio;
            for(int j=0; j<meio; j++){ pai->P[j] = temp_P[j]; pai->C[j] = temp_C[j]; }
            pai->P[meio] = temp_P[meio];
            novo_pai->n = (k-1)-meio;
            for(int j=0; j < novo_pai->n; j++){
                novo_pai->P[j] = temp_P[j+meio+1]; ((No*)novo_pai->P[j])->pai = novo_pai;
                novo_pai->C[j] = temp_C[j+meio+1];
            }
            novo_pai->P[novo_pai->n] = temp_P[k]; ((No*)novo_pai->P[novo_pai->n])->pai = novo_pai;
            
            // ALTERAÇÃO: Liberação da memória
            delete[] temp_C;
            delete[] temp_P;

            inserir_no_pai(pai, chave_promovida, novo_pai);
        }
    }

public:
    ArvoreBmais(int ordem) {
        if (ordem < 3) k = 3; else k = ordem;
        raiz = new No(k, true);
    }

    void inserir(Arquivo *arquivo) {
        No* folha = raiz;
        while (!folha->folha) {
            int i = 0;
            while (i < folha->n && arquivo->hash >= folha->C[i]) i++;
            ((No*)folha->P[i])->pai = folha;
            folha = (No*)folha->P[i];
        }
        if (folha->n < k - 1) {
            int i = 0;
            while (i < folha->n && arquivo->hash > folha->C[i]) i++;
            for (int j = folha->n; j > i; j--) {
                folha->C[j] = folha->C[j-1]; folha->P[j] = folha->P[j-1];
            }
            folha->C[i] = arquivo->hash; folha->P[i] = arquivo;
            folha->n++;
        } else {
            No* nova_folha = new No(k, true);
            // ALTERAÇÃO: Alocação dinâmica para remover warning
            std::string* temp_C = new std::string[k]; 
            void** temp_P = new void*[k];

            for(int i=0; i<k-1; i++) { temp_C[i] = folha->C[i]; temp_P[i] = folha->P[i]; }
            int i=0;
            while(i < k-1 && arquivo->hash > temp_C[i]) i++;
            for(int j=k-1; j>i; j--) { temp_C[j] = temp_C[j-1]; temp_P[j] = temp_P[j-1]; }
            temp_C[i] = arquivo->hash; temp_P[i] = arquivo;
            int meio = k/2;
            folha->n = meio; nova_folha->n = k - meio;
            for(i=0; i<meio; i++) { folha->C[i] = temp_C[i]; folha->P[i] = temp_P[i]; }
            for(i=0; i<nova_folha->n; i++) { nova_folha->C[i] = temp_C[i+meio]; nova_folha->P[i] = temp_P[i+meio]; }
            nova_folha->prox = folha->prox;
            folha->prox = nova_folha;

            // ALTERAÇÃO: Liberação da memória
            delete[] temp_C;
            delete[] temp_P;

            inserir_no_pai(folha, nova_folha->C[0], nova_folha);
        }
    }
    
    No* buscar_no_folha(std::string hash) {
        No* folha = raiz;
        while (folha && !folha->folha) {
            int i = 0;
            while (i < folha->n && hash >= folha->C[i]) i++;
            folha = (No*)folha->P[i];
        }
        return folha;
    }

    bool buscar_exata(std::string hash, std::ofstream& outfile) {
        outfile << "[" << hash << "]\n"; 
        No* folha = buscar_no_folha(hash);
        if (!folha) return false;
        bool encontrado = false;
        for (int i = 0; i < folha->n; i++) {
            if (folha->C[i] == hash) {
                encontrado = true;
                break;
            }
        }
        if (encontrado) {
             for (int i = 0; i < folha->n; i++) {
                Arquivo* resultado = (Arquivo*)folha->P[i];
                outfile << resultado->nome << ":size=" << resultado->tamanho << ",hash=" << resultado->hash << "\n";
            }
        }
        return encontrado;
    }

    void buscar_intervalo(std::string hash_inicio, std::string hash_fim, std::ofstream& outfile) {
        outfile << "[" << hash_inicio << "," << hash_fim << "]\n";
        No* folha_atual = buscar_no_folha(hash_inicio);
        if(!folha_atual) return;

        while (folha_atual != nullptr) {
            if (folha_atual->n > 0 && folha_atual->C[0] > hash_fim) break;
            bool no_e_relevante = false;
            for (int i = 0; i < folha_atual->n; i++) {
                if (folha_atual->C[i] >= hash_inicio && folha_atual->C[i] <= hash_fim) {
                    no_e_relevante = true;
                    break;
                }
            }
            if(no_e_relevante) {
                for (int i = 0; i < folha_atual->n; i++) {
                    Arquivo* arq = (Arquivo*)folha_atual->P[i];
                    outfile << arq->nome << ":size=" << arq->tamanho << ",hash=" << arq->hash << "\n";
                }
            }
            folha_atual = folha_atual->prox;
        }
    }
};

// --- FUNÇÃO MAIN COM A LÓGICA FINAL DO SEPARADOR ---
int main(int argc, char* argv[]) {
    if (argc != 3) { return 1; }

    std::ifstream infile(argv[1]);
    std::ofstream outfile(argv[2]);
    
    int ordem_arvore; infile >> ordem_arvore;
    ArvoreBmais arvore(ordem_arvore);

    int num_arquivos; infile >> num_arquivos;
    Arquivo* pool_de_arquivos = new Arquivo[num_arquivos + 200]; 
    int proximo_arquivo_idx = 0;

    for (int i = 0; i < num_arquivos; ++i) {
        infile >> pool_de_arquivos[proximo_arquivo_idx].nome 
               >> pool_de_arquivos[proximo_arquivo_idx].tamanho 
               >> pool_de_arquivos[proximo_arquivo_idx].hash;
        arvore.inserir(&pool_de_arquivos[proximo_arquivo_idx]);
        proximo_arquivo_idx++;
    }

    int num_operacoes; infile >> num_operacoes;
    std::string linha_inteira;
    std::getline(infile, linha_inteira); 

    for (int i = 0; i < num_operacoes; ++i) {
        if (!std::getline(infile, linha_inteira) || linha_inteira.empty()) { i--; continue; }
        
        std::stringstream ss(linha_inteira);
        std::string operacao; ss >> operacao;

        if (operacao == "INSERT") {
            ss >> pool_de_arquivos[proximo_arquivo_idx].nome 
               >> pool_de_arquivos[proximo_arquivo_idx].tamanho 
               >> pool_de_arquivos[proximo_arquivo_idx].hash;
            arvore.inserir(&pool_de_arquivos[proximo_arquivo_idx]);
        } else if (operacao == "SELECT") {
            std::string token1; ss >> token1;
            if (token1 == "RANGE") {
                std::string hash1, hash2;
                ss >> hash1 >> hash2;
                arvore.buscar_intervalo(hash1, hash2, outfile);
                if (i < num_operacoes - 1) {
                    outfile << "-\n";
                }
            } else {
                bool encontrado = arvore.buscar_exata(token1, outfile);
                if (!encontrado && i < num_operacoes - 1) {
                    outfile << "-\n";
                }
            }
        }
    }
    
    delete[] pool_de_arquivos;
    infile.close();
    outfile.close();

    return 0;
}
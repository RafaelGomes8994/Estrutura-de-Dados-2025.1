#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <utility> // Para std::swap

// Nenhuma outra biblioteca é necessária

struct Livro {
    long long isbn;
    std::string autor;
    std::string titulo;
};

// --- Algoritmos de Ordenação Otimizados (Corretos) ---
void insertionSort(Livro livros[], int baixo, int alto) {
    for (int p = baixo + 1; p <= alto; ++p) {
        Livro tmp = std::move(livros[p]);
        int j;
        for (j = p; j > baixo && tmp.isbn < livros[j - 1].isbn; --j) {
            livros[j] = std::move(livros[j - 1]);
        }
        livros[j] = std::move(tmp);
    }
}

int particionar(Livro livros[], int baixo, int alto, long long pivo) {
    int i = baixo;
    int j = alto - 1;
    for (;;) {
        while (livros[++i].isbn < pivo) {}
        while (pivo < livros[--j].isbn) {}
        if (i < j) {
            std::swap(livros[i], livros[j]);
        } else {
            break;
        }
    }
    std::swap(livros[i], livros[alto - 1]);
    return i;
}

void quickSortRecursivo(Livro livros[], int baixo, int alto) {
    if (alto - baixo <= 10) {
        insertionSort(livros, baixo, alto);
        return;
    }
    int meio = (baixo + alto) / 2;
    if (livros[meio].isbn < livros[baixo].isbn) std::swap(livros[baixo], livros[meio]);
    if (livros[alto].isbn < livros[baixo].isbn) std::swap(livros[baixo], livros[alto]);
    if (livros[alto].isbn < livros[meio].isbn) std::swap(livros[meio], livros[alto]);
    std::swap(livros[meio], livros[alto - 1]);
    long long pivo = livros[alto - 1].isbn;
    int pi = particionar(livros, baixo, alto, pivo);
    quickSortRecursivo(livros, baixo, pi - 1);
    quickSortRecursivo(livros, pi + 1, alto);
}


// --- Algoritmos de Busca ---

// --- FUNÇÃO SUBSTITUÍDA ---
// Esta é a nova busca binária com a lógica da função 'bbi'
int buscaBinaria(const Livro livros[], int n, long long isbn_procurado, int& chamadas) {
    chamadas = 0;
    if (n == 0) {
        return -1;
    }

    int baixo = 0;
    int alto = n - 1;
    
    // Calcula o pivô inicial, antes do laço
    int pivo = baixo + (alto - baixo) / 2;
    
    // O laço só continua se o pivô não for o alvo
    while (alto >= baixo && livros[pivo].isbn != isbn_procurado) {
        chamadas++; // Conta a iteração do laço
        if (livros[pivo].isbn > isbn_procurado) {
            alto = pivo - 1;
        } else {
            baixo = pivo + 1;
        }
        
        // Recalcula o pivô para a próxima iteração
        if (alto >= baixo) {
            pivo = baixo + (alto - baixo) / 2;
        }
    }

    // Se o laço terminou, verificamos a razão
    if (alto < baixo) {
        chamadas++; // Conta a última verificação que falhou
        return -1;   // Não encontrado
    } else {
        chamadas++; // Conta a verificação bem-sucedida que parou o laço
        return pivo; // Encontrado
    }
}


// A busca "interpolada" que na verdade usa a heurística de módulo
int buscaInterpolada(const Livro livros[], int n, long long isbn_procurado, int& chamadas) {
    int baixo = 0, alto = n - 1;
    chamadas = 0;
    while (baixo <= alto) {
        chamadas++; // Conta a iteração do laço
        if (baixo == alto) {
            if (livros[baixo].isbn == isbn_procurado) return baixo;
            break; 
        }

        int tamanho_intervalo = alto - baixo + 1;
        long long pos = baixo + ((livros[alto].isbn - livros[baixo].isbn) % tamanho_intervalo);
        
        if (pos > alto) {
            pos = alto;
        }
        if (pos < baixo) {
            pos = baixo;
        }

        if (livros[pos].isbn == isbn_procurado) {
            return pos;
        }
        
        if (livros[pos].isbn < isbn_procurado) {
            baixo = pos + 1;
        } else {
            alto = pos - 1;
        }
    }
    return -1;
}


// --- Função Principal ---
int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_de_entrada> <arquivo_de_saida>" << std::endl;
        return 1;
    }

    std::ifstream arquivo_entrada(argv[1]);
    std::ofstream arquivo_saida(argv[2]);

    if (!arquivo_entrada.is_open() || !arquivo_saida.is_open()) {
        std::cerr << "Erro ao abrir arquivos." << std::endl;
        return 1;
    }

    int n_livros;
    arquivo_entrada >> n_livros;
    arquivo_entrada.ignore();

    Livro* livros = new Livro[n_livros];

    for (int i = 0; i < n_livros; ++i) {
        std::string linha;
        std::getline(arquivo_entrada, linha);
        std::stringstream ss(linha);
        std::string isbn_str;
        ss >> isbn_str;
        livros[i].isbn = std::stoll(isbn_str);
        std::string resto;
        std::getline(ss, resto);
        size_t pos_ampersand = resto.find('&');
        if (pos_ampersand != std::string::npos) {
            livros[i].autor = resto.substr(1, pos_ampersand - 1);
            livros[i].titulo = resto.substr(pos_ampersand + 1);
        }
        for (char &c : livros[i].autor) { if (c == ' ') c = '_'; }
        for (char &c : livros[i].titulo) { if (c == ' ') c = '_'; }
    }

    if (n_livros > 1) {
        quickSortRecursivo(livros, 0, n_livros - 1);
    }

    int n_consultas;
    arquivo_entrada >> n_consultas;
    
    int vitorias_binaria = 0, vitorias_interpolacao = 0;
    long long total_chamadas_binaria = 0, total_chamadas_interpolacao = 0;

    for (int i = 0; i < n_consultas; ++i) {
        long long isbn_consulta;
        arquivo_entrada >> isbn_consulta;

        int chamadas_b, chamadas_i;
        int indice_b = buscaBinaria(livros, n_livros, isbn_consulta, chamadas_b);
        buscaInterpolada(livros, n_livros, isbn_consulta, chamadas_i);

        total_chamadas_binaria += chamadas_b;
        total_chamadas_interpolacao += chamadas_i;

        // A lógica de vitória que o seu código mais recente usava
        if (chamadas_b < chamadas_i) {
            vitorias_binaria++;
        } else if (chamadas_i <= chamadas_b) {
            vitorias_interpolacao++;
        }

        arquivo_saida << "[" << isbn_consulta << "]B=" << chamadas_b << "|I=" << chamadas_i << "|";
        if (indice_b != -1) {
            arquivo_saida << "Author:" << livros[indice_b].autor << ",Title:" << livros[indice_b].titulo << "\n";
        } else {
            arquivo_saida << "ISBN_NOT_FOUND\n";
        }
    }

    int media_b = (n_consultas > 0) ? total_chamadas_binaria / n_consultas : 0;
    int media_i = (n_consultas > 0) ? total_chamadas_interpolacao / n_consultas : 0;

    arquivo_saida << "BINARY=" << vitorias_binaria << ":" << media_b << "\n";
    arquivo_saida << "INTERPOLATION=" << vitorias_interpolacao << ":" << media_i << "\n";

    delete[] livros;
    arquivo_entrada.close();
    arquivo_saida.close();

    return 0;
}
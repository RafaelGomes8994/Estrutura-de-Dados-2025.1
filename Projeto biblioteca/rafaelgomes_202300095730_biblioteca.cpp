#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <utility> // Para std::swap

struct Livro {
    long long isbn;
    std::string autor;
    std::string titulo;
};


// --- Algoritmos de Ordenação Otimizados ---

/**
 * @brief Insertion Sort (Ordenação por Inserção).
 * Eficiente para arrays pequenos. Usado como parte da otimização do Quicksort.
 */
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

/**
 * @brief Particiona o subarray para o Quicksort.
 * Otimizado para trabalhar com o pivô selecionado pela "Mediana de Três".
 */
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
    std::swap(livros[i], livros[alto - 1]); // Restaura o pivô
    return i;
}


/**
 * @brief Função recursiva principal do Quicksort com otimizações.
 * @param livros Array a ser ordenado.
 * @param baixo Índice inicial.
 * @param alto Índice final.
 */
void quickSortRecursivo(Livro livros[], int baixo, int alto) {
    // Otimização 1: Usar Insertion Sort para subarrays pequenos (cutoff)
    if (alto - baixo <= 10) {
        insertionSort(livros, baixo, alto);
        return;
    }

    // Otimização 2: Estratégia de pivô "Mediana de Três"
    int meio = (baixo + alto) / 2;
    if (livros[meio].isbn < livros[baixo].isbn) std::swap(livros[baixo], livros[meio]);
    if (livros[alto].isbn < livros[baixo].isbn) std::swap(livros[baixo], livros[alto]);
    if (livros[alto].isbn < livros[meio].isbn) std::swap(livros[meio], livros[alto]);
    
    // O pivô é o elemento do meio, que colocamos na penúltima posição
    std::swap(livros[meio], livros[alto - 1]);
    long long pivo = livros[alto - 1].isbn;

    // Particiona e chama recursivamente
    int pi = particionar(livros, baixo, alto, pivo);
    quickSortRecursivo(livros, baixo, pi - 1);
    quickSortRecursivo(livros, pi + 1, alto);
}


// --- Algoritmos de Busca (Sem alterações, já eram robustos) ---
int buscaBinaria(const Livro livros[], int n, long long isbn_procurado, int& chamadas) {
    int baixo = 0, alto = n - 1;
    chamadas = 0;
    while (baixo <= alto) {
        chamadas++;
        int meio = baixo + (alto - baixo) / 2;
        if (livros[meio].isbn == isbn_procurado) return meio;
        if (livros[meio].isbn < isbn_procurado) baixo = meio + 1;
        else alto = meio - 1;
    }
    return -1;
}

int buscaInterpolada(const Livro livros[], int n, long long isbn_procurado, int& chamadas) {
    int baixo = 0, alto = n - 1;
    chamadas = 0;
    while (baixo <= alto && isbn_procurado >= livros[baixo].isbn && isbn_procurado <= livros[alto].isbn) {
        chamadas++;
        if (livros[alto].isbn == livros[baixo].isbn) {
            if (livros[baixo].isbn == isbn_procurado) return baixo;
            return -1;
        }
        long long pos = baixo + (((double)(isbn_procurado - livros[baixo].isbn) * (alto - baixo)) / (livros[alto].isbn - livros[baixo].isbn));
        if (pos < baixo || pos > alto) break;
        if (livros[pos].isbn == isbn_procurado) return pos;
        if (livros[pos].isbn < isbn_procurado) baixo = pos + 1;
        else alto = pos - 1;
    }
    return -1;
}


// --- Função Principal (main) (Sem alterações na lógica principal) ---
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
        std::getline(ss, isbn_str, '&');
        std::getline(ss, livros[i].autor, '&');
        std::getline(ss, livros[i].titulo);
        livros[i].isbn = std::stoll(isbn_str);
    }

    // Chama a função de ordenação otimizada
    if (n_livros > 1) {
        quickSortRecursivo(livros, 0, n_livros - 1);
    }

    int n_consultas;
    arquivo_entrada >> n_consultas;
    
    int vitorias_binaria = 0, vitorias_interpolacao = 0;
    int total_chamadas_binaria = 0, total_chamadas_interpolacao = 0;

    for (int i = 0; i < n_consultas; ++i) {
        long long isbn_consulta;
        arquivo_entrada >> isbn_consulta;

        int chamadas_b, chamadas_i;
        int indice_b = buscaBinaria(livros, n_livros, isbn_consulta, chamadas_b);
        buscaInterpolada(livros, n_livros, isbn_consulta, chamadas_i);
        
        total_chamadas_binaria += chamadas_b;
        total_chamadas_interpolacao += chamadas_i;

        if (chamadas_b < chamadas_i) vitorias_binaria++;
        else vitorias_interpolacao++;

        arquivo_saida << "[" << isbn_consulta << "] B=" << chamadas_b << "|I=" << chamadas_i << "|";
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
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <utility>

struct Livro {
    long long isbn;
    std::string autor;
    std::string titulo;
};

void insertionSort(Livro livros[], int baixo, int alto) {
    for (int p = baixo + 1; p <= alto; ++p) {
        Livro tmp = std::move(livros[p]);
        int j = p;
        while (j > baixo && tmp.isbn < livros[j - 1].isbn) {
            livros[j] = std::move(livros[j - 1]);
            --j;
        }
        livros[j] = std::move(tmp);
    }
}

int particionar(Livro livros[], int baixo, int alto, long long pivo) {
    int i = baixo, j = alto - 1;
    while (true) {
        while (livros[++i].isbn < pivo) {}
        while (pivo < livros[--j].isbn) {}
        if (i < j) std::swap(livros[i], livros[j]);
        else break;
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

int buscaBinaria(const Livro livros[], int n, long long isbn_procurado, int& chamadas) {
    chamadas = 0;
    if (n == 0) return -1;
    int baixo = 0, alto = n - 1;
    int pivo = baixo + (alto - baixo) / 2;
    while (alto >= baixo && livros[pivo].isbn != isbn_procurado) {
        ++chamadas;
        if (livros[pivo].isbn > isbn_procurado) alto = pivo - 1;
        else baixo = pivo + 1;
        if (alto >= baixo) pivo = baixo + (alto - baixo) / 2;
    }
    if (alto < baixo) {
        ++chamadas;
        return -1;
    } else {
        ++chamadas;
        return pivo;
    }
}

int buscaInterpolada(const Livro livros[], int n, long long isbn_procurado, int& chamadas) {
    int baixo = 0, alto = n - 1;
    chamadas = 0;
    while (baixo <= alto) {
        ++chamadas;
        if (baixo == alto) {
            if (livros[baixo].isbn == isbn_procurado) return baixo;
            break;
        }
        int tamanho_intervalo = alto - baixo + 1;
        long long pos = baixo + ((livros[alto].isbn - livros[baixo].isbn) % tamanho_intervalo);
        if (pos > alto) pos = alto;
        if (pos < baixo) pos = baixo;
        if (livros[pos].isbn == isbn_procurado) return pos;
        if (livros[pos].isbn < isbn_procurado) baixo = pos + 1;
        else alto = pos - 1;
    }
    return -1;
}

void substituirEspacos(std::string& str) {
    for (char& c : str) if (c == ' ') c = '_';
}

bool lerLivro(std::istream& in, Livro& livro) {
    std::string linha;
    if (!std::getline(in, linha)) return false;
    std::stringstream ss(linha);
    std::string isbn_str;
    ss >> isbn_str;
    livro.isbn = std::stoll(isbn_str);
    std::string resto;
    std::getline(ss, resto);
    size_t pos_ampersand = resto.find('&');
    if (pos_ampersand != std::string::npos) {
        livro.autor = resto.substr(1, pos_ampersand - 1);
        livro.titulo = resto.substr(pos_ampersand + 1);
    }
    substituirEspacos(livro.autor);
    substituirEspacos(livro.titulo);
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_de_entrada> <arquivo_de_saida>\n";
        return 1;
    }
    std::ifstream arquivo_entrada(argv[1]);
    std::ofstream arquivo_saida(argv[2]);
    if (!arquivo_entrada.is_open() || !arquivo_saida.is_open()) {
        std::cerr << "Erro ao abrir arquivos.\n";
        return 1;
    }
    int n_livros;
    arquivo_entrada >> n_livros;
    arquivo_entrada.ignore();
    Livro* livros = new Livro[n_livros];
    for (int i = 0; i < n_livros; ++i) {
        if (!lerLivro(arquivo_entrada, livros[i])) {
            std::cerr << "Erro ao ler livro.\n";
            delete[] livros;
            return 1;
        }
    }
    if (n_livros > 1) quickSortRecursivo(livros, 0, n_livros - 1);
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
        if (chamadas_b < chamadas_i) ++vitorias_binaria;
        else ++vitorias_interpolacao;
        arquivo_saida << "[" << isbn_consulta << "]B=" << chamadas_b << "|I=" << chamadas_i << "|";
        if (indice_b != -1)
            arquivo_saida << "Author:" << livros[indice_b].autor << ",Title:" << livros[indice_b].titulo << "\n";
        else
            arquivo_saida << "ISBN_NOT_FOUND\n";
    }
    int media_b = (n_consultas > 0) ? total_chamadas_binaria / n_consultas : 0;
    int media_i = (n_consultas > 0) ? total_chamadas_interpolacao / n_consultas : 0;
    arquivo_saida << "BINARY=" << vitorias_binaria << ":" << media_b << "\n";
    arquivo_saida << "INTERPOLATION=" << vitorias_interpolacao << ":" << media_i << "\n";
    delete[] livros;
    return 0;
}

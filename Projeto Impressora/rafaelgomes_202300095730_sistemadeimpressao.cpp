#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const int MAX_PRINTERS = 100;
const int MAX_DOCS = 1000;

struct Document {
    string name;
    int pages;
    int index;
};

struct Printer {
    string name;
    int free_time;
    Document queue[MAX_DOCS];
    int queue_size;
};

struct Event {
    string name;
    int pages;
    int end_time;
    int index;
};

int main(int argc, char* argv[]) {
    if (argc < 3) return 1;
    ifstream entrada(argv[1]);
    ofstream saida(argv[2]);
    int N;
    entrada >> N;
    Printer printers[MAX_PRINTERS];
    for (int i = 0; i < N; ++i) {
        entrada >> printers[i].name;
        printers[i].free_time = 0;
        printers[i].queue_size = 0;
    }
    int M;
    entrada >> M;
    Document docs[MAX_DOCS];
    for (int i = 0; i < M; ++i) {
        entrada >> docs[i].name >> docs[i].pages;
        docs[i].index = i;
    }

    // Alocação dos documentos
    for (int i = 0; i < M; ++i) {
        // Encontra impressora com menor free_time
        int best = 0;
        for (int j = 1; j < N; ++j)
            if (printers[j].free_time < printers[best].free_time)
                best = j;
        printers[best].queue[printers[best].queue_size] = docs[i];
        printers[best].queue_size++;
        printers[best].free_time += docs[i].pages;

        // Impressão do estado atual da fila da impressora escolhida
        saida << printers[best].name << ":";
        for (int k = 0; k < printers[best].queue_size; ++k) {
            if (k) saida << ",";
            saida << printers[best].queue[k].name << "-" << printers[best].queue[k].pages << "p";
        }
        saida << endl;
    }

    // Empilhamento dos eventos (folhas)
    Event events[MAX_DOCS];
    int events_size = 0;
    for (int i = 0; i < N; ++i) {
        int t = 0;
        for (int j = 0; j < printers[i].queue_size; ++j) {
            t += printers[i].queue[j].pages;
            events[events_size].name = printers[i].queue[j].name;
            events[events_size].pages = printers[i].queue[j].pages;
            events[events_size].end_time = t;
            events[events_size].index = printers[i].queue[j].index;
            events_size++;
        }
    }
    // Ordena por end_time decrescente, depois index crescente (bubble sort)
    for (int i = 0; i < events_size - 1; ++i) {
        for (int j = 0; j < events_size - i - 1; ++j) {
            if (events[j].end_time < events[j + 1].end_time ||
                (events[j].end_time == events[j + 1].end_time && events[j].index > events[j + 1].index)) {
                Event tmp = events[j];
                events[j] = events[j + 1];
                events[j + 1] = tmp;
            }
        }
    }

    int total = 0;
    for (int i = 0; i < M; ++i) total += docs[i].pages;
    saida << total << "p" << endl;
    for (int i = 0; i < events_size; ++i)
        saida << events[i].name << "-" << events[i].pages << "p" << endl;

    return 0;
}

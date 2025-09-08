# Estrutura de Dados - 2025.1

Este repositório contém todos os projetos e materiais de aula desenvolvidos para a disciplina de Estrutura de Dados.

## Estrutura do Repositório

O conteúdo está organizado em unidades, correspondendo aos períodos do curso, além de uma pasta com os slides das aulas.

-   `Primeira Unidade/`: Projetos focados em estruturas de dados lineares, hashing e algoritmos de busca.
-   `Segunda Unidade/`: Projetos focados em estruturas de dados em árvore.
-   `Terceira Unidade/`: Projetos focados em estruturas de dados avançadas e suas aplicações.
-   `Slides/`: Apresentações em PDF com o conteúdo teórico da disciplina.

---

## Projetos Desenvolvidos

### Primeira Unidade

-   **Projeto Redes Sociais**: Implementação de um círculo de amigos usando uma lista duplamente encadeada circular para simular uma rede social.
    -   Arquivo principal: [`Primeira Unidade/Projeto Redes Sociais/redesocial.cpp`](Primeira%20Unidade/Projeto%20Redes%20Sociais/redesocial.cpp)
-   **Projeto Impressora**: Simulação de um sistema de gerenciamento de impressão que distribui documentos para várias impressoras, otimizando o tempo com um min-heap.
    -   Arquivo principal: [`Primeira Unidade/Projeto Impressora/sistemadeimpressao.cpp`](Primeira%20Unidade/Projeto%20Impressora/sistemadeimpressao.cpp)
-   **Projeto Engenho de Busca**: Simulação de um balanceador de carga para um motor de busca, utilizando hashing para distribuir requisições entre servidores.
    -   Arquivo principal: [`Primeira Unidade/Projeto engenho de Busca/engenhodebusca.cpp`](Primeira%20Unidade/Projeto%20engenho%20de%20Busca/engenhodebusca.cpp)
-   **Projeto Biblioteca**: Comparação de desempenho entre busca binária e busca por interpolação para encontrar livros em um acervo ordenado por ISBN.
    -   Arquivo principal: [`Primeira Unidade/Projeto biblioteca/biblioteca.cpp`](Primeira%20Unidade/Projeto%20biblioteca/biblioteca.cpp)

### Segunda Unidade

-   **Projeto Árvore Binária**: Implementação de uma árvore binária de busca com operações de inserção e percursos (pré-ordem, em-ordem, pós-ordem).
    -   Arquivo principal: [`Segunda Unidade/Projeto Arvore binaria/rafaelgomes_202300095730_arvorebinaria.cpp`](Segunda%20Unidade/Projeto%20Arvore%20binaria/rafaelgomes_202300095730_arvorebinaria.cpp)
-   **Projeto Dicionário**: Implementação de um dicionário de sinônimos utilizando uma Árvore AVL para manter o balanceamento e garantir buscas eficientes.
    -   Arquivo principal: [`Segunda Unidade/Projeto Dicionario/rafaelgomes_202300095730_dicionario.cpp`](Segunda%20Unidade/Projeto%20Dicionario/rafaelgomes_202300095730_dicionario.cpp)
-   **Projeto PoximDB**: Implementação de um sistema de gerenciamento de arquivos simulado, utilizando uma Árvore B para indexação e busca de registros.
    -   Arquivo principal: [`Segunda Unidade/Projeto PoximDB/poximdb.cpp`](Segunda%20Unidade/Projeto%20PoximDB/poximdb.cpp)
-   **Projeto PoximDBPlus**: Uma versão aprimorada do PoximDB, utilizando uma Árvore B+ para otimizar buscas exatas e por intervalo.
    -   Arquivo principal: [`Segunda Unidade/Projeto PoximDBPLus/poximdbplus.cpp`](Segunda%20Unidade/Projeto%20PoximDBPLus/poximdbplus.cpp)

### Terceira Unidade

-   **Projeto Controle de Senha**: Sistema para gerenciamento de filas de atendimento em múltiplos órgãos, utilizando filas de prioridade (implementadas com heap) para organizar as senhas.
    -   Arquivo principal: [`Terceira Unidade/Projeto controle de senha/controledesenha.cpp`](Terceira%20Unidade/Projeto%20controle%20de%20senha/controledesenha.cpp)
-   **Projeto Propagação**: Simulação de um sistema de loteria que utiliza conjuntos disjuntos (Union-Find) para agrupar apostadores que compartilham os mesmos números e determinar os vencedores.
    -   Arquivo principal: [`Terceira Unidade/Projeto Propagacao/propagacao.cpp`](Terceira%20Unidade/Projeto%20Propagacao/propagacao.cpp)
-   **Projeto Autocompletar**: Implementação de um sistema de autocompletar palavras utilizando uma Árvore Trie (Prefix Tree).
    -   Arquivo principal: [`Terceira Unidade/Projeto Autocompletar/autocompletar.cpp`](Terceira%20Unidade/Projeto%20Autocompletar/autocompletar.cpp)
-   **Projeto Loteria**: Simulação de um processo de propagação em uma matriz, como um incêndio ou um boato, utilizando algoritmos de busca em largura (BFS).
    -   Arquivo principal: [`Terceira Unidade/Projeto Loteria/loteria.cpp`](Terceira%20Unidade/Projeto%20Loteria/loteria.cpp)

---

## Material de Aula

A pasta [`Slides/`](Slides) contém o material teórico utilizado nas aulas, cobrindo tópicos como:
-   Ponteiros e Alocação de Memória
-   Complexidade de Algoritmos
-   Listas, Filas e Pilhas
-   Árvores (Binária, AVL, B, B+)
-   Heaps e Filas de Prioridade
-   Conjuntos
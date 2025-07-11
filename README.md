# Estruturas de Dados Avançadas - Árvore Van Emde Boas

Uma implementação da **Árvore Van Emde Boas** com espaço linear usando tabela de dispersão com *table doubling/halving*.

Segue abaixo a especificação das funções e estruturas. Iremos utilizar uma **Tabela Hash encadeada** com uma **função de hash** de módulo simples.

```c++
class Node {
    public:
        long key;
        VEBTree* pointer;
        Node* next;

        Node(VEBTree* tree) {
            this->key = -1;
            this->pointer = tree;
            this->next = NULL;
        }
};
```

```c++
class HashTable {
    public:
        long table_size;
        long number_clusters;
        Node** clusters;

        long hashFunction(long k);
        long inserirChave(long k, VEBTree* tree);
        long removerChave(long k);
        std::pair<long, Node*> buscarChave(long k);
        void tableChanging(float ratio);
        void tableDoubling();
        void tableHalving();

        HashTable() {
            this->table_size = initial_size;
            this->number_clusters = 0;
            this->clusters = (struct Node**)malloc(sizeof(struct Node*) * initial_size);
            this->clusters[0] = NULL, this->clusters[1] = NULL;
        }
};
```

A árvore van Emde Boas é inicializada com um universo de chaves de tamanho 2^32 = 4294967296. 

```c++
class VEBTree {
    public:
        long min;
        long max;
        long universe_size;
        long w;
        long number_clusters;
        VEBTree* resumo;
        HashTable clusters;

        void Incluir(long x);
        void Remover(long x);
        long Predecessor(long x);
        long Sucessor(long x);
        void Imprimir(ofstream& Output);
        void print(const string &prefix);
        vector<long> getCluster();

        VEBTree(long u) {
            this->min = None;
            this->universe_size = u;
            this->w = log2(u);
            this->number_clusters = sqrt(u);

            this->resumo = NULL;
            if (universe_size > 2) this->clusters = HashTable();
        }
};
```

Se um elemento não possui predecessor (respec. sucessor), a linha retornada será "Não tem predecessor" (respec. "Não tem sucessor"). Exemplo no arquivo `saida.txt`:

```
PRE 1
Não tem predecessor
SUC 6553678
Não tem sucessor
```

### Instruções

Para executar, compile o arquivo main.cpp:

```
g++ main.cpp
```

Se desejar, pode adicionar a tag `-o` seguida de um nome para o arquivo compilado. Por exemplo:

```
g++ main.cpp -o vebtree.out
```

Caso um nome para o arquivo compilado não seja especificado, será criado um arquivo chamado `a.out` (ou `a.exe` no caso do Windows). Execute o arquivo passando como parâmetro o caminho para o arquivo com as operações:

```
./a.out ./entrada3.txt
```

Um arquivo chamado `saida.txt` será gerado com as impressões e resultados das operações de sucessor, predecessor e imprimir especificadas no arquivo de entrada.
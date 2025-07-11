#include <iostream>
#include <optional>
#include <iomanip>
#include <cmath>
#include <fstream> 
#include <string>
#include <vector>
using namespace std;

#define U 4294967296
#define N_CLUSTERS 65536
#define initial_size 2
#define None -1

class Node;
class HashTable;
class VEBTree;

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

long HashTable::hashFunction(long k) {
    return k % this->table_size;
}

long HashTable::inserirChave(long k, VEBTree* tree) {
    Node* newNode = new Node(tree);
    newNode->key = k;
    long index = this->hashFunction(k);
    if(this->clusters[index] == NULL) this->clusters[index] = newNode;
    else {
        newNode->next = this->clusters[index];
        this->clusters[index] = newNode;
    }
    this->number_clusters += 1;
    if (this->number_clusters / this->table_size >= 0.75) this->tableDoubling();
    return index;
}

long HashTable::removerChave(long k) {
    long index = this->hashFunction(k);
    struct Node* prevNode = NULL;
    struct Node* currNode = this->clusters[index];
    while (currNode != NULL) {
        if (k == currNode->key) {
            if (currNode == this->clusters[index]) this->clusters[index] = NULL;
            else prevNode->next = currNode->next;
            free(currNode);
            this->number_clusters -= 1;
            if (this->table_size > 2 && this->number_clusters / this->table_size <= 0.25) this->tableHalving();
            break;
        }
        prevNode = currNode;
        currNode = currNode->next;
    }
    return index;
}

std::pair<long, Node*> HashTable::buscarChave(long k) {
    long index = this->hashFunction(k);
    struct Node* bucketHead = this->clusters[index];
    while (bucketHead != NULL) {
        if (bucketHead->key == k) {
            return std::pair<long, Node*> {index, bucketHead};
        }
        bucketHead = bucketHead->next;
    }
    return std::pair<long, Node*> {-1, bucketHead};
}

void HashTable::tableChanging(float ratio) {
    Node** newTable = (struct Node**)malloc(sizeof(struct Node*) * this->table_size * ratio);
    for (auto i = 0; i < this->table_size * ratio; i++) newTable[i] = NULL;
    Node** oldTable = this->clusters;
    long aux = this->table_size;
    this->number_clusters = 0;
    this->table_size = long(this->table_size * ratio);
    this->clusters = newTable;
    for (long i = 0; i < aux; i++) {
        struct Node* bucketHead = oldTable[i];
        while (bucketHead != NULL) {
            this->inserirChave(bucketHead->key, bucketHead->pointer);
            bucketHead = bucketHead->next;
        }
    }
}

void HashTable::tableDoubling() {
    this->tableChanging(2);
}

void HashTable::tableHalving() {
    this->tableChanging(0.5);
}

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

void VEBTree::Incluir(long x) {
    if (this->min == None) {
        this->min = x;
        this->max = this->min;
    } else {
        if (x < this->min) {
            long aux = this->min;
            this->min = x;
            x = aux;
        }
        if (x > this->max) {
            this->max = x;
        }
        if (this->universe_size > 2) {
            long w_ = this->w >> 1;
            long c = x >> w_;   
            long i = x & ((1 << w_) - 1);
            std::pair<long, Node*> cluster = this->clusters.buscarChave(c);
            if (cluster.first == -1) {
                if (!this->resumo) this->resumo = new VEBTree(sqrt(this->universe_size));
                this->resumo->Incluir(c);
                this->clusters.inserirChave(c, new VEBTree(sqrt(this->universe_size)));
                cluster = this->clusters.buscarChave(c);
            }
            cluster.second->pointer->Incluir(i);
        }
    }
}

void VEBTree::Remover(long x) {
    if (this->universe_size <= 2) {
        if (x == this->min) {
            if (this->min == this->max) this->min = None, this->max = None;
            else this->min = this->max;
        } else this->max = None;
        return;
    }
    long w_ = this->w >> 1;
    long c = x >> w_;
    long i = x & ((1 << w_) - 1);
    std::pair<long, Node*> cluster;
    if (x == this->min) {
        if(this->resumo) {
            c = this->resumo->min;
            if (c == None) {
                this->min = None;
                return;
            }
            cluster = this->clusters.buscarChave(c);
            i = cluster.second->pointer->min;
            x = (c << w_) | i;
            this->min = x;
        } else this->min = None, this->max = None;
    }
    cluster = this->clusters.buscarChave(c);
    if (cluster.first != -1) {
        cluster.second->pointer->Remover(i);
        if (cluster.second->pointer->min == None) {
            this->clusters.removerChave(c);
            this->resumo->Remover(c);
        }
    }
    if (this->resumo) {
        if (this->resumo->min == None) {
            free(this->resumo);
            this->resumo = NULL;
            this->max = this->min;
        } else {
            long c_ = this->resumo->max;
            cluster = this->clusters.buscarChave(c_);
            long i_ = cluster.second->pointer->max;
            long x_ = (c_ << w_) | i_;
            this->max = x_;
        }
    }
}

long VEBTree::Sucessor(long x) {
    if (this->min == None || x >= this->max) {
        return -1;
    }
    if (x < this->min) {
        return this->min;
    }
    if (this->universe_size <= 2) return this->max;
    long w_ = this->w >> 1;
    long c = x >> w_;
    long i = x & ((1 << w_) - 1);
    long x_, i_;
    std::pair<long, Node*> cluster = this->clusters.buscarChave(c);
    if (cluster.second != NULL && i < cluster.second->pointer->max) {
        i_ = cluster.second->pointer->Sucessor(i);
        if (i_ == None) {
            return i_;
        }
        x_ = (c << w_) | i_;
        return x_;
    }
    long c_ = this->resumo->Sucessor(c);
    if (c_ == None) {
        return c_;
    }
    cluster = this->clusters.buscarChave(c_);
    i_ = cluster.second->pointer->min;
    x_ = (c_ << w_) | i_;
    return x_;
}

long VEBTree::Predecessor(long x) {
    if (this->min == None || x <= this->min) {
        return -1;
    }
    if (x > this->max) {
        return this->max;
    }
    if (this->universe_size <= 2) return this->min;
    long w_ = this->w >> 1;
    long c = x >> w_;
    long i = x & ((1 << w_) - 1);
    long x_, i_;
    std::pair<long, Node*> cluster = this->clusters.buscarChave(c);
    if (cluster.second != NULL && i > cluster.second->pointer->min) {
        i_ = cluster.second->pointer->Predecessor(i);
        if (i_ == None) {
            return i_;
        }
        x_ = (c << w_) | i_;
        return x_;
    }
    long c_ = this->resumo->Predecessor(c);
    if (c_ == None) {
        return this->min;
    }
    cluster = this->clusters.buscarChave(c_);
    i_ = cluster.second->pointer->max;
    x_ = (c_ << w_) | i_;
    return x_;
}

void VEBTree::Imprimir(ofstream& Output) {
    if (this->min == None) {
        cout << "Arvore vazia" << endl;
        Output << "Arvore vazia" << endl;
        return;
    }
    Output << "Min: " << this->min;
    cout << "Min: " << this->min;
    for (long i = 0; i < this->number_clusters; i++) {
        std::pair<int, Node*> cluster = this->clusters.buscarChave(i);
        if (cluster.first != -1) {
            Output << ", C[" << i << "]:";
            cout << ", C[" << i << "]:";
            vector<long> elements = cluster.second->pointer->getCluster();
            for (long j = 0; j < elements.size(); j++) {
                if (j != 0) {
                    Output << ", " << elements[j];
                    cout << ", " << elements[j];
                } else {
                    Output << " " << elements[j];
                    cout << " " << elements[j];
                }
            }
        }
    }
    Output << endl;
    cout << endl;
}

vector<long> VEBTree::getCluster() {
    vector<long> elements;
    elements.push_back(this->min);
    if (this->universe_size <= 2) {
        if (this->min != this->max) elements.push_back(this->max);
        return elements;
    } 
    long x;
    long w_ = this->w >> 1;  
    for (long c = 0; c < this->number_clusters; c++) {
        std::pair<int, Node*> cluster = this->clusters.buscarChave(c);
        if (cluster.first != -1) {
            vector<long> cluster_elements = cluster.second->pointer->getCluster();
            for (long number : cluster_elements) {
                x = (c << w_) | number;
                elements.push_back(x);
            }
        }
    }
    return elements;
}

void VEBTree::print(const string &prefix) {
    cout << prefix << "[u = " << this->universe_size << ", w = " << this->w << ", min = " << this->min << ", max = " << this->max;
    if (this->resumo) {
        cout << endl;
        cout << prefix << "  Resumo: " << endl;
        this->resumo->print(prefix + "    ");
        for (long i = 0; i < this->number_clusters; i++) {
            std::pair<int, Node*> cluster = this->clusters.buscarChave(i);
            if (cluster.first != -1) {
                cout << prefix << "  Cluster " << i << ": " << endl;
                cluster.second->pointer->print(prefix + "    ");
            }
        }
        cout << prefix << "]" << endl;
    } else cout << "]" << endl;
}

int main(int argc, char** argv) {

    if (argc <= 0) {
        cout << "Nenhum argumento passado. Por favor tente novamente passe o arquivo de txt como entrada." << endl;
        return 1;
    }

    string filename = argv[1];
    string line;

    ofstream Output("saida.txt");

    ifstream MyFile(filename);
    if (!MyFile.is_open()) {
        cout << "Não foi possivel abrir o arquivo indicado." << endl;
        return 1;
    }

    VEBTree vebtree = VEBTree(U);

    while (getline(MyFile, line)) {
        stringstream ss(line);
        string command;
        long n;
        if (ss >> command >> n) {
            if (command == "INC") {
                // INSERIR
                Output << line << endl;
                cout << line << endl;
                vebtree.Incluir(n);
            } else if (command == "REM") {
                // REMOVER
                Output << line << endl;
                cout << line << endl;
                vebtree.Remover(n);
            } else if (command == "SUC") {
                // SUCESSOR
                Output << line << endl;
                cout << line << endl;
                long s = vebtree.Sucessor(n);
                if (s != -1) Output << s << endl;
                else Output << "Não tem sucessor" << endl;
            } else if (command == "PRE") {
                // SUCESSOR
                Output << line << endl;
                cout << line << endl;
                long p = vebtree.Predecessor(n);
                if (p != -1) Output << p << endl;
                else Output << "Não tem predecessor" << endl;
            }
        } else if (command == "IMP") {
            // IMPRIMIR
            Output << line << endl;
            cout << line << endl;
            vebtree.print("");
            vebtree.Imprimir(Output);
        }
    }

    MyFile.close();
    Output.close();
    
    return 0;
}
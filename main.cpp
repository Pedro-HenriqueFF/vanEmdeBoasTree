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
        void tableChanging(long ratio);
        void tableDoubling();
        void tableHalving();

        HashTable() {
            this->table_size = initial_size;
            this->number_clusters = 0;
            this->clusters = (struct Node**)malloc(sizeof(struct Node*) * initial_size);
        }
};

long HashTable::hashFunction(long k) {
    return k % this->table_size;
}

long HashTable::inserirChave(long k, VEBTree* tree) {
    cout << "Table size: " << this->table_size << endl;
    Node* newNode = new Node(tree);
    newNode->key = k;
    long index = this->hashFunction(k);
    if(this->clusters[index] == NULL) this->clusters[index] = newNode;
    else {
        newNode->next = this->clusters[index];
        this->clusters[index] = newNode;
    }
    this->number_clusters += 1;
    if (number_clusters / table_size >= 0.75) this->tableDoubling();
    return index;
}

long HashTable::removerChave(long k) {
    long index = this->hashFunction(k);
    struct Node* prevNode = NULL;
    struct Node* currNode = this->clusters[index];
    while (currNode != NULL) {
        if (k == currNode->key) {
            if (currNode == this->clusters[index]) this->clusters[index] = currNode->next;
            else prevNode->next = currNode->next;
            free(currNode);
            this->number_clusters -= 1;
            if (number_clusters / table_size <= 0.25) this->tableHalving();
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
            //cout << index << " " << bucketHead->pointer << endl;
            return std::pair<long, Node*> {index, bucketHead};
        }
        bucketHead = bucketHead->next;
    }
    return std::pair<long, Node*> {-1, bucketHead};
}

void HashTable::tableChanging(long ratio) {
    Node** newTable = (struct Node**)malloc(sizeof(struct Node*) * long(this->table_size * ratio));
    Node** oldTable = this->clusters;
    long aux = this->table_size;
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
    this->tableChanging(long(2));
}

void HashTable::tableHalving() {
    this->tableChanging(long(0.5));
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
    cout << "Inserindo valor " << x << ", Universo " << this->universe_size << endl;
    //cout << "passo 1, valor " << x << ", Universo " << this->universe_size << endl;
    if (this->min == None) {
        //cout << "passo 2, valor " << x << ", Universo " << this->universe_size << endl;
        this->min = x;
        this->max = this->min;
    } else {
        //cout << "passo 3, valor " << x << ", Universo " << this->universe_size << endl;
        if (x < this->min) {
            //cout << "passo 4, valor " << x << ", Universo " << this->universe_size << endl;
            long aux = this->min;
            this->min = x;
            x = aux;
        }
        //cout << "passo 5, valor " << x << ", Universo " << this->universe_size << endl;
        if (x > this->max) {
            //cout << "passo 6, valor " << x << ", Universo " << this->universe_size << endl;
            this->max = x;
        }
        if (this->universe_size > 2) {
            long w_ = this->w >> 1;
            long c = x >> w_;   
            long i = x & ((1 << w_) - 1);
            //cout << "passo 7, valor " << x << ", Universo " << this->universe_size << endl;
            //cout << w_ << " " << c << " " << i << endl;
            std::pair<long, Node*> cluster = this->clusters.buscarChave(c);
            if (cluster.first == -1) {
                //cout << "passo 8, valor " << x << ", Universo " << this->universe_size << endl;
                if (!this->resumo) this->resumo = new VEBTree(sqrt(this->universe_size));
                //cout << "passo 9, valor " << x << ", Universo " << this->universe_size << endl;
                this->resumo->Incluir(c);
                //cout << "passo 10, valor " << x << ", Universo " << this->universe_size << endl;
            }
            //cout << "passo 11, valor " << x << ", Universo " << this->universe_size << endl;
            if (cluster.second == NULL) {
                //cout << "passo 12, valor " << x << ", Universo " << this->universe_size << endl;
                //cout << sqrt(this->universe_size) << endl;
                //VEBTree* new_cluster = new VEBTree(sqrt(this->universe_size));
                this->clusters.inserirChave(c, new VEBTree(sqrt(this->universe_size)));
                cluster = this->clusters.buscarChave(c);
                //cout << cluster.first << " " << cluster.second->pointer << endl;
                //cout << "passo 13, valor " << x << ", Universo " << this->universe_size << endl;
            }
            //cout << cluster.first << " " << cluster.second->pointer << endl;
            //cout << "passo 14, valor " << x << ", Universo " << this->universe_size << endl;

            cluster.second->pointer->Incluir(i);
            //cout << "passo 15, valor " << x << ", Universo " << this->universe_size << endl;
        }
    }
}

/* void VEBTree::Remover(long x) {
    //cout << "Removendo valor " << x << ", Universo " << this->universe_size << endl;
    if (this->universe_size <= 2) {
        //cout << "passo 1, valor " << x << ", Universo " << this->universe_size << endl;
        if (x == this->min) {
            if (this->min == this->max) this->min = None;
            else this->min = this->max;
        } else this->max = None;
        return;
    }
    long w_ = this->w >> 1;
    long c = x >> w_;
    long i = x & ((1 << w_) - 1);
    //cout << w_ << " " << c << " " << i << endl;
    if (x == this->min) {
        //cout << "passo 2, valor " << x << ", Universo " << this->universe_size << endl;
        if(this->resumo) {
            c = this->resumo->min;
            if (c == None) {
                //cout << "passo 3, valor " << x << ", Universo " << this->universe_size << endl;
                this->min = None;
                return;
            }
            //cout << "passo 4, valor " << x << ", Universo " << this->universe_size << endl;
            i = this->clusters[c]->min;
            x = (c << w_) | i;
            this->min = x;
        } else this->min = None;
    }
    //cout << "passo 5, valor " << x << ", Universo " << this->universe_size << endl;
    if (this->clusters[c]) {
        //cout << "passo 6, valor " << x << ", Universo " << this->universe_size << endl;
        //cout << endl;
        this->clusters[c]->Remover(i);
        //cout << endl;
        //cout << "passo 7, valor " << x << ", Universo " << this->universe_size << endl;
        if (this->clusters[c]->min == None) {
            //cout << "passo 8, valor " << x << ", Universo " << this->universe_size << endl;
            free(this->clusters[c]);
            this->clusters[c] = NULL;
            //cout << endl;
            this->resumo->Remover(c);
            //cout << endl;
            //cout << "passo 9, valor " << x << ", Universo " << this->universe_size << endl;
        }
    }
    //cout << "passo 10, valor " << x << ", Universo " << this->universe_size << endl;
    if (this->resumo) {
        if (this->resumo->min == None) {
            //cout << "passo 11, valor " << x << ", Universo " << this->universe_size << endl;
            free(this->resumo);
            this->resumo = NULL;
            this->max = this->min;
        } else {
            //cout << "passo 13, valor " << x << ", Universo " << this->universe_size << endl;
            long c_ = this->resumo->max;
            long i_ = this->clusters[c_]->max;
            long x_ = (c_ << w_) | i_;
            this->max = x_;
        }
    }
    //cout << "passo 14, valor " << x << ", Universo " << this->universe_size << endl;
}

long VEBTree::Sucessor(long x) {
    //cout << "Pegando sucessor de " << x << ", Universo " << this->universe_size << endl;
    if (this->min == None || x >= this->max) {
        //cout << "passo 1, valor " << x << ", Universo " << this->universe_size << endl; 
        return -1;
    }
    if (x < this->min) {
        //cout << "passo 2, valor " << x << ", Universo " << this->universe_size << endl;
        return this->min;
    }
    if (this->universe_size <= 2) return this->max;
    //cout << "passo 3, valor " << x << ", Universo " << this->universe_size << endl;
    long w_ = this->w >> 1;
    long c = x >> w_;
    long i = x & ((1 << w_) - 1);
    long x_, i_;
    if (this->clusters[c] && i < this->clusters[c]->max) {
        //cout << "passo 4, valor " << x << ", Universo " << this->universe_size << endl;
        i_ = this->clusters[c]->Sucessor(i);
        if (i_ == None) {
            //cout << "passo 5, valor " << x << ", Universo " << this->universe_size << endl; 
            return i_;
        }
        x_ = (c << w_) | i_;
        //cout << "passo 6, valor " << x << ", Universo " << this->universe_size << endl;
        return x_;
    }
    //cout << "passo 7, valor " << x << ", Universo " << this->universe_size << endl;
    long c_ = this->resumo->Sucessor(c);
    //cout << "passo 8, valor " << x << ", Universo " << this->universe_size << endl;
    if (c_ == None) {
        //cout << "passo 9, valor " << x << ", Universo " << this->universe_size << endl; 
        return c_;
    }
    i_ = this->clusters[c_]->min;
    x_ = (c_ << w_) | i_;
    //cout << "passo 10, valor " << x << ", Universo " << this->universe_size << endl;
    return x_;
}

long VEBTree::Predecessor(long x) {
    //cout << "Pegando predecessor de " << x << ", Universo " << this->universe_size << endl;
    if (this->min == None || x <= this->min) {
        //cout << "passo 1, valor " << x << ", Universo " << this->universe_size << endl; 
        return -1;
    }
    if (x > this->max) {
        //cout << "passo 2, valor " << x << ", Universo " << this->universe_size << endl;
        return this->max;
    }
    if (this->universe_size <= 2) return this->min;
    //cout << "passo 3, valor " << x << ", Universo " << this->universe_size << endl;
    long w_ = this->w >> 1;
    long c = x >> w_;
    long i = x & ((1 << w_) - 1);
    long x_, i_;
    if (this->clusters[c] && i > this->clusters[c]->min) {
        //cout << "passo 4, valor " << x << ", Universo " << this->universe_size << endl;
        i_ = this->clusters[c]->Predecessor(i);
        if (i_ == None) {
            //cout << "passo 5, valor " << x << ", Universo " << this->universe_size << endl; 
            return i_;
        }
        x_ = (c << w_) | i_;
        //cout << "passo 6, valor " << x << ", Universo " << this->universe_size << endl;
        return x_;
    }
    //cout << "passo 7, valor " << x << ", Universo " << this->universe_size << endl;
    long c_ = this->resumo->Predecessor(c);
    //cout << "passo 8, valor " << x << ", Universo " << this->universe_size << endl;
    if (c_ == None) {
        //cout << "passo 9, valor " << x << ", Universo " << this->universe_size << endl; 
        return this->min;
    }
    i_ = this->clusters[c_]->max;
    x_ = (c_ << w_) | i_;
    //cout << "passo 10, valor " << x << ", Universo " << this->universe_size << endl;
    return x_;
}
 */
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

    /* auto hashTable = new HashTable();
    hashTable->inserirChave(1, (VEBTree*)0x1);
    hashTable->inserirChave(2, (VEBTree*)0x2);
    cout << hashTable->buscarChave(1).second->pointer << endl;
    return 0; */

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
            } /* else if (command == "REM") {
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
            } */
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
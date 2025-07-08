//Árvore VanEmdeBoas

//Inclusão
//Remoção
//Sucessor
//Predecessor
//Imprimir
#include <iostream>
#include <optional>
#include <iomanip>
#include <cmath>
#include <set>
#include <random>
#include <ctime>
#include <fstream> 
#include <string>
using namespace std;

#define U 16
#define N_CLUSTERS 4
#define None -1

class VEBTree {
    public:
        int min;
        int max;
        int universe_size;
        int w;
        int number_clusters;
        VEBTree* resumo;
        VEBTree* clusters[N_CLUSTERS];

        void Incluir(int x);
        void Remover(int x);
        void Predecessor(int x);
        void Sucessor(int x);
        void Imprimir(const string &prefix);

        VEBTree(int u) {
            this->min = None;
            this->universe_size = u;
            this->w = log2(u);
            this->number_clusters = sqrt(u);

            this->resumo = NULL;
            for (int i = 0; i < this->number_clusters; i++) {
                this->clusters[i] = NULL;
            }
        }
};

void VEBTree::Incluir(int x) {
    cout << "Inserindo valor " << x << endl;
    //cout << "passo 1" << endl;
    if (this->min == None) {
        //cout << "passo 2" << endl;
        this->min = x;
        this->max = this->min;
    } else {
        //cout << "passo 3" << endl;
        if (x < this->min) {
            //cout << "passo 4" << endl;
            int aux = this->min;
            this->min = x;
            x = aux;
        }
        //cout << "passo 5" << endl;
        if (x > this->max) {
            //cout << "passo 6" << endl;
            this->max = x;
        }
        if (this->universe_size > 2) {
            int w_ = this->w >> 1;
            int c = x >> w_;
            int i = x & ((1 << w_) - 1);
            //cout << "passo 7" << endl;
            //cout << w_ << " " << c << " " << i << endl;
            //cout << this->clusters[c] << endl;
            if (!this->clusters[c]) {
                //cout << "passo 8" << endl;
                if (!this->resumo) this->resumo = new VEBTree(sqrt(this->universe_size));
                //cout << "passo 9" << endl;
                this->resumo->Incluir(c);
                //cout << "passo 10" << endl;
            }
            //cout << "passo 11" << endl;
            if (!this->clusters[c]) this->clusters[c] = new VEBTree(sqrt(this->universe_size));
            //cout << "passo 12" << endl;
            this->clusters[c]->Incluir(i);
            //cout << "passo 13" << endl;
        }
    }
}

void VEBTree::Remover(int x) {
    cout << "Removendo valor " << x << ", Universo " << this->universe_size << endl;
    if (this->universe_size <= 2) {
        cout << "passo 1, valor " << x << ", Universo " << this->universe_size << endl;
        if (x == this->min) {
            if (this->min == this->max) this->min = None;
            else this->min = this->max;
        } else this->max = None;
        return;
    }
    int w_ = this->w >> 1;
    int c = x >> w_;
    int i = x & ((1 << w_) - 1);
    cout << w_ << " " << c << " " << i << endl;
    if (x == this->min) {
        cout << "passo 2, valor " << x << ", Universo " << this->universe_size << endl;
        if(this->resumo) {
            c = this->resumo->min;
            if (c == None) {
                cout << "passo 3, valor " << x << ", Universo " << this->universe_size << endl;
                this->min = None;
                return;
            }
            cout << "passo 4, valor " << x << ", Universo " << this->universe_size << endl;
            i = this->clusters[c]->min;
            x = (c << w_) | i;
            this->min = x;
        } else this->min = None;
    }
    cout << "passo 5, valor " << x << ", Universo " << this->universe_size << endl;
    if (this->clusters[c]) {
        cout << "passo 6, valor " << x << ", Universo " << this->universe_size << endl;
        cout << endl;
        this->clusters[c]->Remover(i);
        cout << endl;
        cout << "passo 7, valor " << x << ", Universo " << this->universe_size << endl;
        if (this->clusters[c]->min == None) {
            cout << "passo 8, valor " << x << ", Universo " << this->universe_size << endl;
            free(this->clusters[c]);
            this->clusters[c] = NULL;
            cout << endl;
            this->resumo->Remover(c);
            cout << endl;
            cout << "passo 9, valor " << x << ", Universo " << this->universe_size << endl;
        }
    }
    cout << "passo 10, valor " << x << ", Universo " << this->universe_size << endl;
    if (this->resumo) {
        if (this->resumo->min == None) {
            cout << "passo 11, valor " << x << ", Universo " << this->universe_size << endl;
            free(this->resumo);
            this->resumo = NULL;
            this->max = this->min;
        } else {
            cout << "passo 13, valor " << x << ", Universo " << this->universe_size << endl;
            int c_ = this->resumo->max;
            int i_ = this->clusters[c_]->max;
            int x_ = (c_ << w_) | i_;
            this->max = x_;
        }
    }
    cout << "passo 14, valor " << x << ", Universo " << this->universe_size << endl;
}

void VEBTree::Imprimir(const string &prefix) {
    cout << prefix << "[u = " << this->universe_size << ", w = " << this->w << ", min = " << this->min;
    if (this->resumo) {
        cout << endl;
        cout << prefix << "  Resumo: " << endl;
        this->resumo->Imprimir(prefix + "    ");
        for (int i = 0; i < this->number_clusters; i++) {
            if (this->clusters[i]) {
                cout << prefix << "  Cluster " << i << ": " << endl;
                this->clusters[i]->Imprimir(prefix + "    ");
            }
        }
        cout << prefix << "]" << endl;
    } else cout << "]" << endl;
}

int main() {
    VEBTree vebtree = VEBTree(U);

    cout << "Tudo certo!" << endl;

    vebtree.Incluir(0);
    vebtree.Incluir(1);
    vebtree.Incluir(2);
    vebtree.Incluir(7);
    vebtree.Incluir(8);
    vebtree.Incluir(9);

    cout << "Árvore VanEmdeBoas: " << endl;
    vebtree.Imprimir("");

    //vebtree.Remover(0);
    //vebtree.Remover(1);
    //vebtree.Remover(2);
    //vebtree.Remover(7);
    //vebtree.Remover(8);
    vebtree.Remover(9);

    cout << "Árvore VanEmdeBoas: " << endl;
    vebtree.Imprimir("");
    return 0;
}
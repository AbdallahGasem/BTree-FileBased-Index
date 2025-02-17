#ifndef UNTITLED_BTREE_H
#define UNTITLED_BTREE_H

#include <iostream>
#include <fstream>

using namespace std;

class BTree {
private:
    int records, nodes, size;
    fstream file;

public:
    BTree (int records, int nodes);

    int insert (int id, int ref);

    void remove (int id);

    int search (int id);

    void printBTree();
};

#endif

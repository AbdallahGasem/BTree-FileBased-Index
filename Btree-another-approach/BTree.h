#ifndef UNTITLED_BTREE_H
#define UNTITLED_BTREE_H

#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

class BTree {
public:
    
    int numOfRecords, m;
    fstream file;
    const char* ifname;
    vector<vector<pair<int, int>>>index;    // index file represented as a 2d vector

public:

    // initialize index vector
    void initialize(int numOfRecords, int m);   

    // write the index form the index 2d vector into the physical file (Flush)
    void writeIndexVec();

    // read the index form the physical file into the index 2d vector (Cache)
    void readIndexVec();

    // sort an index node by the node.first
    void sortNode(vector<pair<int,int>> &v);

    // update the largest cell id in each node recursively
    void updateParents(int nodeIdx, int newID, int oldID);

    // checks if the parent is full or not
    // vector<int> getParents(int nodeIdx);

    // gpt
    int getParentIndex(int childNodeIndex);

public:
    // create a new index file
    BTree (const char* idxFileName, int numOfRecords, int m);   
    
    // open an existing index file
    BTree (const char* idxFileName);    

    // destructor
    ~BTree();

    // points to the next empty node!
    int getHead();  
    void updateHead(int val);
    void refreshHead();

    // 0: leaf , 1: nonleaf
    bool getNodeType(int i);    
    void updateNodetype(int nodeIdx, bool type);

    // search for the suitable node for insert
    int searchSuitable(int id);

    /* insert helpers */
    void split(int currentNodeIndex, int id, int ref);

    int getFirstEmptyCell(vector<pair<int,int>> &indexNode );

    void insert (int id, int ref);

    void remove (int id);

    int search (int id);

    void printBTree();


};

#endif

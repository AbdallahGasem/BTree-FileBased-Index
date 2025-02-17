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
    int load_factor;
    fstream file;
    const char* ifname;
    vector<vector<pair<int, int>>>index;    // index file represented as a 2d vector
    int numOfRecords, m;

    // initialize index vector
    void initialize(int numOfRecords, int m);   



    // write the index form the index 2d vector into the physical file (Flush)
    void writeIndexVec();

    // read the index form the physical file into the index 2d vector (Cache)
    void readIndexVec();

    // sort an index node by the node.first

    void sortNode(vector<pair<int,int>> &v);
    // create a new index file
    BTree (const char* idxFileName, int numOfRecords, int m);   
    
    // open an existing index file
    BTree (const char* idxFileName);    

    // points to the next empty node!
    int getHead();  
    void updateHead();

    // 0: leaf , 1: nonleaf
    bool getNodeType(int i);    
    void updateNodetype(int nodeIdx, bool type);

    /* insert helpers */
    void split(int currentNodeIndex, int id, int ref);

    int getFirstEmptyCell(vector<pair<int,int>> &indexNode );

    void insert (int id, int ref);

    void remove (int id);

    void Simple_Remove(int id);
    
    int check_head(int id);

    vector<int> leaf_node_index(int id);

    void update_row_head(int id);

    void hard_insertion();

    int search (int id);



    void printBTree();

    int used_cells(int row_number);

    void removesort(vector<pair<int, int>> &v);
    
    void removesort(int id);

    ~BTree();


    // search for the suitable node for insert
    int searchSuitable(int id);

    // update the largest cell id in each node recursively
    void updateParents(int nodeIdx, int newID, int oldID);
    ////////////////////////////////////////////////////////////////////Deletion//////////////////////////////////////////////////////
  
    vector<int> get_parent(int row_number);
    
    vector<int> get_sibling(int row_number,int id);

    vector<int> check_sibling(int row_number,int id);

    void borrow_left(int row_number,int left_sibling,int id);

    void borrow_right(int row_number,int right_sibling,int id);
    
    void update_right(int id);


  
};

#endif

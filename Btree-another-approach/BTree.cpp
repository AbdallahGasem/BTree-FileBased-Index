//                                                      بسم الله الرحمن الرحيم                                                 //
// program: BTree.cpp
// Description:
// Authors:  Abdallah Gasem , Mohamed Wagdy, Ahmed Karam, Rawan el-Naggary & Abdelrahman Lotfy
// Date: 25-01-2025
// Version: 1.0
/* file run command: g++ -o BTree BTree.cpp; if ($?) { ./BTree } */
// ----------------------------------------------------------------------------------------------------------------------------- //

#include "BTree.h"

using namespace std;

// initialize index vector
void BTree::initialize(int numOfRecords, int m)
{

    // first pair at index 0 indicates node type 0: leaf and 1: node
    index.resize(numOfRecords, vector<pair<int, int>>(m + 1, {-1, -1}));
    for (int i = 0; i < index.size(); i++)
    {
        if (i == 9)
        {
            index[i][1].second = -1;
            break;
        }

        index[i][1].second = i + 1;
    }
}

int BTree::getHead()
{
    return index[0][1].second;
}

void BTree::updateHead(int val )
{
    index[0][1].second = val;
}

void BTree:: refreshHead()
{
    bool Eflag = false;
    int emptyNodeIdx = -1;

    for (int i = 1; i < index.size(); i++)
    {
        for (int j = 1; j < index[i].size(); j++)
        {
            if (index[i][j].first == -1)
            {
                Eflag = true;
            }
            else
            {
                // the node is not empty and has a value!
                Eflag = false;
                break;
            } 
        }

        if (Eflag)
        {
            emptyNodeIdx = i;
            break;
        }
        else
        {
            continue;
        }
    }

    if (emptyNodeIdx != -1)
    {
        updateHead(emptyNodeIdx);
        
        // debuggingg //
        cout << "refreshHead(): the new head after refresh is " << emptyNodeIdx << endl;
        // debuggingg //

        return;
    }

    cout << "\n refreshHead(): hmmmm, something is wrong you should not reach this message! \n";    
}

// create a new index file
BTree::BTree(const char *idxFileName, int numOfRecords, int m)
{

    this->numOfRecords = numOfRecords;

    this->m = m;

    ifname = idxFileName;

    // file.open(idxFileName, ios::out | ios::binary);

    initialize(numOfRecords, m);
    writeIndexVec(); // after intialization write the vector to the physical file
}

BTree::BTree(const char *idxFileName)
{

    ifname = idxFileName;
    readIndexVec();

    m = index[0].size();    // note that we deal with m+1 cells at each node!
    numOfRecords = index.size();
}

BTree::~BTree()
{
    writeIndexVec();
}

bool BTree::getNodeType(int i) // 0: leaf , 1: nonleaf
{
    return index[i][0].first == 1;
}

void BTree::updateNodetype(int nodeIdx, bool type)
{
    index[nodeIdx][0].first = type;
    index[nodeIdx][0].second = type;
}

//---------------------------------------------------------[READ & WRITE]----------------------------------------------------------

void BTree::readIndexVec()
{
    file.open(ifname, ios::in | ios::binary);

    if (!file.is_open())
    {
        cout << "readIndexVec(): ERROR! Cannot open the file." << endl;
        return;
    }

    // Read the number of rows
    int rows;
    file.read(reinterpret_cast<char *>(&rows), sizeof(rows));
    index.resize(rows);

    for (auto &row : index)
    {
        // Read the number of columns for this row
        int cols;
        file.read(reinterpret_cast<char *>(&cols), sizeof(cols));
        row.resize(cols);

        // Read each pair<int, int> in the row
        for (auto &p : row)
        {
            file.read(reinterpret_cast<char *>(&p.first), sizeof(p.first));
            file.read(reinterpret_cast<char *>(&p.second), sizeof(p.second));
        }
    }

    file.close();
}

void BTree::writeIndexVec()
{
    file.open(ifname, ios::out | ios::binary);

    if (!file.is_open())
    {
        cout << "writeIndexVec(): ERROR! Cannot open the file." << endl;
        return;
    }

    // Write the number of rows and columns first
    int rows = index.size();
    file.write(reinterpret_cast<char *>(&rows), sizeof(rows));

    for (const auto &row : index)
    {
        int cols = row.size();
        file.write(reinterpret_cast<char *>(&cols), sizeof(cols));

        // Write each pair<int, int> in the row
        for (const auto &p : row)
        {
            file.write(reinterpret_cast<const char *>(&p.first), sizeof(p.first));
            file.write(reinterpret_cast<const char *>(&p.second), sizeof(p.second));
        }
    }

    file.close();
}

//----------------------------------------------------------[INSERT]--------------------------------------------------------------
/* Helpers */

// return the index of the first empty cell in a node , returns -1 if the node was full
int BTree::getFirstEmptyCell(vector<pair<int, int>> &indexNode)
{

    for (int i = 1; i < indexNode.size(); i++)
    {
        if (indexNode[i].first == -1)
        {
            return i;
        }
    }

    return -1;
}

// sort an index node by the node.first
void BTree::sortNode(vector<pair<int, int>> &v)
{
    sort(v.begin() + 1, v.end(), [](const pair<int, int> &a, const pair<int, int> &b)   // skip the type pair at [0]
         { return a.first < b.first; });
}

int BTree::searchSuitable(int id)
{
    int next;   // will always point to an internal node aka record number!

    for (int i = 1; i < index.size(); i++) //  0 1 2 3 4 5 6 7 8 9  if numOfrecords = 10 , skip the Head node
    {
        for (int j = 1; j < index[i].size(); j++)   // 0 1 2 3 4 5 if m = 5    [forgot to skip the type pair! test now 7:01 am]
        {
            if (getNodeType(i) == 1 && id < index[i][j].first)
            {
                // why second?! bec of the refs but i case it doesnt refer to anything its just a value?
                // wont enter notice the condition: getNodeType(i) == 1 && id < index[i][j].first
                next = index[i][j].second;  
                break;
            }

            // handles the edge case where the inserted value is bigger than the biggest value in the ROOT
            else if (getNodeType(i) == 1 && j == index[i].size() - 1 && id > index[i][j].first)
            {
                next = index[i][j].second;
                break;
            }
        }

        if (getNodeType(next) == 0)
        {
            return next;
        }

        else
        {
            continue;
        }
    }

    return -1; // should never return this
}

void BTree::updateParents(int nodeIdx, int newID, int oldID)
{
    // base case
    if (nodeIdx == 1)
    {
        // update the last parent (ROOT)
        index[nodeIdx][index[nodeIdx].size() - 1].first = newID;
        return;
    }

    // update
    if (index[nodeIdx][index[nodeIdx].size() - 1].first == oldID)     // skips the leaf nodes
    {
        index[nodeIdx][index[nodeIdx].size() - 1].first = newID;
    }
    
    updateParents(nodeIdx - 1, newID, oldID);
}

vector<int> BTree:: getParents(int nodeIdx)
{
    int next;   // will always point to an internal node aka record number!
    vector<int>v;

    for (int i = 1; i < index.size(); i++) 
    {
        for (int j = 1; j < index[i].size(); j++)   
        {
            if (getNodeType(i) == 1 && nodeIdx == index[i][j].second)
            {
                // why second?! bec of the refs but i case it doesnt refer to anything its just a value?
                // wont enter notice the condition: getNodeType(i) == 1 && id < index[i][j].first
                next = i;  
                break;
            }

        }

        if (getNodeType(next) == 1)
        {
            v.push_back(next);
        }

    }

    return {}; // should never return this

}

// splits an overflowed node
void BTree::split(int currentNodeIndex, int id, int ref)
{
    vector<pair<int, int>> overFlowedNode;

    // loading the overFlowedNode vector by the index node pairs - start from one to skip the type pair
    for (int i = 1; i < index[currentNodeIndex].size(); i++)
    {
        overFlowedNode.push_back(index[currentNodeIndex][i]);
    }
    overFlowedNode.push_back({id, ref}); // push the node that caused the overflow!

    // sort to get the median and the largest id
    sortNode(overFlowedNode);

    // tracking the median and the largest id values that will be at the parent node after spliting
    pair<int, int> median = overFlowedNode[floor(overFlowedNode.size() / 2) - 1];
    pair<int, int> largest = overFlowedNode[overFlowedNode.size() - 1];
    
    // debuging //
    // cout << "split(): the median id is: " << median.first << " and the largest id is: " << largest.first << endl;
    // cout << "should be (3)" << overFlowedNode[0].first << endl;
    // debuging //

    int lnodeIdx = getHead();

    // loading left node in the index with its values - start from one to skip the type pair
    for (int i = 0 ; i < floor(overFlowedNode.size() / 2) ; i++)
    {
        // was index[lnodeIdx][i].first = overFlowedNode[i].first; which starts from 0 so it was putting data
        // in the type pair at index[i][0] which caused the (3,12) to be missing!
        index[lnodeIdx][i+1].first = overFlowedNode[i].first;   
        index[lnodeIdx][i+1].second = overFlowedNode[i].second;

        // update node type to leaf
        updateNodetype(lnodeIdx, 0);
    }
    sortNode(index[lnodeIdx]);
    // updateHead(); -- replaced with refreshHead()!
    refreshHead();

    int rnodeIdx = getHead();

    // loading right node in the index with its values
    for (int i = floor(overFlowedNode.size() / 2); i <= overFlowedNode.size(); i++)
    {
        index[rnodeIdx][i].first = overFlowedNode[i].first;
        index[rnodeIdx][i].second = overFlowedNode[i].second;

        // update node type to leaf
        updateNodetype(rnodeIdx, 0);
    }
    sortNode(index[rnodeIdx]);
    // updateHead(); -- replaced with refreshHead()!
    refreshHead();

    // Resetting the parent node   -- fits only in the case of first creation of the parent as afterwards when inserting the overflowed will be appended to the parent!
    for (int i = 1; i < index[currentNodeIndex].size(); i++)
    {
        index[currentNodeIndex][i] = {-1, -1};
    }

    // update the parent node
    int firstEmptyCell = getFirstEmptyCell(index[currentNodeIndex]);
    index[currentNodeIndex][firstEmptyCell].first = median.first;
    index[currentNodeIndex][firstEmptyCell].second = lnodeIdx;

    firstEmptyCell++;

    index[currentNodeIndex][firstEmptyCell].first = largest.first;
    index[currentNodeIndex][firstEmptyCell].second = rnodeIdx;

    sortNode(index[currentNodeIndex]);

    // update node type to internal
    updateNodetype(currentNodeIndex, 1);
}

void BTree::insert(int id, int ref)
{ /* Root Node will always be at index[1] */
    int nodeIdx;
    int firstEmptyCell;
    int oldID;    

    if (getHead() == 1)     // create the root node
    {
        firstEmptyCell = getFirstEmptyCell(index[1]);
        if (firstEmptyCell == -1)    // need to split
        {
            // trigger the head update when a node is going to split! - i guess we need to use the refreshHead() instead but placed after and inside the split! [7:24 am]
            // updateHead();    // trigger the head update when a node is going to split! - replaced with refreshHead()!

            refreshHead();
            split(1, id, ref);
        }
        else
        {
            index[1][firstEmptyCell].first = id;
            index[1][firstEmptyCell].second = ref;
        }
        sortNode(index[1]);
    }
    else
    {

        nodeIdx = searchSuitable(id);
        firstEmptyCell = getFirstEmptyCell(index[nodeIdx]);
        oldID = index[nodeIdx][index[nodeIdx].size() - 1].first;

        if (firstEmptyCell == -1)
        {
            // trigger the head update when a node is going to split! - i guess we need to use the refreshHead() instead but placed after and inside the split! [7:24 am]
            // updateHead();    // trigger the head update when a node is going to split! - replaced with refreshHead()!
            refreshHead();
            split(nodeIdx, id, ref);
        }

        else
        {
            if (id > oldID)
            {
                index[nodeIdx][firstEmptyCell].first = id;
                index[nodeIdx][firstEmptyCell].second = ref;
                updateParents(nodeIdx-1, id, oldID);    // nodeIdx-1 to avoid updating itself
            }

            else
            {
                index[nodeIdx][firstEmptyCell].first = id;
                index[nodeIdx][firstEmptyCell].second = ref;
            }
        }
        sortNode(index[nodeIdx]);
    }

}

//----------------------------------------------------------[DELETE]---------------------------------------------------------------
void BTree::remove(int id)
{
    // remove implementation
}

//----------------------------------------------------------[SEARCH]---------------------------------------------------------------
int BTree::search(int id)
{
    // Traverse the index vector (each node of the B-Tree)
    int reference = -1;

    for (int i = 0; i < numOfRecords; i++)
    {

        // Traverse the key-value pairs in the current node
        for (int j = 0; j < index[i].size(); j++)
        {
            if (index[i][0].first == 1 && index[i][0].second == 1)
                break;
            if (index[i][j].first == id)
            {
                reference = index[i][j].second;
            }
        }
    }

    return reference;
}

void BTree::printBTree()
{

    // printBTree implementation

    for (int i = 0; i < index.size(); i++)
    {
        for (int j = 0; j < index[i].size(); j++)
        {
            cout << "(" << index[i][j].first << " " << index[i][j].second << ")" << " , ";
        }
        cout << endl;
    }
}

int main()
{
    cout << "\n________________________________________________________[START]__________________________________________________________\n";
    BTree btree("INDEX.txt",10,5);
    // BTree btree("INDEX.txt");
    cout << "\n------------------------------------------------------------------------\n";
    btree.printBTree();
    cout << "\n------------------------------------------------------------------------\n";
    cout << endl;

    btree.insert(3, 12);    // -- working
    btree.insert(10, 48);    //-- working
    btree.insert(7, 24);     //-- working
    btree.insert(24, 60);    //-- working
    btree.insert(14, 72);    //-- working
    btree.insert(19, 84);    // requires splitting           -- working 
    btree.insert(30, 96);    // edge case needs recursion    -- working
    // btree.insert(15, 108);       -- working
    // btree.insert(1, 120);        -- working
    // btree.insert(5, 132);        -- working

    
    // btree.insert(2, 144);    // requires splitting and merging to the ROOT! <New case>



    // btree.insert(8, 156);
    // btree.insert(9, 168);
    // btree.insert(6, 180);
    // btree.insert(11, 192 );
    // btree.insert(12, 204);
    // btree.insert(17, 216);
    // btree.insert(18, 228);
    // btree.insert(32, 240);

    cout << endl;
    cout << "\n------------------------------------------------------------------------\n";
    btree.printBTree();
    cout << "\n------------------------------------------------------------------------\n";

    // cout << "Suitable node number is: " << btree.searchSuitable(15);    // expected is 3
    cout << "\n_________________________________________________________[END]__________________________________________________________\n";
}

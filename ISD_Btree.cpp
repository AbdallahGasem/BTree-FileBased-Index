//                                                      بسم الله الرحمن الرحيم                                                 //
// program: BTree.cpp 
// Description: 
// Authors:  Abdallah Gasem , Mohamed Wagdy, Ahmed Karam, Rawan el-Naggary & Abdelrahman Lotfy
// Date: 25-01-2025
// Version: 1.0
/* file run command: g++ -o BTree BTree.cpp; if ($?) { ./BTree } */
// ----------------------------------------------------------------------------------------------------------------------------- //

#include "BTree.h"
#include <bits/stdc++.h>

using namespace std;


// initialize index vector
void BTree::initialize(int numOfRecords, int m) {
    
    // first pair at index 0 indicates node type 0: leaf and 1: node  
    index.resize(numOfRecords, vector<pair<int, int>>(m+1,{-1,-1}));
    for (int i = 0; i < index.size(); i++)
    {
        if (i == 9)
        {
            index[i][1].second = -1;
            break;
        }
        
        index[i][1].second = i+1;
    }

}

int BTree::getHead()
{
    return index[0][1].second;
}

void BTree::updateHead()
{
    index[0][1].second++;
}


// create a new index file
BTree::BTree(const char* idxFileName, int numOfRecords, int m) {

    this->numOfRecords = numOfRecords;

    this->m = m;

    ifname = idxFileName;

    // file.open(idxFileName, ios::out | ios::binary);
    load_factor=m/2;
    //initialize(numOfRecords, m);
    hard_insertion();
    writeIndexVec();    // after intialization write the vector to the physical file

}

BTree::BTree(const char *idxFileName)
{

    ifname = idxFileName;
    readIndexVec();

    m = index[0].size();
    numOfRecords = index.size();
}

bool BTree::getNodeType(int i)  // 0: leaf , 1: nonleaf
{
    return index[i][0].first == 1; 

}

void BTree::updateNodetype(int nodeIdx, bool type)
{
    index[nodeIdx][0].first = type;
    index[nodeIdx][0].second = type;

}

//---------------------------------------------------------[READ & WRITE]----------------------------------------------------------

void BTree::readIndexVec() {
    file.open(ifname, ios::in | ios::binary);

    if (!file.is_open()) {
        cout << "readIndexVec(): ERROR! Cannot open the file." << endl;
        return;
    }

    // Read the number of rows
    int rows;
    file.read(reinterpret_cast<char*>(&rows), sizeof(rows));
    index.resize(rows);

    for (auto& row : index) {
        // Read the number of columns for this row
        int cols;
        file.read(reinterpret_cast<char*>(&cols), sizeof(cols));
        row.resize(cols);

        // Read each pair<int, int> in the row
        for (auto& p : row) {
            file.read(reinterpret_cast<char*>(&p.first), sizeof(p.first));
            file.read(reinterpret_cast<char*>(&p.second), sizeof(p.second));
        }
    }

    file.close();
}


void BTree::writeIndexVec() {
    file.open(ifname, ios::out | ios::binary);

    if (!file.is_open()) {
        cout << "writeIndexVec(): ERROR! Cannot open the file." << endl;
        return;
    }

    // Write the number of rows and columns first
    int rows = index.size();
    file.write(reinterpret_cast<char*>(&rows), sizeof(rows));

    for (const auto& row : index) {
        int cols = row.size();
        file.write(reinterpret_cast<char*>(&cols), sizeof(cols));

        // Write each pair<int, int> in the row
        for (const auto& p : row) {
            file.write(reinterpret_cast<const char*>(&p.first), sizeof(p.first));
            file.write(reinterpret_cast<const char*>(&p.second), sizeof(p.second));
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
void BTree:: sortNode(vector<pair<int, int>> &v)
{
    sort(v.begin()+1, v.end(), [](const pair<int, int> &a, const pair<int, int> &b) {
        return a.first < b.first;
    });
}

void BTree::removesort(vector<pair<int, int>> &v)
{
    
    sort(v.begin()+1, v.end(), [](const pair<int, int> &a, const pair<int, int> &b) {
        return a.first > b.first;
    });

}
void BTree::removesort(int id)
{
    removesort(index[id]);
    sort(index[id].begin()+1, index[id].begin()+1+used_cells(id), [](const pair<int, int> &a, const pair<int, int> &b)
    {
        return a.first < b.first;
    });
}

BTree::~BTree()
{
    writeIndexVec();
}

int BTree::used_cells(int row_number)
{

int counter=0;
for (int i = 1; i < this->index[row_number].size(); i++)
{
    if( this->index[row_number][i].first != -1 ) counter++;
}

return counter;

}


// splits an overflowed node    -- till now it can handles the first node split in the whole tree
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

    int lnodeIdx = getHead();

    // loading left node in the index with its values - start from one to skip the type pair
    for (int i = 1; i <= floor(overFlowedNode.size() / 2) - 1; i++)
    {
        index[lnodeIdx][i].first = overFlowedNode[i].first;
        index[lnodeIdx][i].second = overFlowedNode[i].second;

        // update node type to leaf
        updateNodetype(lnodeIdx, 0);
    }
    sortNode(index[lnodeIdx]);
    updateHead();

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
    updateHead();

    // Resetting the parent node   -- wrong?! fits only in the case of first creation of the parent as afterwards when inserting the overflowed will be appended to the parent!
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
//----------------------------------------------------------[SEARCH]---------------------------------------------------------------
int BTree::search(int id) {
    // Traverse the index vector (each node of the B-Tree)
    int reference=-1;

    for (int i = 0 ; i < numOfRecords;i++) 
    {
        
        // Traverse the key-value pairs in the current node
        for (int j = 0 ; j < index[i].size() ; j++) 
        {
            if(index[i][0].first==1 && index[i][0].second==1) break;
            if (index[i][j].first == id) 
            {
                reference = index[i][j].second;
            }
        }
    }

    return reference ;
}
//----------------------------------------------------------[INSERT]---------------------------------------------------------------
void BTree::insert(int id, int ref)
{ /* Root Node will always be at index[1] */
    int nodeIdx;
    int firstEmptyCell;
    int oldID;

    if (getHead() == 1) // create the root node
    {
        firstEmptyCell = getFirstEmptyCell(index[1]);
        if (firstEmptyCell == -1) // need to split
        {
            updateHead(); // trigger the head update when a node is going to split!
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
            updateHead();    // trigger the head update when a node is going to split!
            split(nodeIdx, id, ref);
        }

        else
        {
            if (id > oldID)
            {
                index[nodeIdx][firstEmptyCell].first = id;
                index[nodeIdx][firstEmptyCell].second = ref;
                updateParents(nodeIdx-1, id, oldID);
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

int BTree::searchSuitable(int id)
{
    int next;

    for (int i = 1; i < index.size(); i++)
    {
        for (int j = 0; j < index[i].size(); j++)
        {
            if (getNodeType(i) == 1 && id < index[i][j].first)
            {
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
        index[nodeIdx][m - 1].first = newID;
        return;
    }

    // update
    if (index[nodeIdx][m - 1].first == oldID)     // skips the leaf nodes
    {
        index[nodeIdx][m - 1].first = newID;
    }
    
    updateParents(nodeIdx - 1, newID, oldID);
}

//----------------------------------------------------------[DELETE]---------------------------------------------------------------
// void BTree::remove(int id) {
//     for (int i = 1; i < index.size(); i++) 
//     {
//         for (int j = 1; j < index[i].size(); j++) 
//         {
//             if (id == index[i][j].first) 
//             {

vector<int> BTree::leaf_node_index(int id)
{
    vector<int> node_index(2, -1);
    for(int i=numOfRecords-1;i>0;i--)
    {
        if(index[i][0].first==0)
        {
            for (int j = 1; j < m; j++)
            {
                if(index[i][j].first==id)
                {
                    cout<<"\n entered second if condition";
                    node_index[0]=i;
                    node_index[1]=j;
                    return node_index;
                }
            }
            
        }
    }
    return node_index;
}

int BTree::check_head(int id)
{
    for(int i=numOfRecords-1;i>0;i--)
    {
        if(index[i][0].first==0)
        {
            int row_size=used_cells(i);
            // cout<<"\nrow size = "<<row_size;
            if(index[i][row_size].first==id&&row_size>0)
            {
                return 1;
            }
        }
    }
    return 0;
}

// void BTree::update_row_head(int id)
// {
//     vector<int> node_location=leaf_node_index(id);
//     int new_id = index[node_location[0]][node_location[1]-1].first;
//     int old_id = index[node_location[0]][node_location[1]].first;
//     for(int i=1;i<numOfRecords;i++)
//     {
//         for(int j=1;j<m;j++)
//         {
//             if (index[i][j].first==old_id&&index[i][0].first==1)
//             {
//                 index[i][j].first=new_id;
//             }
//         }
//     }
// }

void BTree::Simple_Remove(int id)
{

    vector<int> node_location=leaf_node_index(id);
    if (node_location[0]!=-1&&node_location[1]!=-1)
    {
        index[node_location[0]][node_location[1]].first=-1;
        index[node_location[0]][node_location[1]].second=-1;
        removesort(node_location[0]);

    }else
    {
        cout<<"node id not found";
    }
}

void BTree::remove(int id) 
{
    int check=check_head(id);
    int node_fullness=used_cells(leaf_node_index(id)[0]);
    if (node_fullness>load_factor)
    {
        if(check==0)
        {
            Simple_Remove(id);
        }else if (check==1)
        {
            update_row_head(id);
            Simple_Remove(id);
        }
    }else
    {
        vector<int> sibling_check=check_sibling(leaf_node_index(id)[0],id);
        if (sibling_check[0]==1)
        {
            if(check==1)
            {
                update_row_head(id);
            }
            borrow_left(leaf_node_index(id)[0],sibling_check[1],id);
        }else if(sibling_check[0]==2)
        {
            // if(check==1)
            // {
            //     cout<<"\nabo karam\n";
            //     update_row_head(id);
            // }
            borrow_right(leaf_node_index(id)[0],sibling_check[1],id);
        }
        else
        {
            // merge();
        }   
    }
}

vector<int> BTree::get_parent(int row_number)
{
    // this vector should contain parent's row number in index 0
    // and the greatest id in the node we want to delete an element from it
    vector<int> v(2,-1);
    for (int i = 1; i < numOfRecords; i++)
    {
        if (index[i][0].first==1)
        {
            for (int j = 0; j < m; j++)
            {
                if (index[i][j].second==row_number)
                {
                    v[0]=i;
                    v[1]=index[i][j].first;
                    return v;
                }
                
            }
        }
    }    
    return v;
}

vector<int> BTree::get_sibling(int row_number,int id)
{
    vector<int> sibling_index(2, -1);
    for (int j = 0; j < m; j++)
    {
        if(index[row_number][j].first==id&&j!=1&&j!=m-1)
        {
            int left_sibling=index[row_number][j-1].second;
            int right_sibling=index[row_number][j+1].second;
            sibling_index[0]=left_sibling;
            sibling_index[1]=right_sibling;
        }else if(index[row_number][j].first==id&&j==1)
        {
            int right_sibling=index[row_number][j+1].second;
            sibling_index[1]=right_sibling;
        }else if(index[row_number][j].first==id&&j==m)
        {
            int left_sibling=index[row_number][j-1].second;
            sibling_index[0]=left_sibling;
        }
    }
    return sibling_index;
}

vector<int> BTree::check_sibling(int row_number,int id)
{
    vector<int> parent=get_parent(row_number);
    vector<int> sibling = get_sibling(parent[0],parent[1]);
    vector<int> v(2,-1);
    if(sibling[0]!=-1)
    {
        if(used_cells(sibling[0])>load_factor)
        {
            v[0]=1;
            v[1]=sibling[0];
            return v;
        }
    }
    if(sibling[1]!=-1)
    {
        if(used_cells(sibling[1])>load_factor)
        {
            v[0]=2;
            v[1]=sibling[1];
            return v;
        }
    }
    v[0]=0;
    return v;
}

void BTree:: borrow_left(int row_number,int left_sibling,int id)
{
    pair<int,int> borrowed_node=index[left_sibling][used_cells(left_sibling)];
    update_row_head(borrowed_node.first);
    Simple_Remove(borrowed_node.first);
    vector<int> removed_cell=leaf_node_index(id);
    index[removed_cell[0]][removed_cell[1]].first=borrowed_node.first;
    index[removed_cell[0]][removed_cell[1]].second=borrowed_node.second;
    removesort(row_number);
}

void BTree::borrow_right(int row_number,int right_sibling,int id)
{
    pair<int,int> borrowed_node=index[right_sibling][1];
    Simple_Remove(borrowed_node.first);
    vector<int> removed_cell=leaf_node_index(id);
    index[removed_cell[0]][removed_cell[1]].first=borrowed_node.first;
    index[removed_cell[0]][removed_cell[1]].second=borrowed_node.second;
    removesort(row_number);
    update_right(index[removed_cell[0]][removed_cell[1]].first);
}

void BTree::update_right(int id)
{
    vector<int> node_location=leaf_node_index(id);
    int new_id = index[node_location[0]][node_location[1]+1].first;
    int old_id = index[node_location[0]][node_location[1]].first;
    for(int i=1;i<numOfRecords;i++)
    {
        for(int j=1;j<m;j++)
        {
            if (index[i][j].first==old_id&&index[i][0].first==1)
            {
                index[i][j].first=new_id;
            }
        }
    }
}

void BTree::update_row_head(int id)
{
    vector<int> node_location=leaf_node_index(id);
    int new_id = index[node_location[0]][node_location[1]-1].first;
    int old_id = index[node_location[0]][node_location[1]].first;
    for(int i=1;i<numOfRecords;i++)
    {
        for(int j=1;j<m;j++)
        {
            if (index[i][j].first==old_id&&index[i][0].first==1)
            {
                index[i][j].first=new_id;
            }
        }
    }
}

void BTree::hard_insertion()
{
     index = {
    // Root node (index 0)
    {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1} },

    // Node 1 (index 1)
    { {1,1},{10, 8}, {32,9}, {-1, -1}, {-1, -1}, {-1, -1} },

    // Node 2 (index 2)
    {{0,0}, {1, 120}, {2, 144}, {3, 12}, {-1, -1}, {-1, -1} },

    // Node 3 (index 3)
    {{0,0}, {11, 192}, {12, 204},{14, 72}, {15, 108}, {-1, -1} },

    // Node 4 (index 4)
    {{0,0}, {5,132}, { 6,180}, {7,24}, {-1, -1}, {-1, -1} },

    // Node 5 (index 5)
    { {0,0},{8,156}, { 9,168}, {10,48}, {-1, -1}, {-1, -1} },

    // Node 6 (index 6)
    {{0,0}, {17,216}, {18,228}, {19,84}, {-1, -1}, {-1, -1} },

    // Node 7 (index 7)
    {{0,0}, {24,60}, {30,196}, {32,240}, {-1, -1}, {-1, -1} },

    // Node 8 (index 8)
    {{1,1}, {3,2}, {7, 4}, {10, 5}, {-1, -1}, {-1, -1} },

    // Node 9 (index 9)
    {{1,1}, {15,3}, {19,6}, {32,7}, {-1, -1}, {-1, -1} }
};
}



//----------------------------------------------------------[PRINT]----------------------------------------------------------------
void BTree::printBTree() {

    // printBTree implementation

    for (int i = 0; i < index.size(); i++)
    {
        for (int j = 0; j < index[i].size(); j++)
        {
            cout << "("<<index[i][j].first << " " << index[i][j].second << ")" << " , ";
        }
        cout << endl;
        
    }

}



int main(){
    BTree btree("INDEX.txt",10,5);
    // cout << "\n------------------------------------------------------------------------\n";
    // btree.printBTree();
    // cout << "\n------------------------------------------------------------------------\n";

    // btree.insert(3,12);
    // btree.insert(10,48);
    // btree.insert(7,24);
    // btree.insert(24,60);
    // btree.insert(14,72);
    // btree.insert(19,84);
    // cout << "\n------------------------------------------------------------------------\n";
    // btree.printBTree();
    // cout << "\n------------------------------------------------------------------------\n";
    // return 0;
    //cout<<btree.used_cells(1)<<endl;

    int choice;
    bool check = true;

    int records, nodes, id, ref;
    cout<<"Please enter the number of records in the index file followed by the number of nodes in each record:";

    while (check)
    {
        start:
        cout<<"\n\nPlease enter the number of the operation you want:\n"
            <<"1- Insert a new record in the index file.\n"
            <<"2- Delete a record from the index file.\n"
            <<"3- Search for a record.\n"
            <<"4- Display the index file on the console.\n"
            <<"5- Exits.\n";

        cin>>choice;

        switch (choice)
        {
            case 1:
            {
                cout<<"Please enter the ID of the record followed by its reference:";
                cin>>id >>ref;
                // for(int i=1;i<btree.numOfRecords;i++)
                // {
                //     btree.sortNode(btree.index[i]);
                // }
                btree.insert(id, ref);
                    //cout<<"There is no place left in the index file!\n";
                    // cout<<"Record inserted in the index file successfully.\n";
                break;
            }
            case 2:
            {
                cout<<"Please enter the ID of the record:";
                cin>>id;
                for(int i=1;i<btree.numOfRecords;i++)
                {
                    btree.removesort(i);
                }
                btree.remove(id);
                cout<<"Record deleted successfully.\n";
                break;
            }
            case 3:
            {
                cout<<"Please enter the ID of the record:";
                cin>>id;
                ref = btree.search(id);
                if (ref == -1)
                    cout<<"Record is not in the index file!\n";
                else
                    cout<<"The reference of the record is " <<ref <<".\n";
                break;
            }
            case 4:
            {
                // for(int i=1;i<btree.numOfRecords;i++)
                // {
                //     btree.sortNode(btree.index[i]);
                // }
                // btree.printBTree();
                for(int i=1;i<btree.numOfRecords;i++)
                {
                    btree.removesort(i);
                }
                btree.printBTree();
                break;
            }
            case 5:
            {
                return 0;
            }
            default:
            {

                cout<<"The number you entered is invalid!\n";
                goto start;
            }
        }
    }


}

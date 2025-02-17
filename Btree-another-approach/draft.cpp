//                                                      بسم الله الرحمن الرحيم                                                 //
// program: draft.cpp
// Description:
// Author:  Abdallah Gasem
// Date: 25-01-2025
// Version: 1.0
/* file run command: g++ -o draft draft.cpp; if ($?) { ./draft } */
// ----------------------------------------------------------------------------------------------------------------------------- //

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// sort an index node by the node.first
void sortNode(vector<pair<int, int>> &v)
{
    sort(v.begin() + 1, v.end(), [](const pair<int, int> &a, const pair<int, int> &b)
         { return a.first < b.first; });
}

void refreshHead(vector<vector<pair<int, int>>> &index)
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
        index[0][1].second = emptyNodeIdx;
        
        // debuggingg //
        cout << "refreshHead(): the new head after refresh is " << emptyNodeIdx << endl;
        // debuggingg //

        return;
    }

    cout << "\n refreshHead(): hmmmm, something is wrong you should not reach this message! \n";    
}

int main()
{
    vector<vector<pair<int, int>>> test = {
        {{-1, -1}, {-1, 1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
        {{0, 0}, {3, 12}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
        {{-1, -1}, {-1, 3}, {-1, -1}, {-1, -1}, {-1, -1}, {10,48}},
        {{-1, -1}, {-1, 4}, {66, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
        {{-1, -1}, {-1, 5}, {-1, -1}, {2, -1}, {-1, -1}, {-1, -1}},
        {{-1, -1}, {-1, 6}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
        {{-1, -1}, {-1, 7}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
        {{-1, -1}, {-1, 8}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
        {{-1, -1}, {-1, 9}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}},
        {{-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}, {-1, -1}}};

    // sortNode(test[0]);

    // for (int i = 0; i < test[0].size(); i++)
    // {
    //     cout << "(" << test[0][i].first << " " << test[0][i].second << ")" << " , ";
    // }

    refreshHead(test);

    return 0;
};

#include <iostream>
#include <stack>
#include <queue>
#include "BTree.h"

using namespace std;

// constructor
BTree :: BTree (int records, int nodes)
{
    this->size = 0;
    this->records = records;
    this->nodes = nodes;    // m

    this->file.open("BTreeIndex.txt", ios::out | ios::binary);
    if (file.is_open())
    {
        for (int i = 0; i < records; i++)
        {
            int num1 = -1, num2 = (i == records - 1)? -1 : i + 1;
            char space = ' ', line = '\n';
            for (int j = 0; j < nodes*2 + 1; j++)
            {
                if (j == 1)
                    file.write((char*) &num2, sizeof(num2));
                else
                    file.write((char*) &num1, sizeof(num1));

                if (j == nodes*2)
                    file.write(&line, sizeof(line));
                else
                    file.write(&space, sizeof(space));
            }
        }
        file.close();
        file.open("BTreeIndex.txt", ios::in | ios::out | ios::binary);
    }
    else
        cout<<"Error opening file!\n";
}

int BTree :: insert (int id, int ref)
{
    file.clear();

    if (size == 0)
    {
        int nextEmptyNode, recordStatus = 0;

        file.seekp(nodes * 10 + 5, ios::beg);
        file.write((char *) &recordStatus, sizeof(recordStatus));
        file.seekp(1, ios::cur);

        file.seekg(file.tellp(), ios::beg);
        file.read((char *) &nextEmptyNode, sizeof(nextEmptyNode));

        file.seekp(-4, ios::cur);
        file.write((char *) &id, sizeof(id));
        file.seekp(1, ios::cur);
        file.write((char *) &ref, sizeof(ref));

        file.seekp(5, ios::beg);
        file.write((char *) &nextEmptyNode, sizeof(nextEmptyNode));

        file.flush();
        size++;
        return 1;
    }
    else
    {
        stack <int> accessedRecords;
        priority_queue <pair<int, int>> currRecord, splitRecord1, splitRecord2;
        int nextEmptyNode, recordStatus, currId, currRef, maxId, newRef, sizeDiff;
        bool firstLoop = true;

        file.seekg(nodes * 10 + 5, ios::beg);
        accessedRecords.push(1);

        for (int i = 0; i < records; i++)
        {
            file.read((char *) &recordStatus, sizeof(recordStatus));
            file.seekg(1, ios::cur);

            if (recordStatus == 0)
                break;

            for (int j = 0; j < nodes; j++)
            {
                file.read((char *) &currId, sizeof(currId));
                file.seekg(1, ios::cur);
                file.read((char *) &currRef, sizeof(currRef));
                file.seekg(1, ios::cur);

                if (currId == -1)
                    break;

                if (id < currId || j == nodes - 1)
                {
                    file.seekg((nodes * 10 + 5) * currRef, ios::beg);
                    accessedRecords.push(currRef);
                    break;
                }
            }
        }

        currRecord.emplace(-id, ref);
    }
}

void BTree :: remove (int id)
{

}

int BTree :: search (int id)
{
    file.clear();
    file.seekg(nodes * 10 + 5, ios::beg);
    int currId, currRef, recordStatus;

    for (int i = 0; i < records; i++)
    {
        file.read((char *) &recordStatus, sizeof(recordStatus));
        file.seekg(1, ios::cur);

        if (recordStatus == -1)
            break;

        for (int j = 0; j < nodes; j++)
        {
            file.read((char *) &currId, sizeof(currId));
            file.seekg(1, ios::cur);
            file.read((char *) &currRef, sizeof(currRef));
            file.seekg(1, ios::cur);

            if (currId == -1)
                return -1;

            if (id <= currId && recordStatus == 1)
            {
                file.seekg((nodes * 10 + 5) * currRef, ios::beg);
                break;
            }
            else if (id == currId && recordStatus == 0)
                return currRef;
        }
    }
    return -1;
}

void BTree :: printBTree()
{
    file.clear();
    file.seekg(0, ios::beg);
    int num; char c;
    while (file.read((char*) &num, sizeof(num)))
    {
        cout<<num;
        file.read(&c, sizeof(c));
        cout<<c;
    }
}

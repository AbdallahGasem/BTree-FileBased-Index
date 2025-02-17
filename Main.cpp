#include <iostream>
#include "BTree.h"
#include "BTree.cpp"

using namespace std;

int main()
{
    int choice;
    bool check = true;

    int records, nodes, id, ref;
    cout<<"Please enter the number of records in the index file followed by the number of nodes in each record:";
    cin>>records >>nodes;
    BTree bTree(records, nodes);

    while (check)
    {
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
                if (bTree.insert(id, ref) == -1)
                    cout<<"There is no place left in the index file!\n";
                else
                    cout<<"Record inserted in the index file successfully.\n";
                break;
            }
            case 2:
            {
                cout<<"Please enter the ID of the record:";
                cin>>id;
                bTree.remove(id);
                cout<<"Record deleted successfully.\n";
                break;
            }
            case 3:
            {
                cout<<"Please enter the ID of the record:";
                cin>>id;
                ref = bTree.search(id);
                if (ref == -1)
                    cout<<"Record is not in the index file!\n";
                else
                    cout<<"The reference of the record is " <<ref <<".\n";
                break;
            }
            case 4:
            {
                bTree.printBTree();
                break;
            }
            case 5:
            {
                check = false;
                break;
            }
            default:
            {
                cout<<"The number you entered is invalid!\n";
            }
        }
    }
}

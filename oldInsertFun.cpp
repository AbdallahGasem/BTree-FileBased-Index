// int BTree :: insert (int id, int ref)
// {
//     file.clear();

//     if (size == 0)
//     {
//         int nextEmptyNode, recordStatus = 0;

//         file.seekp(nodes * 10 + 5, ios::beg);
//         file.write((char*) &recordStatus, sizeof(recordStatus));
//         file.seekp(1, ios::cur);

//         file.seekg(file.tellp(), ios::beg);
//         file.read((char*) &nextEmptyNode, sizeof(nextEmptyNode));

//         file.seekp(-4, ios::cur);
//         file.write((char*) &id, sizeof(id));
//         file.seekp(1, ios::cur);
//         file.write((char*) &ref, sizeof(ref));

//         file.seekp(5, ios::beg);
//         file.write((char*) &nextEmptyNode, sizeof(nextEmptyNode));

//         file.flush();
//         size++;
//         return 1;
//     }
//     else
//     {
//         stack <int> accessedRecords;
//         priority_queue <pair<int, int>> currRecord, splitRecord1, splitRecord2;
//         int nextEmptyNode, recordStatus, currId, currRef, maxId, newRef, sizeDiff;
//         bool firstLoop = true;

//         file.seekg(nodes * 10 + 5, ios::beg);
//         accessedRecords.push(1);

//         for (int i = 0; i < records; i++)
//         {
//             file.read((char *) &recordStatus, sizeof(recordStatus));
//             file.seekg(1, ios::cur);

//             if (recordStatus == 0)
//                 break;

//             for (int j = 0; j < nodes; j++)
//             {
//                 file.read((char *) &currId, sizeof(currId));
//                 file.seekg(1, ios::cur);
//                 file.read((char *) &currRef, sizeof(currRef));
//                 file.seekg(1, ios::cur);

//                 if (currId == -1)
//                     break;

//                 if (id < currId || j == nodes - 1)
//                 {
//                     file.seekg((nodes * 10 + 5) * currRef, ios::beg);
//                     accessedRecords.push(currRef);
//                     break;
//                 }
//             }
//         }
        
//         currRecord.emplace(-id, ref);

//         while (!accessedRecords.empty())
//         {
//             file.seekg((nodes * 10 + 5) * accessedRecords.top(), ios::beg);
//             file.read((char*) &recordStatus, sizeof(recordStatus));
//             file.seekg(1, ios::cur);

//             if (firstLoop || recordStatus)
//             {
//                 for (int i = 0; i < nodes; i++)
//                 {
//                     file.read((char*) &currId, sizeof(currId));
//                     file.seekg(1, ios::cur);
//                     file.read((char*) &currRef, sizeof(currRef));
//                     file.seekg(1, ios::cur);
//                     if (currId == -1)
//                         break;
//                     currRecord.emplace(-currId, currRef);
//                 }
//             }

//             if (currRecord.size() > nodes)
//             {
//                 for (int i = 0; i < nodes + 1; i++)
//                 {
//                     if (i > nodes/2)
//                         splitRecord2.push(currRecord.top());
//                     else
//                         splitRecord1.push(currRecord.top());

//                     currRecord.pop();
//                 }

//                 file.seekg(5, ios::beg);
//                 file.read((char*) &nextEmptyNode, sizeof(nextEmptyNode));
//                 recordStatus = (firstLoop)? 0 : 1;
//                 sizeDiff = nodes - splitRecord1.size();

//                 if (accessedRecords.size() > 1)
//                 {
//                     file.seekp((nodes * 10 + 5) * accessedRecords.top(), ios::beg);
//                     nextEmptyNode--;
//                 }
//                 else
//                 {
//                     file.seekp((nodes * 10 + 5) * nextEmptyNode, ios::beg);
//                 }

//                 file.write((char*) &recordStatus, sizeof(recordStatus));
//                 file.seekp(1, ios::cur);

//                 while(!splitRecord1.empty())
//                 {
//                     if (splitRecord1.size() == 1)
//                         currRecord.emplace(splitRecord1.top().first, nextEmptyNode);

//                     currId = -splitRecord1.top().first;
//                     currRef = splitRecord1.top().second;

//                     file.write((char*) &currId, sizeof(currId));
//                     file.seekp(1, ios::cur);
//                     file.write((char*) &currRef, sizeof(currRef));
//                     file.seekp(1, ios::cur);

//                     splitRecord1.pop();
//                 }

//                 for (int i = 0; i < sizeDiff; i++)
//                 {
//                     currId = currRef = -1;

//                     file.write((char*) &currId, sizeof(currId));
//                     file.seekp(1, ios::cur);
//                     file.write((char*) &currRef, sizeof(currRef));
//                     file.seekp(1, ios::cur);
//                 }

//                 nextEmptyNode++;
//                 file.seekp((nodes * 10 + 5) * nextEmptyNode, ios::beg);
//                 file.write((char*) &recordStatus, sizeof(recordStatus));
//                 file.seekp(1, ios::cur);

//                 file.seekg(file.tellp(), ios::beg);
//                 file.read((char*) &nextEmptyNode, sizeof(nextEmptyNode));
//                 file.seekg(-4, ios::cur);
//                 file.seekp(file.tellg(), ios::beg);

//                 while(!splitRecord2.empty())
//                 {
//                     if (splitRecord2.size() == 1)
//                         currRecord.emplace(splitRecord2.top().first, nextEmptyNode - 1);

//                     currId = -splitRecord2.top().first;
//                     currRef = splitRecord2.top().second;

//                     file.write((char*) &currId, sizeof(currId));
//                     file.seekp(1, ios::cur);
//                     file.write((char*) &currRef, sizeof(currRef));
//                     file.seekp(1, ios::cur);

//                     splitRecord2.pop();
//                 }

//                 file.seekp(5, ios::beg);
//                 file.write((char*) &nextEmptyNode, sizeof(nextEmptyNode));
//                 firstLoop = false;

//                 if (accessedRecords.size() > 1)
//                     accessedRecords.pop();
//             }
//             else
//             {
//                 file.seekp((nodes * 10 + 5) * accessedRecords.top(), ios::beg);
//                 recordStatus = (firstLoop)? 0 : 1;
//                 sizeDiff = nodes - currRecord.size();

//                 file.write((char*) &recordStatus, sizeof(recordStatus));
//                 file.seekp(1, ios::cur);

//                 while (!currRecord.empty())
//                 {
//                     if (currRecord.size() == 1)
//                         maxId = -currRecord.top().first;

//                     currId = -currRecord.top().first;
//                     currRef = currRecord.top().second;

//                     file.write((char*) &currId, sizeof(currId));
//                     file.seekp(1, ios::cur);
//                     file.write((char*) &currRef, sizeof(currRef));
//                     file.seekp(1, ios::cur);

//                     currRecord.pop();
//                 }

//                 for (int i = 0; i < sizeDiff; i++)
//                 {
//                     currId = currRef = -1;

//                     file.write((char*) &currId, sizeof(currId));
//                     file.seekp(1, ios::cur);
//                     file.write((char*) &currRef, sizeof(currRef));
//                     file.seekp(1, ios::cur);
//                 }

//                 newRef = accessedRecords.top();
//                 accessedRecords.pop();
//                 while (!accessedRecords.empty())
//                 {
//                     file.seekg((nodes * 10 + 5) * accessedRecords.top() + 5, ios::beg);
//                     for (int i = 0; i < nodes; i++)
//                     {
//                         file.read((char*) &currId, sizeof(currId));
//                         file.seekg(1, ios::cur);
//                         file.read((char*) &currRef, sizeof(currRef));
//                         file.seekg(1, ios::cur);

//                         if (currRef == accessedRecords.top() && currId != maxId)
//                         {
//                             file.seekg(-10, ios::cur);
//                             file.seekp(file.tellg(), ios::beg);
//                             file.write((char*) &maxId, sizeof(maxId));
//                             file.seekp(6, ios::cur);
//                             file.seekg(file.tellp(), ios::beg);
//                         }
//                     }
//                     accessedRecords.pop();
//                 }

//                 file.flush();
//                 size++;
//                 return newRef;
//             }
//         }
//     }
// }

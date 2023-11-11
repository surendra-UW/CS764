#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <queue>
#include "MinHeapNode.h"
#include "RecordStructure.h"
using namespace std;

FILE *openFile(char *fileName, char *mode)
{
    FILE *fp = fopen(fileName, mode);
    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }
    return fp;
}

void externalSort(char *input_file, char *output_file, int num_ways, int run_size)
{
    createInitialRuns(input_file, run_size, num_ways);
    mergeFiles(output_file, run_size, num_ways);
}

bool compareByMember1(const RecordStructure &a, const RecordStructure &b)
{
    return a.member1 < b.member1;
}

void createInitialRuns(char *input_file, int run_size, int num_ways)
{
    vector<RecordStructure> arr;
    bool more_input = true;
    int next_output_file = 0;

    vector<FILE *> out(num_ways);

    for (int i = 0; i < num_ways; i++)
    {
        char filename[50];
        sprintf(filename, "output%d.txt", i);
        out[i] = fopen(filename, "w");
        if (!out[i])
        {
            std::cerr << "Error opening output file." << std::endl;
            return;
        }
    }

    FILE *in = fopen(input_file, "r");
    if (!in)
    {
        cerr << "Error opening input file." << endl;
        return;
    }
    std::string inp_file_string(input_file);
    while (more_input)
    {
        arr.clear();
        int value1, value2, value3, value4;
        for (int i = 0; i < run_size; i++)
        {
            if (fscanf(in, "%d,%d,%d,%d", &value1, &value2, &value3, &value4) != 3)
            {
                more_input = false;
                break;
            }

            RecordStructure r = {value1, value2, value3, value4};
            arr.push_back(r);
        }
        // readCSVRows(inp_file_string, run_size, arr);
        sort(arr.begin(), arr.end(), compareByMember1); // do custom sort
        cout << "Size of arr = " << arr.size() << endl;

        for (int j = 0; j < arr.size(); j++)
        {
            cout << "Values added to " << next_output_file << " are " << arr[j].member1 << "\t" << arr[j].member2 << "\t" << arr[j].member3 << endl;
            fprintf(out[next_output_file], "%d,%d,%d,%d\n", arr[j].member1, arr[j].member2, arr[j].member3, arr[j].member4);
            cout << "Added record to " << next_output_file << endl;
        }

        next_output_file++;
    }

    for (int i = 0; i < num_ways; i++)
        fclose(out[i]);

    fclose(in);
}

void mergeFiles(char *output_file, int n, int k)
{
    FILE *in[k];
    for (int i = 0; i < k; i++)
    {
        char fileName[50];
        snprintf(fileName, sizeof(fileName), "output%d.txt", i);
        in[i] = openFile(fileName, "r");
    }

    FILE *out = openFile(output_file, "w");
    int i, val1, val2, val3;

    priority_queue<MinHeapNode> pq;

    for (i = 0; i < k; i++)
    {
        if (fscanf(in[i], "%d,%d,%d", &val1, &val2, &val3) != 3)
            break;
        // cout<<"Val ="<<val<<"\ti="<<i<<endl;
        RecordStructure r = {val1, val2, val3};
        MinHeapNode record = {r, i};
        cout << "Adding element to priority queue\t" << record.element.member1 << "\t" << record.i << endl;
        pq.push(record);
    }
    cout << "Size of priority queue " << pq.size() << endl;
    int count = 0;

    while (count != i)
    {
        MinHeapNode root = pq.top();

        fprintf(out, "%d,%d,%d\n", root.element.member1, root.element.member2, root.element.member3);
        cout << "Removed value of root from run " << root.i << " is " << root.element.member1 << endl;
        if (fscanf(in[root.i], "%d,%d,%d", &val1, &val2, &val3) != 3)
        {
            root.element.member1 = INT_MAX;
            count++;
        }
        else
        {
            root.element.member1 = val1;
            root.element.member2 = val2;
            root.element.member3 = val3;
        }
        cout << "New value of root from run " << root.i << " is " << root.element.member1 << endl;
        pq.pop();
        pq.push(root);
    }
    for (int i = 0; i < k; i++)
        fclose(in[i]);

    fclose(out);
}
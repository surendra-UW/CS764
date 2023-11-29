#include <bits/stdc++.h>
#include "RecordStructure.h"
#include "TournamentTree.h"
using namespace std;

void swap(TournamentTreeNode *x, TournamentTreeNode *y);

// int TournamentTree::left(int i) { return (2 * i); }

TournamentTree::TournamentTree(TournamentTreeNode a[], int size)
{
    this->num_nodes = size;
    this->tournamentTree = a;
}

void swap(TournamentTreeNode *x, TournamentTreeNode *y)
{
    TournamentTreeNode temp = *x;
    *x = *y;
    *y = temp;
}

void merge(RecordStructure arr[], int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 = r - m;

    RecordStructure L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (L[i].members[0] <= R[j].members[0])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }

    while (i < n1)
        arr[k++] = L[i++];

    while (j < n2)
        arr[k++] = R[j++];
}

void mergeSort(RecordStructure arr[], int l, int r)
{
    if (l < r)
    {
        int m = l + (r - l) / 2;
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

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

void readNextValueFromRun(FILE *in[], int k, int run_id, TournamentTreeNode *tournamentTree)
{

    // Note the runId needs to be set explicitly here
    tournamentTree[k + run_id].offsetValueCode = -1;
    tournamentTree[k + run_id].offset = 0;

    if (fscanf(in[run_id], "%d,%d,%d,%d,%d",
               &tournamentTree[k + run_id].element.members[0],
               &tournamentTree[k + run_id].element.members[1],
               &tournamentTree[k + run_id].element.members[2],
               &tournamentTree[k + run_id].element.members[3],
               &tournamentTree[k + run_id].element.members[4]) != 5)
        return;
    printf("\nNew value read = %d ", tournamentTree[k + run_id].element.members[0]);
    printf("Its offset = %d", tournamentTree[k + run_id].offsetValueCode);
}

void performTreeOfLosersSort(FILE *in[], TournamentTree tt, int k)
{
    TournamentTreeNode *tournamentTree = tt.getTree();
    while (tournamentTree[0].element.members[0] != INT_MAX)
    {
        // TODO: Write this record to output file
        printf("Value written to output file\t=\t%d\n", tournamentTree[0].element.members[0]);

        readNextValueFromRun(in, k, tournamentTree[0].runId, tournamentTree);

        printf("Printing Tree before UPDATING THE STRUCTURE\n");
        tt.printTreeOfLosers();
        tt.updateTreeStructure(
            tournamentTree[k + tournamentTree[0].runId],
            tt.getParentIndex(k + tournamentTree[0].runId),
            tournamentTree[tt.getParentIndex(k + tournamentTree[0].runId)]);
        printf("Printing Tree after UPDATING THE STRUCTURE\n");
        tt.printTreeOfLosers();
    }
}

void mergeFiles(char *output_file, int n, int k)
{
    FILE *in[k];
    for (int i = 0; i < k; i++)
    {
        char fileName[10];
        snprintf(fileName, sizeof(fileName), "run-%d", i);
        in[i] = openFile(fileName, "r");
    }

    FILE *out = openFile(output_file, "w");

    // Create a tournament tree with k nodes. Every leaf node
    // has first element of a run

    // k -> denotes the #runs
    // There are 2k nodes in the tree
    TournamentTreeNode *tournamentTree = new TournamentTreeNode[2 * k];
    int i;
    for (i = 0; i < k; i++)
    {
        // break if no output file is empty and
        // index i will be no. of input files
        int run_index_in_tree = k + i;
        if (fscanf(in[i], "%d,%d,%d,%d,%d",
                   &tournamentTree[run_index_in_tree].element.members[0],
                   &tournamentTree[run_index_in_tree].element.members[1],
                   &tournamentTree[run_index_in_tree].element.members[2],
                   &tournamentTree[run_index_in_tree].element.members[3],
                   &tournamentTree[run_index_in_tree].element.members[4]) != 5)
            break;

        tournamentTree[run_index_in_tree].runId = i;
    }

    TournamentTree tt(tournamentTree, 2 * k);
    printf("Printing Tree before comparing\n");
    tt.printTreeOfLosers();

    for (int i = k; i < 2 * k; i++)
    {
        // printf("Computing initial Offset Value Code for Index %d\n", i);
        tt.computeOffsetValueCode(i);
        // printf("\nComputed offset value code for this node = %d\n", tt.getTree()[i].offsetValueCode);
    }
    tt.compareInitialNodes();
    // printf("Printing Tree after comparing\n");
    tt.setRootNode();
    tt.initialPop(1);
    // tt.printTreeOfLosers();

    performTreeOfLosersSort(in, tt, k);
    // tt.printTreeOfLosers();
    // return;

    for (int i = 0; i < k; i++)
        fclose(in[i]);

    fclose(out);
}

void createInitialRuns(char *input_file, int run_size, int num_ways)
{
    FILE *in = openFile(input_file, "r");
    FILE *out[num_ways];
    char fileName[10];
    for (int i = 0; i < num_ways; i++)
    {
        snprintf(fileName, sizeof(fileName), "run-%d", i);
        out[i] = openFile(fileName, "w");
    }

    RecordStructure *arr = (RecordStructure *)malloc(run_size * sizeof(RecordStructure));

    bool more_input = true;
    int next_output_file = 0;

    int i;
    while (more_input)
    {
        uint64_t members[5];
        for (i = 0; i < run_size; i++)
        {
            if (fscanf(in, "%d,%d,%d,%d,%d",
                       &members[0], &members[1], &members[2], &members[3], &members[4]) != 5)
            // TODO: Check if theres a better way to generalize
            {
                more_input = false;
                break;
            }
            RecordStructure r = {*members};
            arr[i] = r;
        }

        mergeSort(arr, 0, i - 1);

        for (int j = 0; j < i; j++)
            fprintf(out[next_output_file], "%d,%d,%d,%d,%d\n", arr[j].members[0], arr[j].members[1], arr[j].members[2], arr[j].members[3], arr[j].members[4]);

        next_output_file++;
    }

    for (int i = 0; i < num_ways; i++)
        fclose(out[i]);

    fclose(in);
}

void externalSort(char *input_file, char *output_file, int num_ways, int run_size)
{
    createInitialRuns(input_file, run_size, num_ways);
    mergeFiles(output_file, run_size, num_ways);
}

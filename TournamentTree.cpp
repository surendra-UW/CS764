#include <bits/stdc++.h>
#include <queue>
#include <unistd.h>
#include "Sort.h"
#include "RecordStructure.h"
#include "TournamentTree.h"
#include "Cache.h"
#include "constants.h"
#include "defs.h"

using namespace std;

// int TournamentTree::left(int i) { return (2 * i); }

TournamentTree::TournamentTree(TournamentTreeNode a[], int size)
{
    this->num_nodes = size;
    this->tournamentTree = a;
}

TournamentTreeNode *TournamentTree::getTree()
{
    return this->tournamentTree;
}
int TournamentTree::left(int i) { return (2 * i); }
int TournamentTree::right(int i) { return (2 * i + 1); }
int TournamentTree::getNodeCount() { return num_nodes; }
TournamentTreeNode TournamentTree::getMin() { return tournamentTree[0]; }

bool TournamentTree::isLeafNode(int index)
{
    return left(index) >= num_nodes || right(index) >= num_nodes;
}

int TournamentTree::getParentIndex(int i)
{
    return floor(i / 2);
}

void TournamentTree::printTreeOfLosers()
{
    for (int i = 0; i < num_nodes; i++)
    {
        printf("%d\t%d\t%d\n", i, tournamentTree[i].element.members[0], tournamentTree[i].runId);
    }
}

void TournamentTree::computeOffsetForLoser(int loserIndex, int winnerIndex)
{
    // TODO: INCOMPLETE
    fullComparison(loserIndex, winnerIndex, tournamentTree[loserIndex].offset);

    tournamentTree[loserIndex].offsetValueCode = (FIELD_COUNT - tournamentTree[loserIndex].offset) * 100 + tournamentTree[loserIndex].element.members[tournamentTree[loserIndex].offset];

    printf("\ncomputeOffsetForLoser:: OVC(%d) = %d, OVC(%d) = %d", tournamentTree[loserIndex].offsetValueCode, loserIndex, tournamentTree[winnerIndex].offsetValueCode, winnerIndex);
}

void TournamentTree::computeOffsetForLoser(TournamentTreeNode loser, TournamentTreeNode winner)
{
    fullComparison(loser, winner, loser.offset);

    loser.offsetValueCode = (FIELD_COUNT - loser.offset) * 100 + loser.element.members[loser.offset];

    printf("\ncomputeOffsetForLoser:: OVC(%d) = %d, OVC(%d) = %d", loser.offsetValueCode, winner.offsetValueCode);
}

void TournamentTree::computeOffsetValueCode(int node_index)
{
    tournamentTree[node_index].offsetValueCode = (FIELD_COUNT - tournamentTree[node_index].offset) * 100 + tournamentTree[node_index].element.members[tournamentTree[node_index].offset];
}

void TournamentTree::compareNodeValues(int left, int right)
{
    int parent_index = getParentIndex(left);
    bool computeNewOffsetValueCode = false;
    if (isLeftLesserThanRight(left, right, computeNewOffsetValueCode))
    {
        if (computeNewOffsetValueCode)
            computeOffsetForLoser(right, left);
        tournamentTree[parent_index] = tournamentTree[left];
    }
    else
    {
        if (computeNewOffsetValueCode)
            computeOffsetForLoser(left, right);
        tournamentTree[parent_index] = tournamentTree[right];
    }
}

void TournamentTree::fullComparison(TournamentTreeNode left, TournamentTreeNode right, int offset)
{
    while (left.offset < FIELD_COUNT)
    {
        if (left.element.members[offset] ==
            right.element.members[offset])
        {
            left.offset++;
            offset++;
            // tournamentTree[right].offset++;
            // TODO: Note if left and right are considered as loser and winner, it doesnt make sense to increment the offset of the winner
        }
        else
        {
            break;
        }
    }
}

void TournamentTree::fullComparison(int left, int right, int offset)
{
    while (tournamentTree[left].offset < FIELD_COUNT)
    {
        if (tournamentTree[left].element.members[offset] ==
            tournamentTree[right].element.members[offset])
        {
            tournamentTree[left].offset++;
            offset++;
            // tournamentTree[right].offset++;
            // TODO: Note if left and right are considered as loser and winner, it doesnt make sense to increment the offset of the winner
        }
        else
        {
            break;
        }
    }
}

bool TournamentTree::isLeftLesserThanRight(TournamentTreeNode left, TournamentTreeNode right, bool &computeNewOffsetValueCode)
{
    // check if nodes have the offsets computed
    // case 1: Yes
    printf("\nisLeftLesserThanRight:: OVC(left) = %d, OVC(right) = %d", left.offsetValueCode, right.offsetValueCode);

    if (left.offsetValueCode != -1 && right.offsetValueCode != -1)
    {
        if (left.offsetValueCode == right.offsetValueCode)
        {
            // do full comparison starting at offset
            int offset = left.offset;
            fullComparison(left, right, left.offset);
            // TODO: Should the individual offsets be set to 0 after this?
            return left.offset < FIELD_COUNT && (left.element.members[offset] < right.element.members[offset]);
        }
        else
        {
            return left.offsetValueCode < right.offsetValueCode;
        }
        computeNewOffsetValueCode = false;
    }
    else
    {
        fullComparison(left, right, left.offset);
        computeNewOffsetValueCode = true;
        return left.offset < FIELD_COUNT && (left.element.members[left.offset] < right.element.members[left.offset]);
    }
    /*
    If a.OVC is computed && b.OVC is computed:
        Compare a and b using OVC
    a.OVC == b.OVC
                Compare values starting at a.offset for both; return the smaller as winner
    a.OVC != b.OVC
    Return the smaller as the winner
    */
    // case 2:
    /*
        One of the nodes dont have the OVC computed
        Do full comparison
        Compute OVC of loser w.r.t winner
    */
}

bool TournamentTree::isLeftLesserThanRight(int left, int right, bool &computeNewOffsetValueCode)
{
    // check if nodes have the offsets computed
    // case 1: Yes
    printf("\nisLeftLesserThanRight:: OVC(%d) = %d, OVC(%d) = %d", tournamentTree[left].offsetValueCode, left, tournamentTree[right].offsetValueCode, right);

    if (tournamentTree[left].offsetValueCode != -1 && tournamentTree[right].offsetValueCode != -1)
    {
        if (tournamentTree[left].offsetValueCode == tournamentTree[right].offsetValueCode)
        {
            // do full comparison starting at offset
            int offset = tournamentTree[left].offset;
            fullComparison(left, right, tournamentTree[left].offset);
            // TODO: Should the individual offsets be set to 0 after this?
            return tournamentTree[left].offset < FIELD_COUNT && (tournamentTree[left].element.members[offset] < tournamentTree[right].element.members[offset]);
        }
        else
        {
            return tournamentTree[left].offsetValueCode < tournamentTree[right].offsetValueCode;
        }
        computeNewOffsetValueCode = false;
    }
    else
    {
        fullComparison(left, right, tournamentTree[left].offset);
        computeNewOffsetValueCode = true;
    }
    /*
    If a.OVC is computed && b.OVC is computed:
        Compare a and b using OVC
    a.OVC == b.OVC
                Compare values starting at a.offset for both; return the smaller as winner
    a.OVC != b.OVC
    Return the smaller as the winner
    */
    // case 2:
    /*
        One of the nodes dont have the OVC computed
        Do full comparison
        Compute OVC of loser w.r.t winner
    */
}

void TournamentTree::compareInitialNodes()
{
    for (int i = this->num_nodes - 1; i >= 1; i -= 2)
    {
        compareNodeValues(i, i - 1);
    }
}

void TournamentTree::setRootNode()
{
    tournamentTree[0] = tournamentTree[1];
}

bool TournamentTree::areNodesEqual(int a, int b)
{
    return (tournamentTree[a].runId == tournamentTree[b].runId && tournamentTree[a].element.members[0] == tournamentTree[b].element.members[0]);
}

void TournamentTree::initialPop(int index)
{
    if (isLeafNode(index))
        return;
    tournamentTree[index] = (areNodesEqual(index, left(index))) ? tournamentTree[right(index)] : tournamentTree[left(index)];
    initialPop(left(index));
    initialPop(right(index));
}

void TournamentTree::updateTreeStructure(TournamentTreeNode node, int parent_index, TournamentTreeNode parent)
{
    if (parent_index == 0)
    {
        tournamentTree[0] = node;
        return;
    }
    TournamentTreeNode larger, smaller, grandParent;
    int grand_parent_index = getParentIndex(parent_index), smaller_index;
    bool computeNewOffsetValueCode = false;
    if (isLeftLesserThanRight(node, parent, computeNewOffsetValueCode))
    {

        larger = parent;
        smaller = node;
    }
    else
    {
        larger = node;
        smaller = parent;
    }
    if (computeNewOffsetValueCode)
        computeOffsetForLoser(larger, smaller);

    grandParent = tournamentTree[grand_parent_index];
    tournamentTree[parent_index] = larger;

    updateTreeStructure(smaller, grand_parent_index, grandParent);
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

void readNextValueFromRun(Cache c, vector<queue<RecordStructure>> in, int k, int run_id, TournamentTreeNode *tournamentTree)
{

    // Note the runId needs to be set explicitly here
    tournamentTree[k + run_id].offsetValueCode = -1;
    tournamentTree[k + run_id].offset = 0;

    if (in[run_id].empty() == false)
    {
        tournamentTree[k + run_id].element = in[run_id].front();
        tournamentTree[k + run_id].runId = run_id;
        in[run_id].pop();
    }
    else
    {
        // load data into this cache_array for run i
        in[run_id] = c.loadDataForRun(run_id);
    }

    printf("\nNew value read = %d ", tournamentTree[k + run_id].element.members[0]);
    printf("Its offset = %d", tournamentTree[k + run_id].offsetValueCode);
}

void appendDataToDestination(char *sourceFilePath, char *destinationFilePath)
{
    FILE *sourceFile = fopen(sourceFilePath, "r");
    FILE *destinationFile = fopen(destinationFilePath, "a");

    if (sourceFile && destinationFile)
    {
        // Move to the end of the destination file
        fseek(destinationFile, 0, SEEK_END);

        // Get the current position, which is the size of the destination file
        long currentPosition = ftell(destinationFile);
        cout << "Current Position in destination = " << currentPosition << endl;
        // Move back to the beginning of the source file
        fseek(sourceFile, 0, SEEK_SET);
        fputc('\n', destinationFile);
        // Read and write in chunks to avoid overwriting existing data in destination file
        char buffer[1024];
        size_t bytesRead;

        while ((bytesRead = fread(buffer, 1, sizeof(buffer), sourceFile)) > 0)
        {
            fwrite(buffer, 1, bytesRead, destinationFile);
        }

        printf("File appended successfully.\n");

        fclose(sourceFile);
        fclose(destinationFile);
    }
    else
    {
        printf("Error opening files.\n");
    }
}

bool isFileFull(char *filePath, long sizeToCheck)
{
    FILE *file = fopen(filePath, "r");
    if (file)
    {
        // Move to the end of the file
        fseek(file, 0, SEEK_END);

        // Get the current position, which is the size of the file
        long size = ftell(file);

        if (size == sizeToCheck)
        {
            printf("File size is exactly %d.\n", sizeToCheck);
            fclose(file);
            return true;
        }
        else
        {
            cout << "Size = " << size << endl;

            printf("File does not exist or its size is not 8KB.\n");
        }
        fclose(file);
    }
    else
    {
        printf("Error opening the file.\n");
    }

    return 0;
}

void truncateFile(char *sourceFile)
{
    if (truncate(sourceFile, 0) == 0)
    {
        printf("Source file truncated.\n");
    }
    else
    {
        printf("Error truncating source file.\n");
    }
}

void writeSortedRecordToFile(RecordStructure rs)
{
    /*
    Check if DRAM_OUT has 8KB of data.
        If yes,
            check if SSD_OUT has 1 MB of data
            If yes,
                append the contents of SSD_OUT to HDD_OUT
                Truncate SSD_OUT data
            Append the contents of DRAM_OUT to SSD_OUT
            Truncate DRAM_OUT data
        Append this new record to DRAM_OUT
    */
    if (isFileFull("out/DRAM_OUT.txt", 8 * 1024))
    {
        if (isFileFull("out/SSD_OUT.txt", 1024 * 1024))
        {
            appendDataToDestination("out/SSD_OUT.txt", "out/HDD_OUT.txt");
            truncateFile("out/SSD_OUT.txt");
        }
        appendDataToDestination("out/DRAM_OUT.txt", "out/SSD_OUT.txt");
        truncateFile("out/DRAM_OUT.txt");
    }
    FILE *file = fopen("out/DRAM_OUT.txt", "w");
    fprintf(file, "%d,%d,%d,%d\n", rs.members[0], rs.members[1], rs.members[2], rs.members[3]);
    fclose(file);
}

void performTreeOfLosersSort(Cache c, vector<queue<RecordStructure>> in, TournamentTree tt, int k)
{
    TournamentTreeNode *tournamentTree = tt.getTree();
    while (tournamentTree[0].element.members[0] != INT_MAX)
    {
        printf("Value written to output file\t=\t%d\n", tournamentTree[0].element.members[0]);
        writeSortedRecordToFile(tournamentTree[0].element);
        readNextValueFromRun(c, in, k, tournamentTree[0].runId, tournamentTree);

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

void mergeFiles(Cache c, vector<queue<RecordStructure>> in, int k)
{

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
        if (in[i].empty() == false)
        {
            tournamentTree[run_index_in_tree].element = in[i].front();
            tournamentTree[run_index_in_tree].runId = i;
            in[i].pop();
        }
        else
        {
            // load data into this cache_array for run i
            in[i] = c.loadDataForRun(i);
        }
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

    performTreeOfLosersSort(c, in, tt, k);
}

void externalSort(Cache c, int num_ways)
{
    // Convert the cache.txt file to array and pass to below function
    vector<queue<RecordStructure>> cache_array;
    for (int i = 0; i < num_ways; i++)
    {
        // cache_array.push_back(queue<RecordStructure>());
        cache_array[i] = c.loadDataForRun(i);
    }

    /*
        cache_array[i] = vector<RecordStructure> -> stores records of the ith partition
        cache_array[i][j] -> stores an individual record
    */
    mergeFiles(c, cache_array, num_ways);
}

#include <queue>
#include <vector>
#include <unistd.h>
#include "Sort.h"
#include "RecordStructure.h"
#include "TournamentTree.h"
#include "Cache.h"
#include "DRAM.h"
#include "constants.h"
#include "defs.h"

using namespace std;

TournamentTree::TournamentTree() {}

TournamentTree::TournamentTree(TournamentTreeNode a[], int size, DRAM d, Cache c)
{
    this->num_nodes = size;
    this->tournamentTree = a;
    this->c = c;
    this->d = d;
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
        printf("%d\t%s\t%d\n", i, tournamentTree[i].element.members[0].c_str(), tournamentTree[i].runId);
    }
}

void TournamentTree::computeOffsetForLoser(int loserIndex, int winnerIndex)
{
    int offset = 0;
    fullComparison(loserIndex, winnerIndex, offset);

    tournamentTree[loserIndex].offsetValueCode = (FIELD_LENGTH - offset) * DOMAIN_OF_VALUES;
    if(offset < FIELD_LENGTH){
        tournamentTree[loserIndex].offsetValueCode += (int)tournamentTree[loserIndex].element.members[0][offset];
    }

    printf("\ncomputeOffsetForLoser:: OVC(%d) = %d, OVC(%d) = %d", loserIndex, tournamentTree[loserIndex].offsetValueCode, winnerIndex,  tournamentTree[winnerIndex].offsetValueCode);
}

void TournamentTree::computeOffsetForLoser(TournamentTreeNode &loser, TournamentTreeNode &winner)
{
    int offset = 0;
    fullComparison(loser, winner, offset);

    loser.offsetValueCode = (FIELD_LENGTH - offset) * DOMAIN_OF_VALUES;
    if(offset < FIELD_LENGTH){
        loser.offsetValueCode += (int)loser.element.members[0][offset];
    }
    TRACE(true);
    printf("\ncomputeOffsetForLoserReference:: OVC(loser) = %d, OVC(winner) = %d", loser.offsetValueCode, winner.offsetValueCode);
}

void TournamentTree::computeOffsetValueCode(int node_index)
{
    int offset = 0;
    while(offset < FIELD_LENGTH){
        if(tournamentTree[node_index].element.members[0][offset] != '0')
            break;
        offset++;
    }
    cout<<"Offset of node "<<node_index<<"\t = "<<offset<<endl;

    tournamentTree[node_index].offsetValueCode = (FIELD_LENGTH - offset) * DOMAIN_OF_VALUES;
    if(offset < FIELD_LENGTH) {
        tournamentTree[node_index].offsetValueCode += (int)tournamentTree[node_index].element.members[0][offset];
    }
    
    // cout<<"Part 2 char value = "<<tournamentTree[node_index].element.members[0][tournamentTree[node_index].offset]<<endl;
    // cout<<"Part 2 int value = "<<(int)tournamentTree[node_index].element.members[0][tournamentTree[node_index].offset]<<endl;
    
    cout<<"Offset Value code of node "<<node_index<<"\t = "<<tournamentTree[node_index].offsetValueCode<<endl;
    
}

void TournamentTree::compareNodeValues(int left, int right)
{
    int parent_index = getParentIndex(left);
    bool computeNewOffsetValueCode = false;
    if (isLeftLesserThanRight(left, right, computeNewOffsetValueCode))
    {
         
        if (computeNewOffsetValueCode)
            computeOffsetForLoser(right, left);
        printf("\nCompare Node Values:: OVC(left) = %d, OVC(right) = %d\n", tournamentTree[left].offsetValueCode, tournamentTree[right].offsetValueCode);
        tournamentTree[parent_index] = tournamentTree[left];
    }
    else
    {
        if (computeNewOffsetValueCode)
            computeOffsetForLoser(left, right);
        printf("\nCompare Node Values:: OVC(left) = %d, OVC(right) = %d\n", tournamentTree[left].offsetValueCode, tournamentTree[right].offsetValueCode);

        tournamentTree[parent_index] = tournamentTree[right];
    }
}

void TournamentTree::fullComparison(TournamentTreeNode &left, TournamentTreeNode &right, int& offset)
{
    while (offset < FIELD_LENGTH && left.element.members[0][offset] == right.element.members[0][offset])
    {
        offset++;
    }
}

void TournamentTree::fullComparison(int left, int right, int& offset)
{
    while (offset < FIELD_LENGTH && tournamentTree[left].element.members[0][offset] == tournamentTree[right].element.members[0][offset])
    {
        offset++;
    }
}

bool TournamentTree::isLeftLesserThanRight(TournamentTreeNode &left, TournamentTreeNode &right, bool &computeNewOffsetValueCode)
{
    // check if nodes have the offsets computed
    // case 1: Yes
    printf("\nisLeftLesserThanRight:: OVC(left) = %d, OVC(right) = %d\n", left.offsetValueCode, right.offsetValueCode);

    if (left.offsetValueCode != -1 && right.offsetValueCode != -1)
    {
        computeNewOffsetValueCode = false;
        if (left.offsetValueCode == right.offsetValueCode)
        {
            // do full comparison starting at offset
            int offset = 0;
            TRACE(true);
            fullComparison(left, right, offset);
            return offset < FIELD_LENGTH && ((int)left.element.members[0][offset] < (int)right.element.members[0][offset]);
        }
        else
        {
            TRACE(true);
            return left.offsetValueCode < right.offsetValueCode;
        }
        
    }
    else
    {
        TRACE(true);
        int offset = 0;
        fullComparison(left, right, offset);
        computeNewOffsetValueCode = true;
        return offset < FIELD_LENGTH && ((int)left.element.members[0][offset] < (int)right.element.members[0][offset]);
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
    printf("\nisLeftLesserThanRight:: OVC(%d) = %d, OVC(%d) = %d\n", left, tournamentTree[left].offsetValueCode, right, tournamentTree[right].offsetValueCode);

    if (tournamentTree[left].offsetValueCode != -1 && tournamentTree[right].offsetValueCode != -1)
    {
        if (tournamentTree[left].offsetValueCode == tournamentTree[right].offsetValueCode)
        {
            // do full comparison
            int offset = 0;
            fullComparison(left, right, offset);
            return offset < FIELD_LENGTH && ((int)tournamentTree[left].element.members[0][offset] < (int)tournamentTree[right].element.members[0][offset]);
        }
        else
        {
            return tournamentTree[left].offsetValueCode < tournamentTree[right].offsetValueCode;
        }
        computeNewOffsetValueCode = false;
    }
    else
    {
        int offset = 0;
        fullComparison(left, right, offset); // TODO: whats the point of this comparison
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
   return false;
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

void TournamentTree::updateTreeStructure(TournamentTreeNode &node, int parent_index, TournamentTreeNode &parent)
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
    
    printf("\nupdateTreeStructure:: OVC(larger) = %d, OVC(smaller) = %d", larger.offsetValueCode, smaller.offsetValueCode);
    
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

void TournamentTree::readNextValueFromRunUtil(vector< queue<RecordStructure> > &in, int k, int run_id)
{
    if (!in[run_id].empty())
    {
        tournamentTree[k + run_id].element = in[run_id].front();
        tournamentTree[k + run_id].runId = run_id;
        in[run_id].pop();
        cout<<"queue size of partition: "<< run_id<<" :"<<in[run_id].size()<<endl;
    }
    else
    {
        // load data into this cache_array for run i
        // check the DRAM offsets if data needs to be loaded from SSD
        if (c.read(run_id) == -1)
        {
            cout<<" Read data log 1\n";
            // Data read from DRAM is not successful
            // no records in DRAM
            int records_loaded_into_dram = d.read(run_id);
            cout<<" Read data log 1.5 \t records loaded into DRAM = "<<records_loaded_into_dram<<"\n";
            if (records_loaded_into_dram == -1)
            {
                cout<<" Read data log 2\n";
                // Partition - run_id in SSD is sorted!
                // TODO: SSD Logic - For final merge step
                // All the data in this run has been read.
                // Pass late fence - NULL vector/ empty vector for termination of the run
                RecordStructure lateFence = {"999999999999", "999999999", "999999999"}; // todo: change this late fence
                in[run_id].push(lateFence);
                return;
            }
            cout<<" Read data log 3\n";
            c.setRecordsInPartition(run_id, records_loaded_into_dram);
            c.read(run_id); // refilling the cache from DRAM since it failed earlier
        }
        in[run_id] = c.loadDataForRun(run_id);
        cout<<"loading new batch for run "<<run_id<<" queue size "<<in[run_id].size()<<endl;
        cout<<" Read data log 4\n";

        if(in[run_id].size() == 0){
            RecordStructure lateFence = {"999999999999", "999999999", "999999999"}; // todo: change this late fence
            in[run_id].push(lateFence);
            cout<<"Pushed late fence for run "<<run_id<<endl;
        }

    }
}

void TournamentTree::readNextValueFromRun(vector<queue<RecordStructure> > &in, int k, int run_id)
{
    // Note the runId needs to be set explicitly here
    tournamentTree[k + run_id].offsetValueCode = -1;
    tournamentTree[k + run_id].offset = 0;
    readNextValueFromRunUtil(in, k, run_id);
    printf("\nNew value read = %s ", tournamentTree[k + run_id].element.members[0].c_str());
    printf("Its offset = %d", tournamentTree[k + run_id].offsetValueCode);
}

void appendDataToDestination(string sourceFilePath, string destinationFilePath)
{
    TRACE(true);
    ifstream inputFile(sourceFilePath);
	ofstream outputFile(destinationFilePath, ios::app);
	if (inputFile.is_open() && outputFile.is_open())
	{
		outputFile << inputFile.rdbuf();
		inputFile.close();
		outputFile.close();
	}
	else
	{
		cout << "cannot open files to evict output data" << endl;
	}
}

bool isFileFull(string filePath, long sizeToCheck)
{
    TRACE(true);
    FILE *file = fopen(filePath.c_str(), "r");
    if (file)
    {
        fseek(file, 0, SEEK_END); // Move to the end of the file
        long size = ftell(file); // Get the current position, which is the size of the file

        if (size >= sizeToCheck)
        {
            printf("File size is exactly %llu.\n", sizeToCheck);
            fclose(file);
            return true;
        }
        printf("size is not 8KB.\n");
        fclose(file);
    }
    else
    {
        printf("Error opening the file.\n");
    }

    return false;
}

void truncateFile(string sourceFile)
{
    ofstream clearFile(sourceFile, ofstream::trunc);
    clearFile.close();
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
    if (isFileFull(DRAM_OUT_FILE_NAME, RoundDown(8 * 1024, recordsize)))
    {
        if (isFileFull(SSD_OUT_FILE_NAME, RoundDown(1024 * 1024, recordsize)))
        {
            appendDataToDestination(SSD_OUT_FILE_NAME, HDD_OUT_FILE_NAME);
            truncateFile(SSD_OUT_FILE_NAME.c_str());
        }
        appendDataToDestination(DRAM_OUT_FILE_NAME, SSD_OUT_FILE_NAME);
        truncateFile(DRAM_OUT_FILE_NAME);
    }
    FILE *file = fopen(DRAM_OUT_FILE_NAME.c_str(), "a");
    fprintf(file, "%s,%s,%s,%s\n", rs.members[0].c_str(), rs.members[1].c_str(), rs.members[2].c_str(), rs.members[3].c_str());
    
    cout<<"\nValue written to DRAM is "<<rs.members[0]<< endl;
    fclose(file);
}

void TournamentTree::performTreeOfLosersSort(vector<queue<RecordStructure>> &in, int k)
{
    // TournamentTreeNode *tournamentTree = getTree();
    while (tournamentTree[0].element.members[0] != "999999999999") // TODO: Handle the late fence case
    {
        printf("\nValue written to output file \t = \t %s \n", tournamentTree[0].element.members[0].c_str());
        writeSortedRecordToFile(tournamentTree[0].element);
        readNextValueFromRun(in, k, tournamentTree[0].runId);

        // printf("Printing Tree before UPDATING THE STRUCTURE\n");
        // tt.printTreeOfLosers();
        updateTreeStructure(
            tournamentTree[k + tournamentTree[0].runId],
            getParentIndex(k + tournamentTree[0].runId),
            tournamentTree[getParentIndex(k + tournamentTree[0].runId)]);
        // printf("Printing Tree after UPDATING THE STRUCTURE\n");
        // tt.printTreeOfLosers();
    }
}

void mergeFiles(DRAM d, Cache c, vector<queue<RecordStructure> > &cache_array, int k)
{
    TRACE(true);
    // Create a tournament tree with k nodes. Every leaf node
    // has first element of a run

    // k -> denotes the #runs
    // There are 2k nodes in the tree
    TournamentTreeNode *tournamentTree = new TournamentTreeNode[2 * k];
    int i;

    TournamentTree tt(tournamentTree, 2 * k, d, c);
    for (i = 0; i < k; i++)
    {
        tt.readNextValueFromRunUtil(cache_array, k, i);
    }
    printf("Printing Tree before comparing\n");
    tt.printTreeOfLosers();

    for (int i = k; i < 2 * k; i++)
    {
        tt.computeOffsetValueCode(i);
    }
    tt.compareInitialNodes();
    tt.setRootNode();
    tt.initialPop(1);
    // tt.printTreeOfLosers();

    tt.performTreeOfLosersSort(cache_array, k);
    // appendDataToDestination("out/SSD_OUT.txt", "out/HDD_OUT.txt");
    // truncateFile("out/SSD_OUT.txt");    
    appendDataToDestination("out/DRAM_OUT.txt", "out/SSD_OUT.txt");
    truncateFile("out/DRAM_OUT.txt");
    appendDataToDestination("out/SSD_OUT.txt", "out/HDD_OUT.txt");
    truncateFile("out/SSD_OUT.txt");
}

void externalSort(DRAM d, Cache c, int num_ways)
{
    // Convert the cache.txt file to array and pass to below function
    TRACE(true);
    vector<queue<RecordStructure> > cache_array;
    for (int i = 0; i < num_ways; i++)
    {
        // cache_array.push_back(queue<RecordStructure>());
        cache_array.push_back(c.loadDataForRun(i));
    }
    cout<<"Generated runs "<<endl;
    /*
        cache_array[i] = vector<RecordStructure> -> stores records of the ith partition
        cache_array[i][j] -> stores an individual record
    */
    mergeFiles(d, c, cache_array, num_ways);
}

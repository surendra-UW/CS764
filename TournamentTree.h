#pragma once

#include "Cache.h"
#include "DRAM.h"
#include "TournamentTreeNode.h"

using namespace std;

FILE *openFile(char *fileName, char *mode);


void mergeFiles(DRAM dram, Cache cache, vector<queue<RecordStructure>> &in, int k);

void externalSort(DRAM dram, Cache cache, int num_ways);
class TournamentTree
{
    TournamentTreeNode *tournamentTree;
    int num_nodes;
    Cache cache;
    DRAM dram;
    int full_string_comparisons_avoided = 0;

public:
    int left(int i);
    int right(int i);
    int getNodeCount();
    bool isLeafNode(int index);
    int getParentIndex(int i);
    void printTreeOfLosers();
    void computeOffsetForLoser(int loserIndex, int winnerIndex);
    void computeOffsetForLoser(TournamentTreeNode &loser, TournamentTreeNode &winner);
    void computeOffsetValueCode(int node_index);
    void compareNodeValues(int left, int right);
    void fullComparison(TournamentTreeNode &left, TournamentTreeNode &right, int& offset);
    void fullComparison(int left, int right, int& offset);
    bool isLeftLesserThanRight(TournamentTreeNode &left, TournamentTreeNode &right, bool &computeNewOffsetValueCode);
    bool isLeftLesserThanRight(int left, int right, bool &computeNewOffsetValueCode);
    void compareInitialNodes();
    void setRootNode();
    bool areNodesEqual(int a, int b);
    void initialPop(int index);
    void updateTreeStructure(TournamentTreeNode &node, int parent_index, TournamentTreeNode &parent);
    TournamentTreeNode *getTree();
    // TournamentTreeNode getMin();
    TournamentTree(TournamentTreeNode a[], int size, DRAM dram, Cache cache);
    void performTreeOfLosersSort(vector< queue<RecordStructure> > &in, int k);
    void readNextValueFromRun(vector< queue<RecordStructure> > &in, int k, int run_id);
    void readNextValueFromRunUtil(vector< queue<RecordStructure> > &in, int k, int run_id);
    TournamentTree();
    void computeOffsetUtil(int nodeIndex, int offset);
};

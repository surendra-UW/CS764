#pragma once
#include "TournamentTreeNode.h"
// A class for Min Heap
void externalSort(char *input_file, char *output_file, int num_ways, int run_size);

class TournamentTree
{

    // pointer to array of elements in heap
    TournamentTreeNode *tournamentTree;
    int num_nodes;

public:
    // Constructor: creates a min heap of given size
    TournamentTree(TournamentTreeNode a[], int size);

    TournamentTreeNode *getTree()
    {
        return this->tournamentTree;
    }

    // to get index of left child of node at index i
    int left(int i) { return (2 * i); }

    // to get index of right child of node at index i
    int right(int i) { return (2 * i + 1); }

    int getParentIndex(int i)
    {
        return floor(i / 2);
    }

    int getNodeCount()
    {
        return this->num_nodes;
    }

    bool isLeafNode(int index)
    {
        return left(index) >= num_nodes || right(index) >= num_nodes;
    }

    // to get the root
    TournamentTreeNode getMin() { return tournamentTree[0]; }

    void printTreeOfLosers()
    {
        for (int i = 0; i < this->num_nodes; i++)
        {
            printf("%d\t%d\t%d\n", i, tournamentTree[i].element.members[0], tournamentTree[i].runId);
        }
    }

    void computeOffsetForLoser(int loserIndex, int winnerIndex)
    {
        // TODO: INCOMPLETE
        fullComparison(loserIndex, winnerIndex, tournamentTree[loserIndex].offset);

        tournamentTree[loserIndex].offsetValueCode = (5 - tournamentTree[loserIndex].offset) * 100 + tournamentTree[loserIndex].element.members[tournamentTree[loserIndex].offset];

        printf("\ncomputeOffsetForLoser:: OVC(%d) = %d, OVC(%d) = %d", tournamentTree[loserIndex].offsetValueCode, loserIndex, tournamentTree[winnerIndex].offsetValueCode, winnerIndex);
    }

    void computeOffsetForLoser(TournamentTreeNode loser, TournamentTreeNode winner)
    {
        fullComparison(loser, winner, loser.offset);

        loser.offsetValueCode = (5 - loser.offset) * 100 + loser.element.members[loser.offset];

        printf("\ncomputeOffsetForLoser:: OVC(%d) = %d, OVC(%d) = %d", loser.offsetValueCode, winner.offsetValueCode);
    }

    void computeOffsetValueCode(int node_index)
    {
        tournamentTree[node_index].offsetValueCode = (5 - tournamentTree[node_index].offset) * 100 + tournamentTree[node_index].element.members[tournamentTree[node_index].offset];
    }

    void compareNodeValues(int left, int right)
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

    void fullComparison(TournamentTreeNode left, TournamentTreeNode right, int offset)
    {
        while (left.offset < 5)
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

    void fullComparison(int left, int right, int offset)
    {
        while (tournamentTree[left].offset < 5)
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

    bool isLeftLesserThanRight(TournamentTreeNode left, TournamentTreeNode right, bool &computeNewOffsetValueCode)
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
                return left.offset < 5 && (left.element.members[offset] < right.element.members[offset]);
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
            return left.offset < 5 && (left.element.members[left.offset] < right.element.members[left.offset]);
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

    bool isLeftLesserThanRight(int left, int right, bool &computeNewOffsetValueCode)
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
                return tournamentTree[left].offset < 5 && (tournamentTree[left].element.members[offset] < tournamentTree[right].element.members[offset]);
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

    void compareInitialNodes()
    {
        for (int i = this->num_nodes - 1; i >= 1; i -= 2)
        {
            compareNodeValues(i, i - 1);
        }
    }

    void setRootNode()
    {
        tournamentTree[0] = tournamentTree[1];
    }

    bool areNodesEqual(int a, int b)
    {
        return (tournamentTree[a].runId == tournamentTree[b].runId && tournamentTree[a].element.members[0] == tournamentTree[b].element.members[0]);
    }

    void initialPop(int index)
    {
        if (isLeafNode(index))
            return;
        tournamentTree[index] = (areNodesEqual(index, left(index))) ? tournamentTree[right(index)] : tournamentTree[left(index)];
        initialPop(left(index));
        initialPop(right(index));
    }

    void updateTreeStructure(TournamentTreeNode node, int parent_index, TournamentTreeNode parent)
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
};

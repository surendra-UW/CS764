#include "RecordStructure.h"

struct TournamentTreeNode
{
    RecordStructure element;
    int runId;
    long long int offsetValueCode = -1;
    // OVC = this.Offset*Domain + (Domain - this.ValueAtOffset)
    int offset = 0;
    // offset = -1 ==> No Offsets computed yet
    int valueAtOffset;
    // ValueAtOffset = element.members[this.offset]
};
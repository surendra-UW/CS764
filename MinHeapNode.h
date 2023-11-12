#pragma once
#include "RecordStructure.h"
struct MinHeapNode
{
    RecordStructure element;
    int i;

    bool operator<(const MinHeapNode &loc2) const
    {
        return this->element.member1 > loc2.element.member1;
        // Return true if this is less than loc2
    }
};
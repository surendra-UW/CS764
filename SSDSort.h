#include "Iterator.h"
typedef uint32_t uint;

class SSDSortPlan : public Plan
{
	friend class SSDSortIterator;

public:
	SSDSortPlan(RowCount produced, RowCount consumed);
	~SSDSortPlan();
	Iterator *init() const;

private:
	RowCount const _produced;
    RowCount const _consumed;
}; // class SSDSortPlan

class SSDSortIterator : public Iterator
{
public:
	SSDSortIterator(SSDSortPlan const *const plan);
	~SSDSortIterator();
	bool next();
    bool copyRamToHDD();
	bool internalSort();
	int externalMerge();

private:
	SSDSortPlan const *const _plan;
	RowCount _consumed, _produced;
    uint batches;
}; // class SortIterator

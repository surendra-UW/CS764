#pragma once
#include "Iterator.h"
typedef uint32_t uint;

class SortPlan : public Plan
{
	friend class SortIterator;

public:
	SortPlan(Plan *const input);
	~SortPlan();
	Iterator *init() const;

private:
	Plan *const _input;
}; // class SortPlan

class SortIterator : public Iterator
{
public:
	SortIterator(SortPlan const *const plan);
	~SortIterator();
	bool next();
	bool internalSort();
	bool copyRamToHDD();
	void clearRam();
	int externalMerge();
	void mergeCacheData();
	void initRamMem(uint blockSize, int step);
	void initCacheMem(uint blockSize, int step);
	bool loadDataBlocks(int partition, int ramOffset, int hddOffset, uint blockSize, int step, std::string writeToHierarchy, std::string readFromHierarchy, int writeToHierarchyBytes, int readFromHierarchyPageSize);

private:
	SortPlan const *const _plan;
	Iterator *const _input;
	RowCount _consumed, _produced;
	int _recsize;
}; // class SortIterator

#include <iostream>
#include <random>
#include <vector>
#include <climits>
#include <algorithm>
#include <functional>
#include "Iterator.h"

class ScanPlan : public Plan
{
	friend class ScanIterator;
public:
	ScanPlan (RowCount const count);
	~ScanPlan ();
	Iterator * init () const;
private:
	RowCount const _count;
}; // class ScanPlan

class ScanIterator : public Iterator
{
public:
	ScanIterator (ScanPlan const * const plan);
	~ScanIterator ();
	bool next ();
	//std::string generateRandomString(size_t length, random_bytes_engine& rbe);
private:
	ScanPlan const * const _plan;
	RowCount _count;
}; // class ScanIterator

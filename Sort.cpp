
#include "defs.h"
#include "Sort.h"
#include "unistd.h"
#include "constants.h"
#include "internal_sort.h"
#include "TournamentTree.h"
#include "Iterator.h"
#include "SSDSort.h"

using namespace std;

#define HDD_PAGE_SIZE 1024

int recordsize = 0;
SortPlan::SortPlan(Plan *const input) : _input(input)
{
	TRACE(true);
} // SortPlan::SortPlan

SortPlan::~SortPlan()
{
	TRACE(true);
	delete _input;
} // SortPlan::~SortPlan

Iterator *SortPlan::init() const
{
	TRACE(true);
	return new SortIterator(this);
} // SortPlan::init

SortIterator::SortIterator(SortPlan const *const plan) : _plan(plan), _input(plan->_input->init()),
														 _consumed(0), _produced(0), _recsize(1004)
{
	TRACE(true);

	// TODO: hard coding and commenting records generation
	// while (_input->next())
	// 	++_consumed;
	_consumed = 15000000;
	delete _input;

	// ifstream inputFile("HDD.txt", ios::binary | ios::ate);
	// if (!inputFile)
	// {
	// 	cout << "cannot open the hard disk" << endl;
	// 	exit(1);
	// }
	// streampos curr = inputFile.tellg();
	// _recsize = curr/_consumed;
	// inputFile.close();
	recordsize = _recsize;
	cout<<"record size: "<<recordsize<<endl;
	// initialize
	runs = 0;

	traceprintf("consumed %lu rows\n",
				(unsigned long)(_consumed));
} // SortIterator::SortIterator

SortIterator::~SortIterator()
{
	TRACE(true);

	traceprintf("produced %lu of %lu rows\n",
				(unsigned long)(_produced),
				(unsigned long)(_consumed));
} // SortIterator::~SortIterator

// external sort
bool SortIterator::next()
{
	TRACE(true);
	if (_produced >= _consumed)
		return false;

	ssdSort();
	externalMerge();
	
	return true;
} // SortIterator::next

void SortIterator:: ssdSort() {
	TRACE(true);
	Plan const *plan = new SSDSortPlan(_produced, _consumed);
	Iterator *ssd_sort_iterator = plan->init();
	ssd_sort_iterator->run();
	runs++;
}

void SortIterator:: externalMerge() {
	TRACE(true);

	//load SSD all partitions 
	SSD ssd_merge(runs);
	for (int i = 0; i < runs; i++)
	{
		ssd_merge.read(i);
	}

	//load dram all partitions
	DRAM dram_merge(runs, true);
	for (int i = 0; i < runs; i++)
	{
		dram_merge.read(i);
	}

	//load cache all partitions
	Cache cache_merge(runs);
	for (int i = 0; i < runs; i++)
	{
		cache_merge.read(i);
	}

	externalSort(&ssd_merge, &dram_merge, cache_merge, runs);

	ssd_merge.clearSSD();
	cache_merge.clearCache();
    dram_merge.clearRam();
}

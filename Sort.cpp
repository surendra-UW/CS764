#include <cstdio>
#include "defs.h"
#include "Sort.h"
#include "unistd.h"
#include "constants.h"
#include "internal_sort.h"
#include "TournamentTree.h"
#include "Iterator.h"
#include "SSDSort.h"

using namespace std;


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
														 _consumed(0), _produced(0), _recsize(54)
{
	TRACE(true);
	traceprintf("Starting to scan records.\n");

	while (_input->next())
		++_consumed;
	// _consumed = 3000000;
	delete _input;
	
	ifstream inputFile(HDD_FILE_NAME, ios::binary | ios::ate);
	if (!inputFile)
	{
		// cout << "cannot open the hard disk" << endl;
		exit(1);
	}
	streampos curr = inputFile.tellg();
	_recsize = curr/_consumed;
	inputFile.close();
	recordsize = _recsize;
	
	traceprintf("record size after delimiters %d\n", recordsize);
	// initialize
	runs = 0;

	traceprintf("consumed %lu rows\n",
				(unsigned long)(_consumed));
} // SortIterator::SortIterator

SortIterator::~SortIterator()
{
	TRACE(true);

	// traceprintf("produced %lu of %lu rows\n",
	// 			(unsigned long)(_consumed),
	// 			(unsigned long)(_consumed));
} // SortIterator::~SortIterator

// external sort
bool SortIterator::next()
{

	// if (_produced >= _consumed)
	// 	return false;

	TRACE(true);
	ssdSort();

	if(runs>1) externalMerge();
	
	return false;
} // SortIterator::next

void SortIterator:: ssdSort() {
	TRACE(true);
	Plan const *ssd_plan = new SSDSortPlan(_produced, _consumed);
	Iterator *ssd_sort_iterator = ssd_plan->init();
	while (ssd_sort_iterator->next()) runs++;
}

void SortIterator:: clearTempFile() {
	ofstream tempFile(HDD_OUT_FILE_NAME, ios:: trunc);
	tempFile.close();

	//rename HDD_OUT2 to HDD_OUT
	if (rename(HDD_OUT2_FILE_NAME.c_str() , HDD_OUT_FILE_NAME.c_str()) == 0) {
        cout << "File renamed successfully." << endl;
    } else {
        perror("Error renaming file");
    }
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
		// TRACE(true);
	    traceprintf("DRAM is loaded with records for external merge.\n");

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
	
	clearTempFile();
}

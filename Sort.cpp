
#include "defs.h"
#include "Sort.h"
#include "unistd.h"
#include "constants.h"
#include "internal_sort.h"
#include "TournamentTree.h"
#include "DRAM.h"
#include "SSD.h"
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
														 _consumed(0), _produced(0), _recsize(52)
{
	TRACE(true);

	// TODO: hard coding and commenting records generation
	// while (_input->next())
	// 	++_consumed;
	_consumed = 13258810;
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
	
	Plan const *plan = new SSDSortPlan(_produced, _consumed);
	Iterator *ssd_sort_iterator = plan->init();
	ssd_sort_iterator->run();
	runs++;
	return true;
} // SortIterator::next

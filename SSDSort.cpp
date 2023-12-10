
#include "defs.h"
#include "SSDSort.h"
#include "unistd.h"
#include "constants.h"
#include "internal_sort.h"
#include "TournamentTree.h"

using namespace std;

#define HDD_PAGE_SIZE 1024

SSDSortPlan::SSDSortPlan(RowCount produced, RowCount consumed) : _produced(produced), _consumed(consumed)
{
	TRACE(true);
} // SortPlan::SortPlan

SSDSortPlan::~SSDSortPlan()
{
	TRACE(true);
} // SortPlan::~SortPlan

Iterator *SSDSortPlan::init() const
{
	TRACE(true);
	return new SSDSortIterator(this);
} // SortPlan::init

SSDSortIterator::SSDSortIterator(SSDSortPlan const *const plan) : _plan(plan), _produced(plan->_produced),_consumed(plan->_consumed), batches(0)
{
	TRACE(true);

} // SortIterator::SortIterator

SSDSortIterator::~SSDSortIterator()
{
	TRACE(true);

	traceprintf("produced %lu of %lu rows\n",
				(unsigned long)(_produced),
				(unsigned long)(_consumed));
} // SortIterator::~SortIterator

// external sort
bool SSDSortIterator::next()
{
	TRACE(true);
	if (_produced >= _plan->_consumed)
		return false;
	internalSort();
	externalMerge();
	return true;
} // SortIterator::next

uint getMaxSSDBatches()
{
	uint partitions = (SSD_SIZE_IN_BYTES) / (DRAM_SIZE_IN_BYTES);
	return (uint)(0.99 * partitions);
}

bool SSDSortIterator::internalSort()
{
	DRAM dram;
	uint max_batches = getMaxSSDBatches();

	cout<<"_produced "<<_produced<<"_consumed "<<_consumed<<"batches "<<batches<<" max_batches "<<max_batches<<endl;
	while ((_produced < _plan->_consumed) && (batches < max_batches))
	{
		std::vector<RecordStructure> arr;
		uint dramRecords = divide(RoundDown(DRAM_SIZE_IN_BYTES, recordsize), (size_t)recordsize);
		uint recordsToConsume = (_consumed - _produced) > dramRecords ? dramRecords : (_consumed - _produced);

		cout << "dram records " << dramRecords << "rec to consusc " << recordsToConsume << endl;
		dram.loadFromHDD(recordsToConsume);
		cout << "loading ram with rec " << recordsToConsume << endl;

		arr = read_ramfile();
		quickSort(arr, customComparator);
		write_ramfile(arr);
		arr.clear(); // Remove all elements

		_produced = _produced + recordsToConsume;
		if (!copyRamToHDD())
			exit(1);
		dram.clearRam();

		batches++;
		cout << "current bacth " << batches << endl;
	}
}

bool SSDSortIterator::copyRamToHDD()
{
	ifstream inputFile(DRAM_FILE_NAME);
	ofstream outputHDDFile(SSD_FILE_NAME, ios::app);
	if (inputFile.is_open() && outputHDDFile.is_open())
	{
		outputHDDFile << inputFile.rdbuf();
		inputFile.close();
		outputHDDFile.close();
		// clearRam();
	}
	else
	{
		cout << "cannot open files to evict ram" << endl;
		return false;
	}
	return true;
}

int SSDSortIterator::externalMerge()
{
	TRACE(true);

	SSD *ssd = NULL;
	//load dram all partitions
	DRAM dram_merge(batches);
	for (int i = 0; i < batches; i++)
	{
		dram_merge.read(i);
	}

	//load cache all partitions
	Cache cache_merge(batches);
	for (int i = 0; i < batches; i++)
	{
		cache_merge.read(i);
	}

	externalSort(ssd, &dram_merge, cache_merge, batches);

	cache_merge.clearCache();
    dram_merge.clearRam();
}

#include "defs.h"
#include "Sort.h"

#include "constants.h"
// #include "Cache.h"

#include "internal_sort.h"
#include "TournamentTree.h"
#include "DRAM.h"
#include "SSD.h"
using namespace std;

#define HDD_PAGE_SIZE 1024
#define NWAY_MERGE 8

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
														 _consumed(0), _produced(0), _recsize(1000)
{
	TRACE(true);

	// TODO: hard coding and commenting records generation
	// while (_input->next())
	// 	++_consumed;
	_consumed = 10000;//13258810;
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
	batches = 0;
	internalSort();
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
	// if (_produced >= _consumed)
	// 	return false;

	externalMerge();

	return false;
} // SortIterator::next

uint getMaxSSDBatches()
{
	uint64_t partitions = (SSD_SIZE_IN_BYTES) / (DRAM_SIZE_IN_BYTES);
	return (uint)(0.99 * partitions);
}

bool SortIterator::internalSort()
{
	DRAM dram;
	uint max_batches = getMaxSSDBatches();
	cout << "max_batches " << max_batches << endl;
	cout << "total consmed " << _consumed << endl;
	while ((_produced < _consumed) && (batches < max_batches))
	{
		std::vector<RecordStructure> arr;
		uint dramRecords = divide(RoundDown(DRAM_SIZE_IN_BYTES, _recsize), (size_t)_recsize);
		uint recordsToConsume = (_consumed - _produced) > dramRecords ? dramRecords : (_consumed - _produced);
		cout << "dram records " << dramRecords << "rec to consusc " << recordsToConsume << endl;
		dram.loadFromHDD(recordsToConsume);
		cout << "loading ram with rec " << recordsToConsume << endl;
		arr = read_ramfile("DRAM.txt");
		quickSort(arr, customComparator);
		write_ramfile("DRAM.txt", arr);
		arr.clear(); // Remove all elements
		arr.shrink_to_fit();
		_produced = _produced + recordsToConsume;
		if (!copyRamToHDD())
			exit(1);
		dram.clearRam();
		batches++;
		cout << "current bacth " << batches << endl;
	}
}

bool SortIterator::copyRamToHDD()
{
	ifstream inputFile("DRAM.txt");
	ofstream outputHDDFile("SSD.txt", ios::app);
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

int SortIterator::getBatches()
{
	return batches;
}

int SortIterator::getRecordSize()
{
	return _recsize;
}

int SortIterator::externalMerge()
{
	TRACE(true);
	// uint64_t dram_partition_size = DRAM_SIZE_IN_BYTES / batches;
	// uint64_t rounded_dram_block = RoundDown(dram_partition_size, _recsize);
	// uint32_t cache_partition_size = CACHE_SIZE_IN_BYTES / batches;
	// uint32_t rounded_cache_block = RoundDown(cache_partition_size, _recsize);

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
	externalSort(dram_merge, cache_merge, batches);
}
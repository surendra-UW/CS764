
#include "defs.h"
#include "Sort.h"

#include "constants.h"
// #include "Cache.h"
#include "DRAM.h"
#include "SSD.h"
#include "internal_sort.h"
#include "TournamentTree.h"

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
														 _consumed(0), _produced(0), _recsize(52)
{
	TRACE(true);

	// TODO: hard coding and commenting records generation
	while (_input->next())
		++_consumed;
	// _consumed = 22690;
	delete _input;

	ifstream inputFile("HDD.txt", ios::binary | ios::ate);
	if (!inputFile)
	{
		cout << "cannot open the hard disk" << endl;
		exit(1);
	}
	streampos curr = inputFile.tellg();
	_recsize = curr / _consumed;
	inputFile.close();
	recordsize = _recsize;
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
	if (_produced >= _consumed)
		return false;

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
		arr.resize(0);
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

	uint64_t dram_partition_size = DRAM_SIZE_IN_BYTES / batches;
	uint64_t rounded_dram_block = RoundDown(dram_partition_size, _recsize);
	uint32_t cache_partition_size = CACHE_SIZE_IN_BYTES / batches;
	uint32_t rounded_cache_block = RoundDown(cache_partition_size, _recsize);

	DRAM dram_merge(batches);
	Cache cache_merge(batches);
	cache_merge.rounded_cache_block = rounded_cache_block;
	for (int i = 0; i < batches; i++)
	{
		dram_merge.read(i, rounded_dram_block);
		cache_merge.read(i, rounded_cache_block);
	}

	externalSort(cache_merge, "CACHE.txt", "CACHE_SORTED_TEST.txt", NWAY_MERGE);
}

// uint SortIterator::blockLeftToMerge(Cache cache, DRAM dram, int partition) {
// 	streamoff offset = cache.getReadOffset(partition);

// }
/*

void SortIterator::initRamMem(uint blockSize, int step) {
	for(int i=0;i<NWAY_MERGE;i++){
		loadRamBlocks(i, 0, 0, blockSize, step);
	}
}

bool SortIterator::loadRamBlocks(int partition, int ramOffset, int hddOffset, uint blockSize, int step)
{
	std::cout << "Hello, World!" << std::endl;
	//if hddOffset reaches limit return false
	fstream ram("DRAM.txt", ios::in|ios::out);
	ifstream inputFile("HDD2.txt");
	if(!ram.is_open() || !inputFile.is_open()) {
		cout<<"failed to load data from Hdd"<<endl;
		TRACE(true);
		exit(1);
	}
	//size of each buffer in bytes in ram
	streamoff ramPartitionSizeInBytes = RoundDown(DRAM_SIZE_IN_BYTES / (NWAY_MERGE + 1), _recsize);
	//size of each buffer in hdd for first run
	streamoff hddPartitionSizeInBytes = RoundDown(DRAM_SIZE_IN_BYTES, _recsize)*step*NWAY_MERGE;
	if (partition > 0 || ramOffset > 0)
	{
		ram.seekp(partition * ramPartitionSizeInBytes + ramOffset, ios::beg);
	}
	if (partition > 0 || hddOffset > 0)
	{
		streamoff hddPos = hddPartitionSizeInBytes + partition*RoundDown(DRAM_SIZE_IN_BYTES, _recsize);
		inputFile.seekg(hddPos + hddOffset, ios::beg);
	}

	blockSize = RoundDown(blockSize, _recsize);
	int read_size = RoundDown(HDD_PAGE_SIZE, _recsize);
	while (blockSize > 0)
	{
		int read_block = blockSize >= read_size ? read_size : blockSize;
		//1 char for the end delimiter for a char array i.e, NULL
		char data[read_block+1];
		inputFile.read(data, read_block);
		ram.write(data, read_block);
		blockSize = blockSize - read_block;
	}

	ram.close();
	inputFile.close();
	return true;
}
*/
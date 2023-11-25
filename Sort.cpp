#include <iostream>
#include <fstream>
#include <string>
#include "Sort.h"
#include "TournamentTree.h"
using namespace std;
#define CACHE_BYTES 512 * 1024
#define DRAM_BYTES 10 * 1024
#define HDD_PAGE_SIZE 1024
#define NWAY_MERGE 8

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

SortIterator::SortIterator(SortPlan const *const plan) : _plan(plan), _input(plan->_input->init()), _consumed(0), _produced(0), _recsize(0)
{
	TRACE(true);
	while (_input->next())
		++_consumed;
	delete _input;

	ifstream inputFile("HDD.txt", ios::binary | ios::end);
	if (!inputFile)
	{
		cout << "cannot open the hard disk" << endl;
		exit(1);
	}
	streampos curr = inputFile.tellg();
	_recsize = curr / _consumed;
	inputFile.close();
	// internalSort();
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
	ifstream inputFile("DRAM.txt", ios::end);
	if (!inputFile)
	{
		cout << "cannot open the hard disk" << endl;
		exit(1);
	}

	char data[160];
	inputFile.read(data, 160);

	cout << "data reqad is " << data << endl;

	inputFile.close();
	return false;
} // SortIterator::next

bool SortIterator::internalSort()
{
	ifstream inputFile("HDD.txt", ios::beg);
	if (!inputFile)
	{
		cout << "cannot open the hard disk" << endl;
		return 1;
	}
	int run = 0;
	while (_produced < _consumed)
	{
		ofstream outputFile("DRAM.txt", ios::app);
		uint64_t block_left = (_consumed - _produced) * _recsize;
		if (block_left > DRAM_BYTES)
		{
			block_left = DRAM_BYTES;
		}
		// should we load 1Mb at a time ???
		block_left = RoundDown(block_left, _recsize);
		int read_size = RoundDown(HDD_PAGE_SIZE, _recsize);
		while (block_left > 0)
		{
			char data[read_size];
			int read_block = block_left / read_size > 0 ? read_size : block_left;
			inputFile.read(data, read_block);
			_produced = _produced + read_block / _recsize;
			outputFile.write(data, read_block);
			block_left = block_left - read_block;
		}
		outputFile.close();
		// TODO: internal sort usig ram data
		if (!copyRamToHDD())
			exit(1);
	}
	_produced = 0;
	inputFile.close();
}

bool SortIterator::copyRamToHDD()
{
	ifstream inputFile("DRAM.txt");
	ofstream outputHDDFile("HDD2.txt", ios::app);
	if (inputFile.is_open() && outputHDDFile.is_open())
	{
		outputHDDFile << inputFile.rdbuf();
		inputFile.close();
		outputHDDFile.close();
		clearRam();
	}
	else
	{
		cout << "cannot open files to evict ram" << endl;
		return false;
	}
	return true;
}
void SortIterator::clearRam()
{
	ofstream clearRAM("DRAM.txt", ofstream::trunc);
	clearRAM.close();
}

void SortIterator::mergeCacheData()
{
	int recordsInEachBatch = divide(CACHE_BYTES, NWAY_MERGE + 1);
	int totalSteps = divide(recordsInEachBatch, CACHE_BYTES) / NWAY_MERGE;
	// totalSteps -> total number of records in a run
	uint cacheOffsets[NWAY_MERGE + 1] = {0};
	uint ramOffsets[NWAY_MERGE + 1] = {0};
	int cachePartitionSizeInBytes = RoundDown(CACHE_BYTES / (NWAY_MERGE + 1), _recsize);
	int ramPartitionSizeInBytes = RoundDown(DRAM_BYTES, _recsize);
	int buffer_size = divide(cachePartitionSizeInBytes, _recsize);
	for (int step = 0; step < totalSteps; step++)
	{
		initCacheMem(cachePartitionSizeInBytes, 1);
	}

	externalSort("CACHE.txt", "CACHE_SORTED_TEST.txt", NWAY_MERGE, buffer_size);
}

int SortIterator::externalMerge()
{
	//  each way merge should have DRAM size records
	RowCount recordsInEachBatch = NWAY_MERGE * divide(DRAM_BYTES, _recsize);
	cout << "rec " << recordsInEachBatch << endl;
	cout << "Record size = " << _recsize << endl;
	int totalSteps = divide(_consumed, recordsInEachBatch);
	// totalSteps -> total number of records in a run
	uint ramOffsets[NWAY_MERGE + 1] = {0};
	uint hddOffsets[NWAY_MERGE] = {0};
	int ramPartitionSizeInBytes = RoundDown(DRAM_BYTES / (NWAY_MERGE + 1), _recsize);
	int hddPartitionSizeInBytes = RoundDown(HDD_PAGE_SIZE, _recsize);
	// step -> one record in a run
	for (int step = 0; step < totalSteps; step++)
	{
		initRamMem(ramPartitionSizeInBytes, step + 1);
		cout << "Step " << step << " of " << totalSteps << endl;
		// break;
		/*
		load data from ram to cache and sort
		when any of the cache buffer is empty load from ram
		*/
	}
	mergeCacheData();
}

void SortIterator::initCacheMem(uint blockSize, int step)
{
	for (int i = 0; i < NWAY_MERGE; i++)
	{
		loadDataBlocks(i, 0, 0, blockSize, step, "CACHE.txt", "DRAM.txt", CACHE_BYTES, DRAM_BYTES);
	}
}

void SortIterator::initRamMem(uint blockSize, int step)
{
	for (int i = 0; i < NWAY_MERGE; i++)
	{
		loadDataBlocks(i, 0, 0, blockSize, step, "DRAM.txt", "HDD.txt", DRAM_BYTES, HDD_PAGE_SIZE);
	}
}

bool SortIterator::loadDataBlocks(int partition, int writeToHierarchyOffset, int readFromHierarchyOffset, uint blockSize, int step, string writeToHierarchy, string readFromHierarchy, int writeToHierarchyBytes, int readFromHierarchyPageSize)
{
	// if readFromLevelOffset reaches limit return false
	fstream writeToHierarchyFile(writeToHierarchy, ios::in | ios::out);
	ifstream readFromHierarchyFile(readFromHierarchy, ios::beg);
	if (!writeToHierarchyFile.is_open() || !readFromHierarchyFile.is_open())
	{
		cout << "failed to load data from " << readFromHierarchy << endl;
		TRACE(true);
		exit(1);
	}
	streamoff writeToHierarchyPartitionSizeInBytes = RoundDown(writeToHierarchyBytes / (NWAY_MERGE + 1), _recsize);
	streamoff readFromHierarchyPartitionSizeInBytes = RoundDown(writeToHierarchyBytes, _recsize) * step * NWAY_MERGE;
	if (partition > 0 || writeToHierarchyOffset > 0)
	{
		writeToHierarchyFile.seekp(partition * writeToHierarchyPartitionSizeInBytes + writeToHierarchyOffset, ios::beg);
	}
	if (partition > 0 || readFromHierarchyOffset > 0)
	{
		// readFromHierarchyFile.seekg(partition * readFromHierarchyPartitionSizeInBytes + readFromHierarchyOffset, ios::beg);

		streamoff hddPos = readFromHierarchyPartitionSizeInBytes + partition * RoundDown(DRAM_BYTES, _recsize);
		readFromHierarchyFile.seekg(hddPos + readFromHierarchyOffset, ios::beg);
	}

	blockSize = RoundDown(blockSize, _recsize);
	int read_size = RoundDown(readFromHierarchyPageSize, _recsize);
	while (blockSize > 0)
	{
		int read_block = blockSize >= read_size ? read_size : blockSize;
		char data[read_block + 1];
		readFromHierarchyFile.read(data, read_block);
		// writeToHierarchyFile.write(data, read_block);
		// Convert char array to a string
		// std::string data_str(data, read_block);
		// cout << "Printing data\n";
		// cout << data_str << endl;
		writeToHierarchyFile.write(data, read_block);
		// break;
		// Write the string to the output file

		blockSize = blockSize - read_block;
	}

	writeToHierarchyFile.close();
	readFromHierarchyFile.close();
	return true;
}
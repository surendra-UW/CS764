#include <iostream>
#include <fstream>
#include <string>
#include "Sort.h"
#include <stdlib.h>
using namespace std;
#define DRAM_BYTES 10*1024
#define HDD_PAGE_SIZE 1024
#define NWAY_MERGE 8

SortPlan::SortPlan (Plan * const input) : _input (input)
{
	TRACE (true);
} // SortPlan::SortPlan

SortPlan::~SortPlan ()
{
	TRACE (true);
	delete _input;
} // SortPlan::~SortPlan

Iterator * SortPlan::init () const
{
	TRACE (true);
	return new SortIterator (this);
} // SortPlan::init

SortIterator::SortIterator (SortPlan const * const plan) :
	_plan (plan), _input (plan->_input->init ()),
	_consumed (0), _produced (0), _recsize(0)
{
	TRACE (true);
	while (_input->next ())  ++ _consumed;
	delete _input;

	ifstream inputFile("HDD.txt", ios::binary|ios::ate);
	if(!inputFile) {
		cout<< "cannot open the hard disk"<<endl; 
		exit(1);
	}
	streampos curr = inputFile.tellg();
	_recsize = curr/_consumed;
	inputFile.close();
	internalSort();
	traceprintf ("consumed %lu rows\n",
			(unsigned long) (_consumed));
} // SortIterator::SortIterator

SortIterator::~SortIterator ()
{
	TRACE (true);

	traceprintf ("produced %lu of %lu rows\n",
			(unsigned long) (_produced),
			(unsigned long) (_consumed));
} // SortIterator::~SortIterator

//external sort
bool SortIterator::next ()
{
	TRACE (true);
	if (_produced >= _consumed)  return false;
	
	externalMerge();

	return false;
} // SortIterator::next

bool SortIterator:: internalSort() {
	ifstream inputFile("HDD.txt");
	if(!inputFile) {
		cout<< "cannot open the hard disk"<<endl;
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
		//should we load 1Mb at a time ???
		block_left = RoundDown(block_left, _recsize);
		int read_size = RoundDown(HDD_PAGE_SIZE, _recsize);
		while (block_left > 0)
		{
			char data[read_size];
			int read_block = block_left / read_size > 0 ? read_size: block_left;
			inputFile.read(data, read_block);
			_produced = _produced + read_block / _recsize;
			outputFile.write(data, read_block);	
			block_left = block_left - read_block;
		}
		outputFile.close();
		//TODO: internal sort usig ram data
		if(!copyRamToHDD()) exit(1);
	}
	_produced = 0;
	inputFile.close();
}

bool SortIterator:: copyRamToHDD() {
	ifstream inputFile("DRAM.txt");
    ofstream outputHDDFile("HDD2.txt", ios::app);
	if(inputFile.is_open() && outputHDDFile.is_open()) {
		outputHDDFile<<inputFile.rdbuf();
		inputFile.close();
		outputHDDFile.close();
		clearRam();
	} else {
		cout<<"cannot open files to evict ram"<<endl;
		return false;
	}
	return true;
}
void SortIterator::clearRam() {
	ofstream clearRAM("DRAM.txt", ofstream::trunc);
	clearRAM.close();
}

int SortIterator:: externalMerge() {
	int recordsInEachBatch = divide(DRAM_BYTES, _recsize);
	int steps = divide(recordsInEachBatch, NWAY_MERGE);
	uint ramOffsets[NWAY_MERGE+1] = {0};
	uint hddOffsets[NWAY_MERGE] = {0};
	int ramPartitionSizeInBytes = RoundDown(DRAM_BYTES/(NWAY_MERGE+1), _recsize);
	// int hddPartitionSizeInBytes = RoundDown(HDD_PAGE_SIZE, _recsize);
	for(int i=0;i<steps; i++) {
		initRamMem(ramPartitionSizeInBytes, i);
		/*
		load data from ram to cache and sort 
		//TODO: tournament tree logic
		when any of the cache buffer is empty load from ram
		*/

	}

}

void SortIterator::initRamMem(uint blockSize, int step) {
	for(int i=0;i<NWAY_MERGE;i++){
		loadRamBlocks(i, 0, 0, blockSize, step);
	}
}

bool SortIterator::loadRamBlocks(int partition, int ramOffset, int hddOffset, uint blockSize, int step)
{
	//if hddOffset reaches limit return false
	fstream ram("DRAM.txt", ios::in|ios::out);
	ifstream inputFile("HDD2.txt");
	if(!ram.is_open() || !inputFile.is_open()) {
		cout<<"failed to load data from Hdd"<<endl;
		TRACE(true);
		exit(1);
	}
	//size of each buffer in bytes in ram
	streamoff ramPartitionSizeInBytes = RoundDown(DRAM_BYTES / (NWAY_MERGE + 1), _recsize);
	//size of each buffer in hdd for first run
	streamoff hddPartitionSizeInBytes = RoundDown(DRAM_BYTES, _recsize)*step*NWAY_MERGE;
	if (partition > 0 || ramOffset > 0)
	{
		ram.seekp(partition * ramPartitionSizeInBytes + ramOffset, ios::beg);
	}
	if (partition > 0 || hddOffset > 0)
	{
		streamoff hddPos = hddPartitionSizeInBytes + partition*RoundDown(DRAM_BYTES, _recsize);
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
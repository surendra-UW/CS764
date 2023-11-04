#include <iostream>
#include <fstream>
#include <string>
#include "Sort.h"
using namespace std;
#define DRAM_BYTES 10*1024*1024
#define HDD_PAGE_SIZE 1024*1024

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

	ifstream inputFile("HDD.txt", ios::binary|ios::end);
	if(!inputFile) {
		cout<< "cannot open the hard disk"<<endl; 
		exit(1);
	}
	streampos curr = inputFile.tellg();
	_recsize = curr/_consumed;
	inputFile.close();
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

bool SortIterator::next ()
{
	TRACE (true);
	if (_produced >= _consumed)  return false;
	internalSort();
	return true;
} // SortIterator::next

bool SortIterator:: internalSort() {
	ifstream inputFile("HDD.txt", ios::beg);
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

	inputFile.close();
}

bool SortIterator:: copyRamToHDD() {
	ifstream inputFile("DRAM.txt");
    ofstream outputHDDFile("HDD2.txt", ios::app);
	if(inputFile.is_open() && outputHDDFile.is_open()) {
		outputHDDFile<<inputFile.rdbuf();
		inputFile.close();
		outputHDDFile.close();
		ofstream clearRAM("DRAM.txt", ofstream::trunc);
		clearRAM.close();
	} else {
		cout<<"cannot open files to evict ram"<<endl;
		return false;
	}
	return true;
}
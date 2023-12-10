#include "defs.h"
#include "SSDSort.h"
#include "unistd.h"
#include "constants.h"
#include "internal_sort.h"
#include "TournamentTree.h"
#include "DRAM.h"
#include "SSD.h"
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

SSDSortIterator::SSDSortIterator(SSDSortPlan const *const plan) : _plan(plan), _produced(plan->_produced), batches(0)
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
	if (_produced >= _consumed)
		return false;
	internalSort();
	externalMerge();
	return true;
} // SortIterator::next

uint getMaxSSDBatches()
{
	uint64_t partitions = (SSD_SIZE_IN_BYTES) / (DRAM_SIZE_IN_BYTES);
	return (uint)(0.99 * partitions);
}

bool SSDSortIterator::internalSort()
{
	DRAM dram;
	uint max_batches = getMaxSSDBatches();

	while ((_produced < _consumed) && (batches < max_batches))
	{
		std::vector<RecordStructure> arr;
		uint dramRecords = divide(RoundDown(DRAM_SIZE_IN_BYTES, recordsize), (size_t)recordsize);
		uint recordsToConsume = (_consumed - _produced) > dramRecords ? dramRecords : (_consumed - _produced);

		cout << "dram records " << dramRecords << "rec to consusc " << recordsToConsume << endl;
		dram.loadFromHDD(recordsToConsume);
		cout << "loading ram with rec " << recordsToConsume << endl;

		const int MINI_RUN_SIZE = 1024*1024;
		uint miniRecordsOffset = divide(RoundDown(MINI_RUN_SIZE , recordsize), (size_t)recordsize);
		std::ifstream input_file(DRAM_FILE_NAME.c_str(), std::ios::binary);
	
	    uint i=0;
		std::streampos currentPosition;
        while (!input_file.eof())
	     {
			uint readBlocks = i * miniRecordsOffset;
			arr = read_ramfile(currentPosition+readBlocks, input_file, miniRecordsOffset);
			i++;
		 }
		 input_file.close();



		/*for(int i = 0; i < numChunks; ++i)
		{
            auto start = arr.begin() + i * miniRecords;
            auto end = start + miniRecords;
            if (end > arr.end()) {
               end = arr.end();
             }
            std::vector<RecordStructure> arrMini(start, end);
			quickSort(arrMini, customComparator);
			write_ramfile(arrMini);
			arrMini.clear();
		}*/
		//quickSort(arr, customComparator);
		//write_ramfile(arr);
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

int SSDSortIterator::externalMerge()
{
	TRACE(true);

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

	cache_merge.clearCache();
    dram_merge.clearRam();
}
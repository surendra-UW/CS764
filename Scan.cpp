#include <iostream>
#include <random>
#include <fstream>
#include <vector>
#include <climits>
#include <algorithm>
#include <functional>
#include <string>
#include "Scan.h"
#include "RecordStructure.h"
#include "defs.h"

using namespace std;
// using random_bytes_engine = std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char>;

ScanPlan::ScanPlan(RowCount const count) : _count(count)
{
	TRACE(true);
} // ScanPlan::ScanPlan

ScanPlan::~ScanPlan()
{
	TRACE(true);
} // ScanPlan::~ScanPlan

Iterator *ScanPlan::init() const
{
	TRACE(true);
	return new ScanIterator(this);
} // ScanPlan::init

ScanIterator::ScanIterator(ScanPlan const *const plan) : _plan(plan), _count(0)
{
	TRACE(true);
} // ScanIterator::ScanIterator

ScanIterator::~ScanIterator()
{
	TRACE(true);
	traceprintf("produced %lu of %lu rows\n",
				(unsigned long)(_count),
				(unsigned long)(_plan->_count));
} // ScanIterator::~ScanIterator

bool ScanIterator::next()
// int main()
{
	TRACE(true);

	if (_count >= _plan->_count)
		return false;

	// const int numRecords = 10;
	// const int rec_size = 1000;
	//  Create a random number generator engine and seed it
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define distribution for random values within the character set "0123456789"
	std::uniform_int_distribution<unsigned short> charDistribution(0, 9);

	// Generate random values for the structure members for each record
	for (int k = 0; k < cvalue; ++k)
	{
		RecordStructure rs;

		// strict 3 columns
		int columnLength = svalue / 3;

		// Populate the first three columns with strings of equal length
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < columnLength; ++j)
			{
				rs.members[i] += static_cast<char>('0' + charDistribution(gen));
			}
		}

		// Any extra bytes spill over to the fourth column
		for (int j = 0; j < svalue % 3; ++j)
		{
			rs.members[3] += static_cast<char>('0' + charDistribution(gen));
		}

		// Write the generated strings to a file
		std::ofstream outputFile("HDD.txt", std::ios::app);
		if (!outputFile)
		{
			std::cerr << "Error opening file for appending." << std::endl;
			return 1;
		}

		outputFile << rs.members[0] << "," << rs.members[1] << "," << rs.members[2] << "," << rs.members[3] << "\n";

		outputFile.close();
	}

	printf("Struct %d appended to file.", _count);
	++_count;
	return true;
} // ScanIterator::next


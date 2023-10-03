#include <iostream>
#include <random>
#include "Scan.h"
#include "RecordStructure.h"
// #include <string>
using namespace std;

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
{
	TRACE(true);

	if (_count >= _plan->_count)
		return false;

	RecordStructure rs;

	// Create a random number generator engine and seed it
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define distributions for random values within specific ranges
	std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()); // Adjust range as needed

	// Generate random values for the structure members
	rs.member1 = distribution(gen);
	rs.member2 = distribution(gen);
	rs.member3 = distribution(gen);

	FILE *outputFile = std::fopen("data.txt", "a");
	if (!outputFile)
	{
		printf("Error opening file for appending.");
		// << std::endl;
		return 1;
	}

	fprintf(outputFile, "%d,%d,%d\n", rs.member1, rs.member2, rs.member3);

	std::fclose(outputFile);

	printf("Struct %d appended to file.", _count);
	++_count;
	return true;
} // ScanIterator::next

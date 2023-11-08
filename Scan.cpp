#include <iostream>
#include <random>
#include "Scan.h"
#include "RecordStructure.h"
// #include <string>

#define RANDMIN 100000000000
#define RANDMAX 999999999999
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
	uniform_int_distribution<unsigned long long> distribution(RANDMIN, RANDMAX); // Adjust range as needed

	// Generate random values for the structure members
	rs.member1 = distribution(gen);
	rs.member2 = distribution(gen);
	rs.member3 = distribution(gen);
	rs.member4 = distribution(gen);
	
	FILE *outputFile = std::fopen("HDD.txt", "a");
	if (!outputFile)
	{
		printf("Error opening file for appending.");
		// << std::endl;
		return 1;
	}

	fprintf(outputFile, "%llu,%llu,%llu,%llu\n", rs.member1, rs.member2, rs.member3, rs.member4);

	std::fclose(outputFile);

	printf("Struct %d appended to file.", _count);
	++_count;
	return true;
} // ScanIterator::next

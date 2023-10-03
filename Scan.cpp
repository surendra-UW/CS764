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

	printf("Where is this invoked?");
	if (_count >= _plan->_count)
		return false;

	RecordStructure rs;
	rs.member1 = 1;
	rs.member2 = 2;
	rs.member3 = 3;
	char text[20] = "dummy test ";

	// std::ofstream outputFile("data.txt", std::ios::app);
	FILE *outputFile = std::fopen("data.txt", "a");
	if (!outputFile)
	{
		printf("Error opening file for appending.");
		// << std::endl;
		return 1;
	}

	// Append the struct to the file
	// std::fwrite(&text, sizeof(text), 1, outputFile);
	fprintf(outputFile, "%d,%d,%d\n", rs.member1, rs.member2, rs.member3);

	std::fclose(outputFile);

	printf("Struct %d appended to file.", _count);
	++_count;
	return true;
} // ScanIterator::next

#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include "Scan.h"
#include "Iterator.h"
#include "Filter.h"
#include "Sort.h"

int main(int argc, char *argv[])
{
	int c;
	int cvalue = 0;
	int svalue = 0;
	const char *ovalue = "output.txt";
	while ((c = getopt(argc, argv, "c:s:o:")) != -1)
	{
		switch (c)
		{
		case 'c':
			cvalue = atoi(optarg);
			break;
		case 's':
			svalue = atoi(optarg);
			break;
		case 'o':
			ovalue = optarg;
			break;
		case '?':
			if (optopt == 'c' || optopt == 's' || optopt == 'o')
			{
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			}
			else if (isprint(optopt))
			{
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			}
			else
			{
				fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
			}
			return 1;
		default:
			abort();
		}
	}
	// To test:
	std::cout << "Total number of records: " << cvalue << std::endl;
	std::cout << "Individual record size: " << svalue << std::endl;
	std::cout << "Output trace file: " << ovalue << std::endl;

	TRACE(true);

	Plan *const plan = new SortPlan(new ScanPlan(10000000));
	// new SortPlan ( new FilterPlan ( new ScanPlan (7) ) );

	Iterator *const it = plan->init();
	it->run();
	delete it;

	delete plan;

	return 0;
}
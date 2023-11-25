#include <vector>
#include "RecordStructure.h"

void quickSort(std::vector<RecordStructure>& arr, bool (*comparator)(const RecordStructure&, const RecordStructure&));

void qSort(std::vector<RecordStructure>& arr, int left, int right, bool (*comparator)(const RecordStructure&, const RecordStructure&)); 

int partition(std::vector<RecordStructure>& arr, int left, int right, bool (*comparator)(const RecordStructure&, const RecordStructure&));

std::vector<RecordStructure> read_ramfile(char *inputfile);

void write_ramfile(char *inputfile, std::vector<RecordStructure>& arr);

 bool customComparator(const RecordStructure &a, const RecordStructure &b);
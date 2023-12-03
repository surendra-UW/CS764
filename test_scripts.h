#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <cstdint>
#include <functional>
#include "RecordStructure.h"

size_t hash_function(const RecordStructure& record);

bool verifySortOrder(const std::vector<RecordStructure>& records);

std::bitset<1000000> createBitmap(const std::string& filename);

bool verifyRecords(const std::bitset<1000000>& inputBitmap, const std::bitset<1000000>& outputBitmap);

void processFileInChunks(const std::string& filename, size_t chunkSize);
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string> 
const std::string CACHE_FILE_NAME = "CACHE.txt";
const std::string DRAM_FILE_NAME = "DRAM.txt";
const std::string SSD_FILE_NAME = "SSD.txt";
const std::string HDD_FILE_NAME = "HDD.txt";

const uint64_t CACHE_SIZE_IN_BYTES = 10<<10;
const uint64_t DRAM_SIZE_IN_BYTES = 1<<20;
const uint64_t SSD_SIZE_IN_BYTES = ((uint64_t) 102)<<20;

#endif
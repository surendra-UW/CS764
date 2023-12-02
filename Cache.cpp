#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include "constants.h"
#include "Cache.h"
#include "defs.h"

using namespace std;

template <typename StreamType>
void fileOpenCheck(StreamType& file, string fileName)
{
	if (!file || !file.is_open())
	{
		printf("cannot open the file %s exiting...\n", fileName.c_str());
		exit(EXIT_FAILURE);
	}
}

void Cache::read(int partition, uint64_t block_size)
{
    cout<<"loading partition: "<<partition<<"with block size "<<block_size<<endl;
    uint partition_size = CACHE_SIZE_IN_BYTES/_NWAY;
    ifstream inputFile(DRAM_FILE_NAME);
    fstream cacheFile(CACHE_FILE_NAME, ios::in|ios::out);
    fileOpenCheck(inputFile, DRAM_FILE_NAME);
    fileOpenCheck(cacheFile, CACHE_FILE_NAME);
    cacheFile.seekp(partition_size*partition, ios::beg);
    inputFile.seekg(readOffsets[partition], ios::beg);
    /* Loading into ram in the 1MB chunks*/
    const int load_size = 1<<20;
    char readBuffer[load_size];
    while (block_size > 0 && !inputFile.eof())
    {
        int read_block = block_size > load_size ? load_size : block_size;
        inputFile.read(readBuffer, read_block);
        cout<<"read block: "<<readBuffer<<endl;
        cacheFile.write(readBuffer, read_block);
        block_size -= read_block;
    }
    readOffsets[partition] = inputFile.tellg();
}

void Cache::write(){

}

Cache::Cache(int NWAY) : _NWAY(NWAY)
{
    uint partition_size = DRAM_SIZE_IN_BYTES/NWAY;
    uint cache_partition_size = CACHE_SIZE_IN_BYTES/NWAY;
    readOffsets = new streamoff[NWAY];
    for (int i = 0; i < NWAY; i++) {
        readOffsets[i] = partition_size*i;
        loadOffsets[i] = cache_partition_size*i;
    }
}

Cache::Cache()
{
    fileName = CACHE_FILE_NAME;
    nextHierFileName = DRAM_FILE_NAME;
    sizeInBytes = CACHE_SIZE_IN_BYTES;
}

Cache:: ~Cache(){

}

char *Cache:: readFromCache(int partition) {

}
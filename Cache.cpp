#include <cstdlib>
#include <string>
#include <sstream>
#include "constants.h"
#include "Cache.h"
#include "defs.h"
#include "RecordStructure.h"

using namespace std;

template <typename StreamType>
void fileOpenCheck(StreamType &file, string fileName)
{
    if (!file || !file.is_open())
    {
        printf("cannot open the file %s exiting...\n", fileName.c_str());
        exit(EXIT_FAILURE);
    }
}

int *Cache::getCachePartitionOffsets(int runId)
{
    return cache_partition_offsets[runId];
}

queue<RecordStructure> Cache::loadDataForRun(int runId)
{
    TRACE(true);
    ifstream inputFile(CACHE_FILE_NAME);
    fileOpenCheck(inputFile, CACHE_FILE_NAME);
    int startOffset = cache_partition_offsets[runId][0]; // Replace with your desired starting offset
    int endOffset = cache_partition_offsets[runId][1];   // Replace with your desired ending offset

    cout<<"start offsrt "<<startOffset<<"end offset "<<endOffset<<endl;
    // Move the file pointer to the starting offset
    inputFile.seekg(startOffset, ios::beg);
    // Calculate the number of bytes to read
    int bytesToRead = endOffset - startOffset;
    // Allocate memory to store the read data
    char buffer[bytesToRead+1];
    // Read the specified range of bytes from the file
    inputFile.read(buffer, bytesToRead);
    buffer[bytesToRead] = '\0';
    int j;
    istringstream record_stream(buffer);
    string line;
    queue<RecordStructure> records_in_run;
    RecordStructure record;

    while (getline(record_stream, line, '\n'))
    {
        // make sure if any garbage read is excluded 
        if(line.length() < recordsize-1) {
            continue;
        }

        istringstream column_stream(line);
        string column;
        j =0;

        while(getline(column_stream, column, ',')) {
            record.members[j] = stoull(column);
            j++;
        }

         records_in_run.push(record);

    }

    // Clean up
    inputFile.close();
    cout<<"records pushed is "<<records_in_run.size()<<endl;
    return records_in_run;
}

int Cache::read(int partition)
{
    TRACE(true);
    uint32_t records_to_consume = max_partition_size / recordsize;

    cout<<"records to consume "<<records_to_consume<<endl;
    cout<<"records left to consume  "<<records_in_partition[partition]<<endl;
    // check the records count left in SSD in this partition
    if (records_in_partition[partition] == 0)
        return -1; // indicating end of partition

    // records left in the partition is less than the partition size
    else if (records_in_partition[partition] < records_to_consume)
    {
        cout<<"records in partiton  "<<records_in_partition[partition]<<endl;
        records_to_consume = records_in_partition[partition];
        // records_to_consume denotes the number of records in a partition.
        // save this
        // so in the file seekg
        // max_partition_size is the amount of data to read
        // so basically read from partition_size * partition to partition_size * partition + max_partition_size
        // max_partition_size = records_to_consume * recordsize;
    }
    uint32_t block_size = records_to_consume*recordsize;
    uint partition_size = RoundDown(CACHE_SIZE_IN_BYTES / _NWAY, recordsize);

    cache_partition_offsets[partition][0] = partition_size * partition;
    cache_partition_offsets[partition][1] = partition_size * partition + block_size;

    cout<<"block size " << block_size<<endl;
    ifstream inputFile(DRAM_FILE_NAME);
    fstream cacheFile(CACHE_FILE_NAME, ios::in | ios::out);
    fileOpenCheck(inputFile, DRAM_FILE_NAME);
    // fileOpenCheck(cacheFile, CACHE_FILE_NAME);
    cacheFile.seekp(partition_size * partition, ios::beg);
    inputFile.seekg(readOffsets[partition], ios::beg);
    cout<<"loading into cache "<<partition<<endl;
    /* Loading into ram in the 1MB chunks*/
    const int load_size = 10 << 10;
    char *readBuffer= new char[load_size];
    while (block_size > 0 && !inputFile.eof())
    {
        int read_block = block_size > load_size ? load_size : block_size;
        inputFile.read(readBuffer, read_block);
        cacheFile.write(readBuffer, read_block);
        block_size -= read_block;
    }
    delete[] readBuffer;
    readOffsets[partition] = inputFile.tellg();
    records_in_partition[partition] = records_in_partition[partition] - records_to_consume;
    inputFile.close();
    cacheFile.close();
    return 0;
}

void Cache::write()
{
}

Cache::Cache(int NWAY) : _NWAY(NWAY)
{
    uint partition_size = RoundDown(DRAM_SIZE_IN_BYTES / NWAY, recordsize);
    uint32_t max_records = partition_size / recordsize;

    max_partition_size = RoundDown(CACHE_SIZE_IN_BYTES / NWAY, recordsize);
    readOffsets = new streamoff[NWAY];
    records_in_partition = new uint32_t[NWAY];
    cache_partition_offsets = new int*[NWAY];
    for (int i = 0; i < NWAY; i++)
    {
        readOffsets[i] = partition_size * i;
        records_in_partition[i] = max_records;
        cache_partition_offsets[i] = new int[2];
        if (i == NWAY - 1)
        {
            // last partition
            ifstream inputFile(DRAM_FILE_NAME, ios::binary | ios::ate);
            streamoff dram_size = inputFile.tellg();
            records_in_partition[i] = (dram_size - readOffsets[i]) / recordsize;
            inputFile.close();
        }
    }
}

Cache::Cache()
{
    fileName = CACHE_FILE_NAME;
    nextHierFileName = DRAM_FILE_NAME;
    sizeInBytes = CACHE_SIZE_IN_BYTES;
}

Cache::~Cache()
{
}

streamoff Cache::getReadOffset(int partition)
{
    return readOffsets[partition];
}

void Cache::setReadOffset(int partition, streamoff offset)
{
    readOffsets[partition] = offset;
}
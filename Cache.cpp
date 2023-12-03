
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

queue<RecordStructure> Cache::loadDataForRun(int runId)
{
    read(runId, rounded_cache_block);
    ifstream inputFile(CACHE_FILE_NAME);
    fileOpenCheck(inputFile, CACHE_FILE_NAME);
    int startOffset = cache_partition_offsets[runId][0]; // Replace with your desired starting offset
    int endOffset = cache_partition_offsets[runId][1];   // Replace with your desired ending offset

    // Move the file pointer to the starting offset
    inputFile.seekg(startOffset, ios::beg);

    // Calculate the number of bytes to read
    int bytesToRead = endOffset - startOffset;

    // Allocate memory to store the read data
    char *buffer = new char[bytesToRead];

    // Read the specified range of bytes from the file
    inputFile.read(buffer, bytesToRead);

    int i = 0, j = 0;
    queue<RecordStructure> records_in_run;
    RecordStructure record;
    while (buffer[i] != '\0')
    {
        if (buffer[i] != '\n')
        {
            if (buffer[i] == ',')
            {
                i++;
                continue;
            }

            record.members[j] = int(buffer[i]) - 48;
            j++;
            // cout << buffer[i];
        }
        else
        {
            records_in_run.push(record);
            j = 0;
            // cout << endl;
        }
        i++;
    }
    records_in_run.push(record);
    // Clean up
    delete[] buffer;
    inputFile.close();
}

int Cache::read(int partition, uint64_t block_size)
{
    uint32_t records_to_consume = block_size / recordsize;

    // check the records count left in SSD in this partition
    if (records_in_partition[partition] == 0)
        return -1; // indicating end of partition

    // records left in the partition is less than the partition size
    else if (records_in_partition[partition] < records_to_consume)
    {
        records_to_consume = records_in_partition[partition];
        // records_to_consume denotes the number of records in a partition.
        // save this
        // so in the file seekg
        // block_size is the amount of data to read
        // so basically read from partition_size * partition to partition_size * partition + block_size
        block_size = records_to_consume * recordsize;
    }

    uint partition_size = RoundDown(CACHE_SIZE_IN_BYTES / _NWAY, recordsize);
    cache_partition_offsets[partition][0] = partition_size * partition;
    cache_partition_offsets[partition][1] = partition_size * partition + block_size;

    ifstream inputFile(DRAM_FILE_NAME);
    fstream cacheFile(CACHE_FILE_NAME, ios::in | ios::out);
    fileOpenCheck(inputFile, DRAM_FILE_NAME);
    fileOpenCheck(cacheFile, CACHE_FILE_NAME);
    cacheFile.seekp(partition_size * partition, ios::beg);
    inputFile.seekg(readOffsets[partition], ios::beg);
    /* Loading into ram in the 1MB chunks*/
    const int load_size = 1 << 10;
    char readBuffer[load_size];
    while (block_size > 0 && !inputFile.eof())
    {
        int read_block = block_size > load_size ? load_size : block_size;
        inputFile.read(readBuffer, read_block);
        if (!read_block)
        {
            cout << "read null" << endl;
        }
        cacheFile.write(readBuffer, read_block);
        block_size -= read_block;
    }
    readOffsets[partition] = inputFile.tellg();
    records_in_partition[partition] = records_in_partition[partition] - records_to_consume;
}

void Cache::write()
{
}

Cache::Cache(int NWAY) : _NWAY(NWAY)
{
    uint partition_size = RoundDown(DRAM_SIZE_IN_BYTES / NWAY, recordsize);
    uint32_t max_records = partition_size / recordsize;

    readOffsets = new streamoff[NWAY];
    records_in_partition = new uint32_t[NWAY];
    for (int i = 0; i < NWAY; i++)
    {
        readOffsets[i] = partition_size * i;
        records_in_partition[i] = max_records;
        if (i == NWAY - 1)
        {
            // last partition
            ifstream inputFile(DRAM_FILE_NAME, ios::binary | ios::ate);
            streamoff dram_size = inputFile.tellg();
            records_in_partition[i] = (dram_size - readOffsets[i]) / recordsize;
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
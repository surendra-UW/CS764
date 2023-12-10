#include <fstream>
#include <string>
#include <iostream>
#include "constants.h"
#include "defs.h"
#include "SSD.h"
using namespace std;

// check if file is open if not exit
template <typename StreamType>
void fileOpenCheck(StreamType &file, string fileName)
{
    if (!file || !file.is_open())
    {
        printf("cannot open the file %s exiting...\n", fileName.c_str());
        exit(EXIT_FAILURE);
    }
}

int SSD::read(int partition)
{
    TRACE(true);
    uint32_t records_to_consume = max_partition_size / recordsize;
    cout<<"loading into ssd: "<<partition<<endl;
    // check the records count left in SSD in this partition
    if (records_in_partition[partition] == 0)
        return -1; // indicating end of partition

    // records left in the partition is less than the partition size
    else if (records_in_partition[partition] < records_to_consume)
    {
        records_to_consume = records_in_partition[partition];
    }
    uint64_t block_size = records_to_consume*recordsize;
    //partition size to be used in SSD for merge step 
    uint partition_size = RoundDown(SSD_SIZE_IN_BYTES / _NWAY, recordsize);

    cout<<"dram block size being read "<<block_size<<"records being consumed "<<records_to_consume<<endl;
    cout<<"seek offsets: dram "<<partition_size * partition << " ssd offset "<<readOffsets[partition]<<endl;

    ifstream inputFile(HDD_OUT_FILE_NAME);
    fstream ssdFile(SSD_FILE_NAME, ios::in | ios::out);
    fileOpenCheck(inputFile, HDD_OUT_FILE_NAME);
    fileOpenCheck(ssdFile, SSD_FILE_NAME);
    ssdFile.seekp(partition_size * partition, ios::beg);
    inputFile.seekg(readOffsets[partition], ios::beg);

    /* Loading into ssd in the 10MB chunks due to os limitations
    can be done in larger chunks if os supports */
    const int load_size = 10 << 20;
    char *readBuffer= new char[load_size];
    while (block_size > 0 && !inputFile.eof())
    {
        int read_block = block_size > load_size ? load_size : block_size;
        inputFile.read(readBuffer, read_block);
        // cout<<"ssd read "<<readBuffer<<"length is "<<sizeof(readBuffer)<<endl;
        ssdFile.write(readBuffer, read_block);
        block_size -= read_block;
    }
    delete[] readBuffer;
  
    readOffsets[partition] = inputFile.tellg();
    cout<<"ssd end point is "<<readOffsets[partition]<<endl;
    records_in_partition[partition] = records_in_partition[partition] - records_to_consume;
    ssdFile.close();
    inputFile.close();
    return records_to_consume;
}

void SSD::clearSSD()
{
    ofstream clearSsd(SSD_FILE_NAME, ofstream::trunc);
    clearSsd.close();
}

SSD::SSD()
{
    fileName = SSD_FILE_NAME;
    nextHierFileName = HDD_OUT_FILE_NAME;
    sizeInBytes = SSD_SIZE_IN_BYTES;
}

//constructor for ssd sort
SSD::SSD(int NWAY) : _NWAY(NWAY)
{
    readOffsets = new streamoff[NWAY];
    records_in_partition = new uint32_t[NWAY];
    // initializing offsets for ssd
    streamoff partition_size = 100*RoundDown(DRAM_SIZE_IN_BYTES, recordsize); //assuming max partitions for int sort is 100
    uint32_t max_records = partition_size / recordsize;

    //leave out some space for output buffer in SSD 
    max_partition_size = RoundDown((0.999*SSD_SIZE_IN_BYTES) / NWAY, recordsize);
    TRACE(true);
    cout << "Max partition size of SSD = " << max_partition_size << endl;

    for (int i = 0; i < NWAY; i++)
    {
        readOffsets[i] = i * partition_size;
        records_in_partition[i] = max_records;
        if (i == NWAY - 1)
        {
            // last partition
            ifstream inputFile(HDD_OUT_FILE_NAME, ios::binary | ios::ate);
            streamoff hdd_size = inputFile.tellg();
            records_in_partition[i] = (hdd_size - readOffsets[i]) / recordsize;
            inputFile.close();
        }
    }
    // SSD();
}

SSD::~SSD()
{
    delete readOffsets;
    delete records_in_partition;
}

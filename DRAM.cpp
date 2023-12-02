#include <fstream>
#include <string>
#include <iostream>
#include "constants.h"
#include "defs.h"
#include "DRAM.h"
using namespace std;

//check if file is open if not exit
template <typename StreamType>
void fileOpenCheck(StreamType& file, string fileName)
{
	if (!file || !file.is_open())
	{
		printf("cannot open the file %s exiting...\n", fileName.c_str());
		exit(EXIT_FAILURE);
	}
}

void DRAM::read(int partition, uint64_t block_size) {
    uint partition_size = RoundDown(DRAM_SIZE_IN_BYTES/_NWAY, recordsize);
    ifstream inputFile(SSD_FILE_NAME);
    fstream dramFile(DRAM_FILE_NAME, ios::in|ios::out);
    fileOpenCheck(inputFile, SSD_FILE_NAME);
    fileOpenCheck(dramFile, DRAM_FILE_NAME);
    dramFile.seekp(partition_size*partition, ios::beg);
    inputFile.seekg(readOffsets[partition], ios::beg);
    /* Loading into ram in the 1MB chunks*/
    const int load_size = 1<<20;
    char readBuffer[load_size];
    while (block_size > 0 && !inputFile.eof())
    {
        int read_block = block_size > load_size ? load_size : block_size;
        inputFile.read(readBuffer, read_block);
        // cout<<"dram read "<<readBuffer<<endl;
        dramFile.write(readBuffer, read_block);
        block_size -= read_block;
    }
    readOffsets[partition] = inputFile.tellg();
}

void DRAM::write() {
    
}

void DRAM::clearRam() {
	ofstream clearRAM(DRAM_FILE_NAME, ofstream::trunc);
	clearRAM.close();
}

DRAM::DRAM()
{
    fileName = DRAM_FILE_NAME;
    nextHierFileName = SSD_FILE_NAME;
    sizeInBytes = DRAM_SIZE_IN_BYTES;
    hddSortOffset = 0;
    cout<<"calling default constructor "<<nextHierFileName<<endl;
}

DRAM::DRAM(int NWAY) : _NWAY(NWAY)
{
    readOffsets = new streamoff[NWAY];
    //initializing offsets for ssd
    streamoff partition_size = RoundDown(DRAM_SIZE_IN_BYTES, recordsize);
    for (int i = 0; i < NWAY; i++)
        readOffsets[i] = i*partition_size;
    // DRAM();
}

DRAM:: ~DRAM()
{
    // if(dramOffsets != NULL)
    //     delete dramOffsets;
}


/* This function loads data from HDD in 100MB into DRAM for internal sorting*/
void DRAM::loadFromHDD(uint recordsToConsume){
    ifstream HDDFile(HDD_FILE_NAME, ios::binary);
    fileOpenCheck(HDDFile, HDD_FILE_NAME);
    HDDFile.seekg(hddSortOffset, ios::beg);
    ofstream DRAMFile(DRAM_FILE_NAME, ios::app);
    fileOpenCheck(DRAMFile, DRAM_FILE_NAME);
    const int blockSize = 1<<20;
    char readBuffer[blockSize];
    uint32_t block_left = recordsToConsume*recordsize;
    while (block_left > 0 && !HDDFile.eof())
    {
        int read_block = block_left > blockSize ? blockSize : block_left;
        HDDFile.read(readBuffer, read_block);
        DRAMFile.write(readBuffer, read_block);
        block_left -= read_block;
    }

    hddSortOffset = HDDFile.tellg();
    HDDFile.close();
    DRAMFile.close();
}
#include <fstream>
#include <string>
#include <iostream>
#include "constants.h"
#include "defs.h"
#include "SSD.h"
using namespace std;


int SSD::read(int partition, uint64_t block_size) {
    
}

void SSD::write() {

}


SSD::SSD()
{
    fileName = SSD_FILE_NAME;
    sizeInBytes = SSD_SIZE_IN_BYTES;
}

SSD::SSD(int NWAY) : _NWAY(NWAY)
{
    readOffsets = new streamoff[NWAY];
    for (int i = 0; i < NWAY; i++)
        readOffsets[i] = 0;
    SSD();
}

SSD:: ~SSD()
{
}

streamoff SSD:: getOffset(int partition) {
    return readOffsets[partition];
}

void SSD:: setOffset (int partition, streamoff offset) {
    readOffsets[partition] = offset;
}
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

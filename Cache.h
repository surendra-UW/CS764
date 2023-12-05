#pragma once
#include "Memory.h"
#include "RecordStructure.h"
#include "defs.h"

using namespace std;

class Cache : public Memory
{
public:
    int read(int partition) override;
    void write() override;
    // void clearRam();
    Cache(int NWAY);
    Cache();
    ~Cache();
    streamoff getReadOffset(int partition);
    void setReadOffset(int partition, streamoff offset);
    queue<RecordStructure> loadDataForRun(int runId);
    int *getCachePartitionOffsets(int runId);
    void setRecordsInPartition(int partition, uint32_t records_count);

private:
    int _NWAY;
    streamoff *readOffsets;
    uint32_t *records_in_partition;
    int **cache_partition_offsets;
};

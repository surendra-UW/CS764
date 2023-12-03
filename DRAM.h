#include <fstream>
#include "Memory.h"

using namespace std;

class DRAM : public Memory
{
public:
    int read(int partition) override;
    void write() override;
    void clearRam();
    DRAM(int NWAY);
    DRAM();
    ~DRAM();

    void loadFromHDD(uint recordsToConsume);

private:
    int _NWAY;
    streamoff *readOffsets;
    streamoff hddSortOffset; // for internal sorting
    uint32_t *records_in_partition;
};

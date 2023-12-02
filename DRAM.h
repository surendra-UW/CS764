#include <fstream>
#include "Memory.h"

using namespace std;

class DRAM: public Memory {
    public: 
        void read(int partition, uint64_t offset) override;
        void write() override;
        void clearRam();
        DRAM(int NWAY);
        DRAM();
        ~DRAM();

        void loadFromHDD(uint recordsToConsume);

    private: 
        int _NWAY;
        streamoff *readOffsets;
        streamoff hddSortOffset;
};                                                                                          

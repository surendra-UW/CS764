#include <fstream>
#include "Memory.h"

using namespace std;

class Cache: public Memory {
    public: 
        int read(int partition, uint64_t block_size) override;
        void write() override;
        // void clearRam();
        Cache(int NWAY);
        Cache();
        ~Cache();
        streamoff getReadOffset(int partition);
        void setReadOffset(int partition, streamoff offset); 

    private: 
        int _NWAY;
        streamoff *readOffsets;
        uint32_t *records_in_partition;
};                                                                                          

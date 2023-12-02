#include <fstream>
#include "Memory.h"

using namespace std;

class Cache: public Memory {
    public: 
        void read(int partition, uint64_t block_size) override;
        void write() override;
        // void clearRam();
        Cache(int NWAY);
        Cache();
        ~Cache();
        char *readFromCache(int partition);

    private: 
        int _NWAY;
        streamoff *readOffsets;
        streamoff *loadOffsets;
};                                                                                          

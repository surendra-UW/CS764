#include <fstream>
#include "Memory.h"

using namespace std;

class SSD : public Memory
{
public:
    int read(int partition) override;
    void write() override;
    void clearRam();
    SSD(int NWAY);
    SSD();
    ~SSD();
    streamoff getOffset(int partition);
    void setOffset(int partition, streamoff offset);

private:
    int _NWAY;
    streamoff *readOffsets;
};

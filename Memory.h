#pragma once
class Memory
{
public:
    /*pure virtual functions as of now leaving
    implementation to the derived classes */
    int max_partition_size;
    virtual int read(int partition) = 0;
    virtual void write() = 0;

    virtual ~Memory() {}

protected:
    std::string fileName;
    std::string nextHierFileName;
    uint64_t sizeInBytes;
};
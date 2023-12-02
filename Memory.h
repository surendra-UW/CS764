#pragma once
class Memory {
public:

    /*pure virtual functions as of now leaving 
    implementation to the derived classes */
    virtual int read(int partition, uint64_t block_size) = 0;
    virtual void write() =0;

    virtual ~Memory() {}
protected:
    std::string fileName;
    std::string nextHierFileName;
    uint64_t sizeInBytes;
};
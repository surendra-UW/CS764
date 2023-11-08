#pragma once
#include <string>

class Buffer {
private:
    struct RecordStructure *data;
    int size;
    int head;
    int tail;
    bool inputBuffer;
    string hierarchy;

public:
    Buffer(int bufferSize, bool inputBuffer, string hierarchy);
    ~Buffer();

    bool isEmpty() const;
    bool isFull() const;
    bool write(RecordStructure value);
    bool read(RecordStructure &value);
    bool evict(string hierarchy);
};
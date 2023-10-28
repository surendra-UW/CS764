#pragma once

class Buffer {
private:
    struct RecordStructure *data;
    int size;
    int head;
    int tail;

public:
    Buffer(int bufferSize);
    ~Buffer();

    bool isEmpty() const;
    bool isFull() const;
    bool write(RecordStructure value);
    bool read(RecordStructure &value);
};
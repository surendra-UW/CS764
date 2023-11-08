#include <string>
#include "Buffer.h"
#include "RecordStructure.h"
using namespace std;

Buffer::Buffer(int bufferSize, bool inputBuffer, string hierarchy) : size(bufferSize), head(0), tail(0) {
    data = new RecordStructure[size];
    inputBuffer = inputBuffer;
    hierarchy = hierarchy;
}

Buffer::~Buffer() {
    delete[] data;
}

bool Buffer::isEmpty() const {
    return head == tail;
}

bool Buffer::isFull() const {
    return (head + 1) % size == tail;
}

bool Buffer::write(RecordStructure value) {
    if (!isFull()) {
        data[head] = value;
        head = (head + 1) % size;
        return true;
    } else {
        evict(hierarchy);
    }
    return false; // Buffer is full
}

bool Buffer::read(RecordStructure &value) {
    if (!isEmpty()) {
        value = data[tail];
        tail = (tail + 1) % size;
        return true;
    } else{
        load(hierarchy);
    }
    return false; // Buffer is empty
}

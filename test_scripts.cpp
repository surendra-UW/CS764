#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <bitset>
#include <cstdint>
#include <functional>
#include <algorithm>
#include "RecordStructure.h"

// Hash function for RecordStructure
size_t hash_function(const RecordStructure& record) {
        return record.member1;
}

bool verifySortOrder(const std::vector<RecordStructure>& records) {
    for (size_t i = 1; i < records.size(); ++i) {
        if (records[i].member1 < records[i - 1].member1) {
            return false; // Sort order violation
        }
    }
    return true; // Sort order verified
}

void processFileInChunks(const std::string& filename, size_t chunkSize) {
    std::ifstream file(filename);
    std::vector<RecordStructure> currentChunk;
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        RecordStructure record;
        char comma;

        if (!(iss >> record.member1 >> comma >> record.member2 >> comma >> record.member3 >> comma >> record.member4)) {
            std::cerr << "Error parsing line in input file.\n";
            return;
        }

        currentChunk.push_back(record);

        if (currentChunk.size() >= chunkSize) {
            // Verify the current chunk
            if (!verifySortOrder(currentChunk)) {
                std::cout << "Sort order violation in chunk.\n";
                return;
            }

            // Discard the current chunk from memory
            currentChunk.clear();
        }
    }

    // Verify the last chunk (if any)
    if (!currentChunk.empty() && !verifySortOrder(currentChunk)) {
        std::cout << "Sort order violation in the last chunk.\n";
        return;
    }

    std::cout << "Sort order verified for the entire file.\n";
}

// Function to read a file and create a bitmap
std::bitset<1000000> createBitmap(const std::string& filename) {
    std::ifstream file(filename);
    std::bitset<1000000> bitmap;
    std::string line;
    int i=1;

     RecordStructure record;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        char comma;
        if ((iss >> record.member1 >> comma >> record.member2 >> comma >> record.member3 >> comma >> record.member4)) {
             size_t hashValue = hash_function(record);
             bitmap.set(hashValue % 1000000, true); 
        }
}
return bitmap;
}

// Function to verify records
bool verifyRecords(const std::bitset<1000000>& inputBitmap, const std::bitset<1000000>& outputBitmap) {
    return (inputBitmap == outputBitmap);
}

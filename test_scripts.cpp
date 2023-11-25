#include <iostream>
#include <fstream>
#include <string>
#include "Sort.h"
#include <stdlib.h>
#include <set>
#include <unordered_set>
#include <limits>
#include "internal_sort.h"

using namespace std;


//Verification of sets of rows and values
bool verifySets(char *inputFile, char *outputFile) {
    std::ifstream input(inputFile);
    std::ifstream output(outputFile);

    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Error opening files." << std::endl;
        return false;
    }

    std::set<std::string> inputRows;

    // Read rows from input file and insert into the set
    std::string inputRow;
    while (std::getline(input, inputRow)) {
        inputRows.insert(inputRow);
    }

    // Check if each row in the output file exists in the set
    std::string outputRow;
    while (std::getline(output, outputRow)) {
        if (inputRows.find(outputRow) == inputRows.end()) {
            std::cerr << "Mismatch found: Row in output file not present in input file." << std::endl;
            return false;
        }
    }

    std::cout << "Sets of rows and values verified successfully." << std::endl;
    return true;
}

//Verification of Sort order
bool verifySortOrder(char *outputFile) {
    std::ifstream output(outputFile);

    if (!output.is_open()) {
        std::cerr << "Error opening file." << std::endl;
        return false;
    }

    int previousValue = std::numeric_limits<int>::min();
    int currentValue;

    // Check if a[i] >= a[i-1] for each value in the output file
    while (output >> currentValue) {
        if (currentValue < previousValue) {
            std::cerr << "Sort order mismatch found." << std::endl;
            return false;
        }
        previousValue = currentValue;
    }

    std::cout << "Sort order verified successfully." << std::endl;
    return true;
}


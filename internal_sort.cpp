#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "internal_sort.h"
using namespace std;

bool customComparator(const RecordStructure &a, const RecordStructure &b)
{
    if (a.member1 != b.member1) {
        return a.member1 < b.member1;
    } else if (a.member2 != b.member2) {
        return a.member2 < b.member2;
    } else if (a.member3 != b.member3) {
        return a.member3 < b.member3;
    } else {
        return a.member4 < b.member4;
    }
}

std::vector<RecordStructure> read_ramfile(char *inputfile){
        std::ifstream input_file(inputfile);
    
    if (!input_file) {
        std::cerr << "Error opening input file!" << std::endl;
    }
    
    // Read data from the file and store it in a vector of RecordStructure
    std::vector<RecordStructure> arr;
    std::string line;
    
    while (std::getline(input_file, line)) {
        std::istringstream ss(line);
        std::string token;
        RecordStructure record;

        char delimiter;
        ss >> record.member1 >> delimiter;
        ss >> record.member2 >> delimiter;  
        ss >> record.member3 >> delimiter;
        ss >> record.member4;
        
        arr.push_back(record);
    }
     input_file.close();
    return arr;

}
void write_ramfile(char *inputfile, std::vector<RecordStructure>& arr){
    FILE* output_file = fopen(inputfile, "w");
    
    if (!output_file) {
        std::cerr << "Error opening output file!" << std::endl;
    }
    
    // Write the sorted data back to the file
    for (size_t i = 0; i < arr.size(); ++i) {
        const RecordStructure& record = arr[i];
          fprintf(output_file, "%llu,%llu,%llu,%llu\n",
            static_cast<unsigned long long>(record.member1),
            static_cast<unsigned long long>(record.member2),
            static_cast<unsigned long long>(record.member3),
            static_cast<unsigned long long>(record.member4));
    }
        fclose(output_file);

    std::cout << "Sorting and writing to file complete." << std::endl;

}

void quickSort(std::vector<RecordStructure>& arr,  bool (*comparator)(const RecordStructure&, const RecordStructure&)) {
    
    qSort(arr, 0, arr.size() - 1, comparator);

}

void qSort(std::vector<RecordStructure>& arr, int left, int right,  bool (*comparator)(const RecordStructure&, const RecordStructure&)) {

  int pivot;
  
  if(left < right) {

    pivot = partition(arr, left, right, comparator);

    qSort(arr, left, pivot - 1, comparator);
    qSort(arr, pivot + 1, right, comparator);

  }

}

int partition(std::vector<RecordStructure>& arr, int left, int right,  bool (*comparator)(const RecordStructure&, const RecordStructure&)) {
  
  int pivot = arr[right].member1;
  int i = left - 1;

  for(int j = left; j < right; j++) {
    if(comparator(arr[j], arr[right])) {
      i++;
      std::swap(arr[i], arr[j]);
    }
  }

  std::swap(arr[i + 1], arr[right]);
  
  return i + 1;

}
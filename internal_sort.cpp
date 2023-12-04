#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include "internal_sort.h"
using namespace std;

bool customComparator(const RecordStructure &a, const RecordStructure &b)
{
    if (a.members[0] != b.members[0])
    {
        return a.members[0] < b.members[0];
    }
    else if (a.members[1] != b.members[1])
    {
        return a.members[1] < b.members[1];
    }
    else if (a.members[2] != b.members[2])
    {
        return a.members[2] < b.members[2];
    }
    else
    {
        return a.members[3] < b.members[3];
    }
}

std::vector<RecordStructure> read_ramfile(char *inputfile)
{
    std::ifstream input_file(inputfile);

    if (!input_file)
    {
        std::cerr << "Error opening input file!" << std::endl;
    }

    // Read data from the file and store it in a vector of RecordStructure
    std::vector<RecordStructure> arr;
    std::string line;

    while (std::getline(input_file, line))
    {
        // cout<<line<<endl;
        std::istringstream ss(line);
        std::string token;
        RecordStructure record;

        char delimiter;
        ss >> record.members[0] >> delimiter;
        ss >> record.members[1] >> delimiter;
        ss >> record.members[2] >> delimiter;
        ss >> record.members[3];

        arr.push_back(record);
    }
    input_file.close();
    cout << "array size after reading " << arr.size() << endl;
    return arr;
}
void write_ramfile(char *inputfile, std::vector<RecordStructure> &arr)
{
    FILE *output_file = fopen(inputfile, "w");

    if (!output_file)
    {
        std::cerr << "Error opening output file!" << std::endl;
    }

    cout << "aray size after internal sorting "<<arr.size() << endl;
    // Write the sorted data back to the file
    for (size_t i = 0; i < arr.size(); ++i)
    {
        const RecordStructure &record = arr[i];
        fprintf(output_file, "%llu,%llu,%llu,%llu\n",
                static_cast<unsigned long long>(record.members[0]),
                static_cast<unsigned long long>(record.members[1]),
                static_cast<unsigned long long>(record.members[2]),
                static_cast<unsigned long long>(record.members[3]));
    }
    fclose(output_file);

    std::cout << "Sorting and writing to file complete." << std::endl;
}

void quickSort(std::vector<RecordStructure> &arr, bool (*comparator)(const RecordStructure &, const RecordStructure &))
{

    qSort(arr, 0, arr.size() - 1, comparator);
}

void qSort(std::vector<RecordStructure> &arr, int left, int right, bool (*comparator)(const RecordStructure &, const RecordStructure &))
{

    int pivot;

    if (left < right)
    {

        pivot = partition(arr, left, right, comparator);

        qSort(arr, left, pivot - 1, comparator);
        qSort(arr, pivot + 1, right, comparator);
    }
}

int partition(std::vector<RecordStructure> &arr, int left, int right, bool (*comparator)(const RecordStructure &, const RecordStructure &))
{

    int pivot = arr[right].members[0];
    int i = left - 1;

    for (int j = left; j < right; j++)
    {
        if (comparator(arr[j], arr[right]))
        {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }

    std::swap(arr[i + 1], arr[right]);

    return i + 1;
}
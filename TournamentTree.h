#pragma once
#include <cstdio>

using namespace std;

FILE *openFile(char *fileName, char *mode);

void mergeFiles(char *output_file, int n, int k);

void createInitialRuns(char *input_file, int run_size, int num_ways);

void externalSort(char *input_file, char *output_file, int num_ways, int run_size);

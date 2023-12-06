#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main () {
    ifstream inputFile("./out/SSD_OUT.txt");
	ofstream outputFile("./out/HDD_OUT.txt", ios::app);
	if (inputFile.is_open() && outputFile.is_open())
	{
		outputFile << inputFile.rdbuf();
		inputFile.close();
		outputFile.close();
		// clearRam();
	}
	else
	{
		cout << "cannot open files to evict output data" << endl;
	}

    ofstream clearFile("./out/SSD_OUT.txt", ofstream::trunc);
    clearFile.close();
    return 0;
    
};
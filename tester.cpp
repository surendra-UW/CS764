#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main () {
    char test[100] = "a,b,c,d\ne,f,g,h\n";
    istringstream ss(test);
    string line;
    
    while (getline(ss, line, '\n')) 
    {
        cout<<line<<endl;
        istringstream records(line);
        string cloumn;

        while(getline(records, cloumn, ',')) {
            cout<<cloumn<<endl;
        }
    }
    return 0;
    
};
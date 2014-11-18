//============================================================================
// Name        : Batch.cpp
// Author      : Greg Burgess
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

void parseFile(const char *path) {
	ifstream myfile(path);
	if(!myfile) {
		cout<<"Error opening output file"<<endl;
		system("pause");
		exit(1);
	}

	const size_t SIZE = 30;
	string line[SIZE]; // creates SIZE empty strings
	size_t i=0;
	while(!myfile.eof() && i < SIZE) {
		getline(myfile,line[i]); // read the next line into the next string
		++i;
	}
	size_t numLines = i;

	for (i=0; i < numLines; ++i) {
		cout << i << ". " << line[i]; // no need to test for empty lines any more
	}
		return;
}


int main(int argc, char* argv[]) {
	if (argc < 4) {
		cout << "Usage: <filename>";
		exit(0);
	}
	parseFile(argv[2]);
	return 0;
}

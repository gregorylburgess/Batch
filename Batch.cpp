//============================================================================
// Name        : Batch.cpp
// Author      : Greg Burgess, Jack Lam
// Version     : 0.01
// Copyright   : GNU General Public License V3
// Description : A Bat Scheduling simulator to test out various scheduling algorithms.
//============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "Proc.h"
using namespace std;

#define SIG_POS 11

Proc * parseFile(string path, int length) {
	int i=0,j=0;
	int *array = (int *)calloc(SIG_POS,sizeof(int));
	Proc * procs = (Proc *)calloc(length,sizeof(Proc));
	char line [1024];
	char * temp;
	FILE *fp;
	//Open the file
	fp = fopen(path.c_str(),"r");
	while (fgets(line, sizeof(line), fp) != NULL && i<length) {
		j=0;
		//Grab the first token
		temp = strtok(line," \t");
		array[j]=atoi(temp);
		//Grab the next SIG_POS
		while(temp!=NULL && j<SIG_POS) {
			//Dump into array
			array[j]=atoi(temp);
			temp = strtok(NULL," \t");
			j++;
		}
		procs[i].init(array);
		i++;
	}
	fclose(fp);
	return procs;
}


int main(int argc, char* argv[]) {
	if (argc < 3) {
		cout << "Usage: <filename> <number of files to read from log>";
		exit(0);
	}
	int NUM_ENTRIES_TO_PROCESS = atoi(argv[2]);
	Proc * array = parseFile(argv[1],NUM_ENTRIES_TO_PROCESS);
	int i=0;
	long totalRuntime = 0;
	//Sum Runtimes
	for(i=0; i<	NUM_ENTRIES_TO_PROCESS;i++) {
		totalRunTime += array[i].runTime);
	}	
	//Any Bat scheduling algorithm takes at 
	//most 2x as long as the optimal schedule.
	totalRuntime *= 2; 

	
	return 0;
}

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
#include "Slot.h"
using namespace std;

#define SIG_POS 11

//Check avail procs at this step

Proc * parseFile(string path, int length) {
	int *array = (int *)calloc(SIG_POS,sizeof(int)),
		i=0,
		j=0;
	char line [1024],
		 * temp;
	Proc * procs = (Proc *)calloc(length,sizeof(Proc));
	FILE *fp;

	fp = fopen(path.c_str(),"r");		//Open the file
	while (fgets(line, sizeof(line), fp) != NULL && i<length) {
		j=0;
		temp = strtok(line," \t");		//Grab the first token
		array[j]=atoi(temp);
		while(temp!=NULL && j<SIG_POS) {//Grab the next SIG_POS lines
			array[j]=atoi(temp);		//Dump into array
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
	if (argc < 4) {
		cout << "Usage: <filename> <number of files to read from log> <number of cores available>";
		exit(0);
	}
	int NUM_ENTRIES_TO_PROCESS = atoi(argv[2]),
		NUM_CORES = atoi(argv[3]),
		i=0;
	long totalRuntime = 0;
	Proc * queue = parseFile(argv[1], NUM_ENTRIES_TO_PROCESS);

	for(i=0; i<NUM_ENTRIES_TO_PROCESS; i++) {//Sum Runtimes & validate core count
		totalRuntime += queue[i].runTime;
		if(NUM_CORES < queue[i].numProc) {
			printf("Error, a job requested more than the maximum number of cores!\
					\n Available:%i\n Requested:%i\n", NUM_CORES, queue[i].numProc);
			exit(0);
		}
	}
	//Any Bat scheduling algorithm takes at most 2x as long as the optimal sequential schedule.
	totalRuntime *= 2; 

	Slot timeSlot[totalRuntime];
	for(i=0;i<NUM_ENTRIES_TO_PROCESS;i++) { 
		timeSlot[i].init(NUM_CORES, NUM_ENTRIES_TO_PROCESS);
	}

	#ifdef FCFS
		
	#endif

	
	return 0;
}













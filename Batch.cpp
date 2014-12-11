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
#include <map>
#include <vector>
#include "Slot.h"
using namespace std;

#define SIG_POS 11
long cleanSlots=0, dirty=0, numTimeSlots=0;
int NUM_ENTRIES_TO_PROCESS=0, NUM_CORES=0;

#include "Algorithms.h"

Proc * parseFile(string path, int NUM_ENTRIES_TO_PROCESS) {
	int *array = (int *)calloc(SIG_POS,sizeof(int)),
		i=0,
		j=0;
	char line [1024],
		 * temp;
	Proc * procs = (Proc *)calloc(NUM_ENTRIES_TO_PROCESS,sizeof(Proc));
	FILE *fp;

	fp = fopen(path.c_str(),"r");		//Open the file
	while (fgets(line, sizeof(line), fp) != NULL && i<NUM_ENTRIES_TO_PROCESS) {
		j=0;
		temp = strtok(line," \t");		//Grab the first token
		array[j]=atoi(temp);
		while(temp!=NULL && j<SIG_POS) {//Grab the next SIG_POS lines
			array[j]=atoi(temp);		//Dump into array
			temp = strtok(NULL," \t");
			j++;
		}
		if (array[4]>=0 && array[3]>0) {  //Ignore lines that don't have a specified processor request or runtimes <= 0
			array[0]=i; //Make our own IDs
			procs[i].init(array);
			i++;
		}
	}
	fclose(fp);
	return procs;
}

int main(int argc, char* argv[]) {
	if (argc < 4) {
		cout << "Usage: <filename> <number of lines to read from log> <number of cores available>";
		exit(0);
	}
	NUM_ENTRIES_TO_PROCESS = atoi(argv[2]);
	NUM_CORES = atoi(argv[3]);

	#ifndef SILENT
	printf("Parsing file...\n");
	#endif
	Proc * queue = parseFile(argv[1], NUM_ENTRIES_TO_PROCESS);
	#ifndef SILENT
	printf("Normalizing times\n");
	#endif
	long startTime = queue[0].submitTime;
	long maxRT = queue[0].runTime;
	int i=0;
	for(i=0; i<NUM_ENTRIES_TO_PROCESS; i++) {//validate core count
		queue[i].submitTime -= startTime;  //Start the first submit time at 0 to save space
		maxRT=max(maxRT,queue[i].runTime);
		if(NUM_CORES < queue[i].numProc) {
			printf("Error, a job requested more than the maximum number of cores!\
					\n Available:%i\n Requested:%i\n", NUM_CORES, queue[i].numProc);
			exit(0);
		}
	}
	#ifndef SILENT
	printf("Building timespan...\n");
	#endif
	numTimeSlots = 1.5*maxRT;
	#ifdef DEBUG
	printf("numTimeSlots:%li\n",numTimeSlots);
	#endif
	#ifndef SILENT
	printf("Allocating...\n");
	#endif
	Slot * timeSlot = (Slot *)calloc(numTimeSlots,sizeof(Slot));
	//Slot timeSlot[numTimeSlots];

	for(i=0;i<numTimeSlots;i++) { 
		timeSlot[i].init(NUM_CORES);
	}
	//if you run out, you have to clean up!
	cleanSlots=numTimeSlots;
	dirty=0;

	//Allocating variables fo math
	map<int, long> slowDown;
	map<int, long> waitTime;
	map<int, long> turnAroundTime;
	map<int, long>::iterator it;
	#ifndef SILENT
	printf("Making schedule...\n");
	#endif
	#ifdef FCFS
	long time = makeFCFS(queue,timeSlot, slowDown, waitTime, turnAroundTime);
	#endif
	#ifdef BACKFILL
	long time = makeBackfill(queue,timeSlot, slowDown, waitTime, turnAroundTime);
	#endif
	#ifdef SPIRAL
	long time = makeBalancedSpiral(queue, slowDown, waitTime, turnAroundTime);
	#endif
	#ifdef EASY
	long time = makeEasy(queue,timeSlot, slowDown, waitTime, turnAroundTime);
	#endif

	long totalSlowDown = 0,  maxSlowDown=0;
	for(it = slowDown.begin();it != slowDown.end(); it++){
		totalSlowDown += it->second;
		maxSlowDown = max(maxSlowDown,it->second);
	}
	double avgSlowDown = totalSlowDown/NUM_ENTRIES_TO_PROCESS;
	printf("\nTotal Slowdown: %li\n",totalSlowDown);
	printf("Avg Slowdown: %f\n",avgSlowDown);
	printf("Max Slowdown: %li \n",maxSlowDown);
	printf("Total run time: %li\n",time+startTime);

	return 0;
}













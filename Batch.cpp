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

void allocate(int i, Slot* timeslot, long runTime, int coreCount) {
	int j=i;
	int end=i+runTime;
	for(j=i;j<end;j++) {
		timeslot[j].cores -= coreCount;
	}
	return;
}

long makeSchedule(Proc *queue, Slot* timeslot,int NUM_ENTRIES_TO_PROCESS) {
	long time=0;
	int i=0, j=0;
	int minProcs = queue[0].numProc;
	std::map<long,Proc> runningProcs;
	vector<long> current_procs;
	//While we have processes to schedule...
	printf("%i<%i\n", i, NUM_ENTRIES_TO_PROCESS);
	while (i<NUM_ENTRIES_TO_PROCESS) {
		//while we have free cores to schedule at this timestep...
		printf("scheduling job %i...%i<%i\n",i, minProcs, timeslot[time].cores);
		printf("time:%li\n",time);
		while (minProcs <= timeslot[time].cores) {
			allocate(time,timeslot,queue[i].runTime, timeslot[i].cores);
			i++;
			minProcs = queue[i].numProc;
		}
		time += 1;
	}
	return time;
}

int main(int argc, char* argv[]) {
	if (argc < 4) {
		cout << "Usage: <filename> <number of lines to read from log> <number of cores available>";
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
	for(i=0;i<totalRuntime;i++) { 
		timeSlot[i].init(NUM_CORES, NUM_ENTRIES_TO_PROCESS);
	}
	printf("Making schedule...\n");
	long time = makeSchedule(queue,timeSlot,NUM_ENTRIES_TO_PROCESS);
	printf("%li\n",time);
	
	return 0;
}













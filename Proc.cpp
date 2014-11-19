#include "Proc.h"
Proc::Proc() {
	ID=0;
	numProc=0;
	submitTime=0;
	runTimeEstimate=0;
	runTime=0;
}

Proc::Proc(int * array) {
	init(array);
}

Proc::Proc(int newID, long newSubmitTime, long waitTime, long newRunTime, 
		   int newNumProc, long newRunTimeEstimate) {
	ID=newID;
	numProc=newNumProc;
	submitTime=newSubmitTime;
	runTimeEstimate=newRunTimeEstimate;
	runTime=newRunTime;
}

void Proc::init(int * array) {
	ID=array[ID_INDEX];
	numProc=array[NUMPROC_INDEX];
	submitTime=array[SUBMITTIME_INDEX];
	runTimeEstimate=array[RUNTIMEESTIMATE_INDEX];
	runTime=array[RUNTIME_INDEX];
}


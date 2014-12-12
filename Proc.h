#define ID_INDEX 0
#define SUBMITTIME_INDEX 1
#define WAITTIME_INDEX 2
#define RUNTIME_INDEX 3
#define NUMPROC_INDEX 4
#define RUNTIMEESTIMATE_INDEX 8

struct Proc {
	int ID;
	int numProc;
	long submitTime;
	long runTimeEstimate;
	long runTime;
	long waitTime;
	Proc();
	Proc(int * array);
	Proc(int newID, long newSubmitTime, long waitTime, long newRunTime, int newNumProc, long newRunTimeEstimate);
	void init(int * array);
};

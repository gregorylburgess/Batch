#define ID_INDEX 0
#define SUBMITTIME_INDEX 1
#define WAITTIME_INDEX 2
#define RUNTIME_INDEX 3
#define NUMPROC_INDEX 4
#ifdef USERT //Pass USERT as a parameter if the dataset doesn't give estimated runtimes
	//If the dataset doesn't give estimated runtime, use real runtime
	#define RUNTIMEESTIMATE_INDEX 3
#else
	//Otherwise, use the real runtime
	#define RUNTIMEESTIMATE_INDEX 9
#endif
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

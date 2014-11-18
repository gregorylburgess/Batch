struct Proc {
  int ID;
  int numProc;
  long submitTime;
  long runTimeEstimate;
  long runTime;
  long startTime;

  Proc(int myID, int mynumProc, long mysubmitTime, long myrunTimeEstimate, long myrunTime);
};

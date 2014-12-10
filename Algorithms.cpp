
void allocate(int i, Slot* timeslot, long runTime, int coreCount) {
	int j=i;
	int end=i+runTime;
	for(j=i;j<end;j++) {
		timeslot[j].cores -= coreCount;
	}
	return;
}

long makeFCFS(Proc *queue, Slot* timeslot,int NUM_ENTRIES_TO_PROCESS) {
	long time= queue[0].submitTime, endTime=0;
	int i=0, j=0;
	int minProcs = queue[0].numProc;
	map<long,Proc> runningProcs;
	vector<long> current_procs;
	//While we have processes to schedule...
printf("%i < %i\n", i, NUM_ENTRIES_TO_PROCESS);
	while (i<NUM_ENTRIES_TO_PROCESS) {
		//while we have free cores to schedule at this timestep...
		while (queue[i].submitTime <= time && minProcs <= timeslot[time].cores && i<NUM_ENTRIES_TO_PROCESS) {
printf("st:%li, time:%li, rc:%i, ac:%i\n",queue[i].submitTime, time, minProcs, timeslot[time].cores);
			endTime = max(endTime,time+queue[i].runTime);
printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",queue[i].ID, time,minProcs, timeslot[time].cores,queue[i].runTime,endTime);
			allocate(time,timeslot,queue[i].runTime, queue[i].numProc);
			i++;
			minProcs = queue[i].numProc;
		}
		
		time += 1;
	}
	return endTime;
}

long makeBackfill(Proc *queue, Slot* timeslot,int NUM_ENTRIES_TO_PROCESS) {
	long time=0;
	long endTime = 0;
	long endTimeForJob = 0;
	int i=0;
	vector<long> current_procs;
	vector<Proc> openJobs;
	int queuePosition = 0;
	//While we have processes to schedule...
	while (true) {
		if(queuePosition >= NUM_ENTRIES_TO_PROCESS){
			//Did all we wanted to do
			break;
		}

		//Get all the new jobs and push them into the openjobs.
		while((queue[queuePosition].submitTime <= time) && (queuePosition < NUM_ENTRIES_TO_PROCESS)){
			openJobs.push_back(queue[queuePosition]);
			queuePosition++;
		}

		//Got some openjobs, time to start scheduling with backfill
		for(i=0; i<openJobs.size(); i++){
			if(openJobs[i].numProc <= timeslot[time].cores){//I fit, time to allocate
				endTime = max(endTime,time+openJobs[i].runTime);
				endTimeForJob = time+openJobs[i].runTime;
				printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",openJobs[i].ID, time,openJobs[i].numProc, timeslot[time].cores,openJobs[i].runTime, endTimeForJob);
				allocate(time,timeslot,openJobs[i].runTime, openJobs[i].numProc);
				openJobs.erase(openJobs.begin()+i);
				i--;
			}
			//else continue since I can't fit this one but I might be able to fit the next one.
		}

		time += 1;
	}
	return endTime;
}

vector<Proc> balancedSpiralHeuristic(vector<Proc> & openJobs, vector<Proc> & oldBalancedJobs){
	vector<Proc> balancedJobs;
	vector<Proc> L;
	vector<Proc> R;
	int sum_L = 0;
	int sum_R = 0;
	Proc x;
	Proc last;
	int i,j;
	int n = openJobs.size() + oldBalancedJobs.size();

	//merge the old balanced jobs into the new open jobs and re-run heuristic
	for(i=0; i<oldBalancedJobs.size(); i++){
		openJobs.push_back(oldBalancedJobs[i]);
	}

	//sort the jobs in terms of weight. Insertion sort
	for(i =1; i<n-1; i++){
		x = openJobs[i];
		j = i;
		while((j>0) && (openJobs[j-1].numProc > x.numProc)){
			openJobs[j] = openJobs[j-1];
			j--;
		}
		openJobs[j] = x;
	}

	//Jobs are sorted.
	//Set biggest job to last and remove
	last = openJobs.back();
	openJobs.pop_back();

	//check to see if empty
	if(openJobs.size() == 0){
		balancedJobs.push_back(last);
		return balancedJobs;
	}

	printf("I made it here\n");
	//Place n-1 in right and n-2 in left
	R.push_back(openJobs.back());
	sum_R += openJobs.back().numProc;
	openJobs.pop_back();

	//Check to see if empty
	if(openJobs.size() == 0){
		balancedJobs.push_back(R.front());
		balancedJobs.push_back(last);
		return balancedJobs;
	}

	L.push_back(openJobs.back());
	sum_L += openJobs.back().numProc;
	openJobs.pop_back();

	//Check to see if empty
	if(openJobs.size() == 0){
		balancedJobs.push_back(L.front());
		balancedJobs.push_back(R.front());
		balancedJobs.push_back(last);
		return balancedJobs;
	}


	//Sum_L and Sum_R initialized, begin balancing
	while(openJobs.size() != 0){
		if(sum_L < sum_R){ //add largest job to last position of L
			L.push_back(openJobs.back());
			sum_L += openJobs.back().numProc;
			openJobs.pop_back();
		}
		else{//sum_L >= sum_R, add largest job to first position of R
			R.insert(openJobs.begin(), openJobs.back());
			sum_R += openJobs.back().numProc;
			openJobs.pop_back();
		}
	}

	//L and R are done, merge them together.
	while(L.size() != 0){
		balancedJobs.push_back(L.front());
		L.erase(L.begin());
	}
	while(R.size() != 0){
		balancedJobs.push_back(R.front());
		R.erase(R.begin());
	}

	//attach last job
	balancedJobs.push_back(last);

	//all done! time to return
	return balancedJobs;

}


long makeBalancedSpiral(Proc *queue, Slot* timeslot,int NUM_ENTRIES_TO_PROCESS) {
	long time=0;
	long endTime = 0;
	long endTimeForJob = 0;
	int i=0;
	vector<long> current_procs;
	vector<Proc> openJobs;
	vector<Proc> balancedJobs;
	int queuePosition = 0;
	//While we have processes to schedule...
	while (true) {


		if(queuePosition >= NUM_ENTRIES_TO_PROCESS){
			//Did all we wanted to do
			break;
		}

		//Get all the new jobs and push them into the openjobs.
		while((queue[queuePosition].submitTime <= time) && (queuePosition < NUM_ENTRIES_TO_PROCESS)){
			openJobs.push_back(queue[queuePosition]);
			queuePosition++;
		}

		//Got some openjobs, want to balance them
		if(openJobs.size() > 0){
			balancedJobs = balancedSpiralHeuristic(openJobs, balancedJobs);

		}


		//Got some openjobs, time to start scheduling with backfill
		for(i=0; i<balancedJobs.size(); i++){
			if(balancedJobs[i].numProc <= timeslot[time].cores){//I fit, time to allocate
				endTime = max(endTime,time+balancedJobs[i].runTime);
				endTimeForJob = time+balancedJobs[i].runTime;
				printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",balancedJobs[i].ID, time,balancedJobs[i].numProc, timeslot[time].cores,balancedJobs[i].runTime, endTimeForJob);
				allocate(time,timeslot,balancedJobs[i].runTime, balancedJobs[i].numProc);
				balancedJobs.erase(balancedJobs.begin()+i);
				i--;
			}
			//else continue since I can't fit this one but I might be able to fit the next one.
		}

		time += 1;
	}
	return endTime;
}


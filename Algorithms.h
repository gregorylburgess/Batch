
void clean(Slot* timeslot,int currentTime, int runTime) {
		//printf("cleanSlots: %li, runTime: %i\n",cleanSlots,runTime); 
		//printf("dirty:%li\n",dirty);
		//printf("current Time:%li\n",currentTime%numTimeSlots);
		int i=dirty;
		while(i!=currentTime%numTimeSlots){
			timeslot[i].cores=NUM_CORES;
			i=(i+1)%numTimeSlots;
			cleanSlots++;
		}
		dirty=(currentTime-1 + numTimeSlots)%numTimeSlots;
		//printf("dirty:%li\n",dirty);
		//printf("cleanSlots: %li\n",cleanSlots); 
	}

void allocate(int currentTime, Slot* timeslot,  long runTime, int coreCount) {
	int i = 0;
	//if(cleanSlots < runTime) { //If you pass Dirty, clean first
		//clean from dirty to the currentTime.
		clean(timeslot, currentTime,runTime);
	//}
	cleanSlots -= runTime;//Mark the allocated slots dirty
	for(i=0;i<runTime;i++) {
		timeslot[(currentTime+i)%numTimeSlots].cores -= coreCount;
	}
	return;
}



long makeFCFS(Proc *queue, Slot* timeslot,  map<int,long>& slowDown, map<int,long>& waitTime, map<int,long>& turnAroundTime) {
	long time= queue[0].submitTime, endTime=0, dirty=0;
	int i=0, j=0;
	int minProcs = queue[0].numProc;
	map<long,Proc> runningProcs;
	vector<long> current_procs;
	//While we have processes to schedule...
	while (i<NUM_ENTRIES_TO_PROCESS) {
		//while we have free cores to schedule at this timestep...
		while (queue[i].submitTime <= time && minProcs <= timeslot[time%numTimeSlots].cores && i<NUM_ENTRIES_TO_PROCESS) {

			endTime = max(endTime,time+queue[i].runTime);
			#ifndef SILENT
			printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",queue[i].ID, time,minProcs, 
timeslot[time%numTimeSlots].cores,queue[i].runTime,endTime);
			#endif
			allocate(time,timeslot,queue[i].runTime, queue[i].numProc);

			slowDown[queue[i].ID] = (time - queue[i].submitTime + queue[i].runTime) / queue[i].runTime;
			waitTime[queue[i].ID] = time - queue[i].submitTime ;
			turnAroundTime[queue[i].ID] = time - queue[i].submitTime  + queue[i].runTime;


			i++;
			minProcs = queue[i].numProc;
		}
		time += 1;
	}
	return endTime;
}

long makeBackfill(Proc *queue, Slot* timeslot,  map<int,long>& slowDown, map<int,long>& waitTime, map<int,long>& turnAroundTime){
	long time=0;
	long endTime = 0;
	long endTimeForJob = 0;
	int i=0;
	vector<long> current_procs;
	vector<Proc> openJobs;
	int queuePosition = 0;
	//While we have processes to schedule...
	while (true) {
		if(queuePosition >= NUM_ENTRIES_TO_PROCESS && time > endTime){
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
			if(openJobs[i].numProc <= timeslot[time%numTimeSlots].cores){//I fit, time to allocate
				endTime = max(endTime,time+openJobs[i].runTime);
				endTimeForJob = time+openJobs[i].runTime;
				#ifndef SILENT
				printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",openJobs[i].ID, time,openJobs[i].numProc, timeslot[time%numTimeSlots].cores,openJobs[i].runTime, endTimeForJob);
				#endif
				allocate(time,timeslot,openJobs[i].runTime, openJobs[i].numProc);

				slowDown[openJobs[i].ID] = (time - openJobs[i].submitTime + openJobs[i].runTime) / openJobs[i].runTime;
				waitTime[openJobs[i].ID] = time - openJobs[i].submitTime ;
				turnAroundTime[openJobs[i].ID] = time - openJobs[i].submitTime + openJobs[i].runTime;

				openJobs.erase(openJobs.begin()+i);
				i--;
			}
			//else continue since I can't fit this one but I might be able to fit the next one.
		}

		time += 1;
	}
	return endTime;
}

vector<Proc> balancedSpiralHeuristic(vector<Proc> & openJobs){
	vector<Proc> balancedJobs;
	vector<Proc> L;
	vector<Proc> R;
	int sum_L = 0;
	int sum_R = 0;
	Proc x;
	Proc last;
	int i,j;
	int n = openJobs.size();



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
			R.insert(R.begin(), openJobs.back());
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


long makeBalancedSpiral(Proc *queue, Slot* timeslot,   map<int,long>& slowDown, map<int,long>& waitTime, map<int,long>& turnAroundTime) {
	long time=0;
	long endTime = 0;
	long endTimeForJob = 0;
	int i=0;
	vector<long> current_procs;
	vector<Proc> openJobs;
	int queuePosition = 0;
	//While we have processes to schedule...
	while (true) {


		if(queuePosition >= NUM_ENTRIES_TO_PROCESS && time > endTime){
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
			openJobs = balancedSpiralHeuristic(openJobs);

		}


		//Got some openjobs, time to start scheduling with backfill
		for(i=0; i<openJobs.size(); i++){
			if(openJobs[i].numProc <= timeslot[time%numTimeSlots].cores){//I fit, time to allocate
				endTime = max(endTime,time+openJobs[i].runTime);
				endTimeForJob = time+openJobs[i].runTime;
				#ifndef SILENT
				printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",openJobs[i].ID, time,openJobs[i].numProc, timeslot[time%numTimeSlots].cores,openJobs[i].runTime, endTimeForJob);
				#endif
				allocate(time,timeslot,openJobs[i].runTime, openJobs[i].numProc);

				slowDown[openJobs[i].ID] = (time - openJobs[i].submitTime + openJobs[i].runTime) / openJobs[i].runTime;
				waitTime[openJobs[i].ID] = time - openJobs[i].submitTime ;
				turnAroundTime[openJobs[i].ID] = time - openJobs[i].submitTime + openJobs[i].runTime;

				openJobs.erase(openJobs.begin()+i);
				i--;
			}
			//else continue since I can't fit this one but I might be able to fit the next one.
		}

		time += 1;
	}
	return endTime;
}

long makeEasy(Proc *queue, Slot* timeslot, map<int,long>& slowDown, map<int,long>& waitTime, map<int,long>& turnAroundTime) {
	long time=0;
	long endTime = 0;
	long endTimeForJob = 0;
	int i=0;
	vector<long> current_procs;
	vector<Proc> openJobs;
	vector<Proc> balancedJobs;
	int queuePosition = 0;
	int futureProcsRemaining;
	int totalCores = timeslot[0].cores;

	//While we have processes to schedule...
	while (true) {
		if(queuePosition >= NUM_ENTRIES_TO_PROCESS && time > endTime){
			//Did all we wanted to do
			break;
		}

		//Get all the new jobs and push them into the openjobs.
		while((queue[queuePosition].submitTime <= time) && (queuePosition < NUM_ENTRIES_TO_PROCESS)){
			openJobs.push_back(queue[queuePosition]);
			queuePosition++;
		}

		//Got some openjobs, time to start scheduling with EASY
		for(i=0; i<openJobs.size(); i++){
			//Check to see if I have a priority job
			if(openJobs.front().numProc <= timeslot[time%numTimeSlots].cores && i==0){
				endTime = max(endTime, time+openJobs.front().runTime);
				endTimeForJob = time + openJobs.front().runTime;

				#ifndef SILENT
				printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",openJobs.front().ID, time,openJobs.front().numProc, timeslot[time%numTimeSlots].cores,openJobs.front().runTime, endTimeForJob);
				#endif
				allocate(time,timeslot,openJobs.front().runTime,openJobs.front().numProc);
				slowDown[openJobs[i].ID] = (time - openJobs[i].submitTime  + openJobs[i].runTime) / openJobs[i].runTime;
				waitTime[openJobs[i].ID] = time - openJobs[i].submitTime;
				turnAroundTime[openJobs[i].ID] = time - openJobs[i].submitTime + openJobs[i].runTime;

				openJobs.erase(openJobs.begin());
				i--;
				//Set the amount of cores I need in the future for the new front job.
				futureProcsRemaining = totalCores - openJobs.front().numProc;
			}

			else{ // I am not the first job so I should try to backfill
				//Check if I can finish before the current job so the priority jobs can start asap
				if((openJobs[i].numProc <= timeslot[time%numTimeSlots].cores) && (openJobs[i].runTimeEstimate+time <= endTime)){
					endTime = max(endTime,time+openJobs[i].runTime);
					endTimeForJob = time+openJobs[i].runTime;
					#ifndef SILENT
					printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",openJobs[i].ID, time,openJobs[i].numProc, timeslot[time%numTimeSlots].cores,openJobs[i].runTime, endTimeForJob);
					#endif
					allocate(time,timeslot,openJobs[i].runTime, openJobs[i].numProc);

					slowDown[openJobs[i].ID] = (time - openJobs[i].submitTime + openJobs[i].runTime) / openJobs[i].runTime;
					waitTime[openJobs[i].ID] = time - openJobs[i].submitTime;
					turnAroundTime[openJobs[i].ID] = time - openJobs[i].submitTime + time + openJobs[i].runTime;

					openJobs.erase(openJobs.begin()+i);
					i--;
				}
				//Need to check if this job can fit and not hamper the priority job.
				else if(openJobs[i].numProc <= timeslot[time%numTimeSlots].cores && openJobs[i].numProc < futureProcsRemaining){
					//I can fit and not interfere!
					//Tell the scheduler how much cores are left...
					futureProcsRemaining -= openJobs[i].numProc;
					endTime = max(endTime,time+openJobs[i].runTime);
					endTimeForJob = time+openJobs[i].runTime;
					#ifndef SILENT
					printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",openJobs[i].ID, time,openJobs[i].numProc, timeslot[time%numTimeSlots].cores,openJobs[i].runTime, endTimeForJob);
					#endif
					allocate(time,timeslot,openJobs[i].runTime, openJobs[i].numProc);

					slowDown[openJobs[i].ID] = (time - openJobs[i].submitTime + openJobs[i].runTime) / openJobs[i].runTime;
					waitTime[openJobs[i].ID] = time - openJobs[i].submitTime;
					turnAroundTime[openJobs[i].ID] = time - openJobs[i].submitTime + openJobs[i].runTime;

					openJobs.erase(openJobs.begin()+i);
					i--;
				}
			}
		}
		time += 1;
	}
	return endTime;
}

vector<Proc> sptHeuristic(vector<Proc> & openJobs){
	Proc x;
	int i,j;
	int n = openJobs.size();

	//sort the jobs in terms of weight, least to greatest. Insertion sort
	for(i =1; i<n-1; i++){
		x = openJobs[i];
		j = i;
		while((j>0) && (openJobs[j-1].runTimeEstimate > x.runTimeEstimate)){
			openJobs[j] = openJobs[j-1];
			j--;
		}
		openJobs[j] = x;
	}
	return openJobs;
}


long makeSPT(Proc *queue, Slot* timeslot,   map<int,long>& slowDown, map<int,long>& waitTime, map<int,long>& turnAroundTime) {
	long time=0;
	long endTime = 0;
	long endTimeForJob = 0;
	int i=0;
	vector<long> current_procs;
	vector<Proc> openJobs;
	int queuePosition = 0;
	//While we have processes to schedule...
	while (true) {


		if(queuePosition >= NUM_ENTRIES_TO_PROCESS && time > endTime){
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
			openJobs = sptHeuristic(openJobs);

		}


		//Got some openjobs, time to start scheduling with backfill
		for(i=0; i<openJobs.size(); i++){
			if(openJobs[i].numProc <= timeslot[time%numTimeSlots].cores){//I fit, time to allocate
				endTime = max(endTime,time+openJobs[i].runTime);
				endTimeForJob = time+openJobs[i].runTime;
				#ifndef SILENT
				printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",openJobs[i].ID, time,openJobs[i].numProc, timeslot[time%numTimeSlots].cores,openJobs[i].runTime, endTimeForJob);
				#endif
				allocate(time,timeslot,openJobs[i].runTime, openJobs[i].numProc);

				slowDown[openJobs[i].ID] = (time - openJobs[i].submitTime + openJobs[i].runTime) / openJobs[i].runTime;
				waitTime[openJobs[i].ID] = time - openJobs[i].submitTime ;
				turnAroundTime[openJobs[i].ID] = time - openJobs[i].submitTime + openJobs[i].runTime;

				openJobs.erase(openJobs.begin()+i);
				i--;
			}
			//else continue since I can't fit this one but I might be able to fit the next one.
		}

		time += 1;
	}
	return endTime;
}

vector<Proc> lptHeuristic(vector<Proc> & openJobs){
	Proc x;
	int i,j;
	int n = openJobs.size();

	//sort the jobs in terms of weight, greatest to least. Insertion sort
	for(i =1; i<n-1; i++){
		x = openJobs[i];
		j = i;
		while((j>0) && (openJobs[j-1].runTimeEstimate > x.runTimeEstimate)){
			openJobs[j] = openJobs[j-1];
			j--;
		}
		openJobs[j] = x;
	}

	//reverse array
	j= n-1;
	for(i=0;i<n/2;i++){
		x = openJobs[i];
		openJobs[i] = openJobs[j];
		openJobs[j] = x;
		j--;
	}
	return openJobs;

}


long makeLPT(Proc *queue, Slot* timeslot,   map<int,long>& slowDown, map<int,long>& waitTime, map<int,long>& turnAroundTime) {
	long time=0;
	long endTime = 0;
	long endTimeForJob = 0;
	int i=0;
	vector<long> current_procs;
	vector<Proc> openJobs;
	int queuePosition = 0;
	//While we have processes to schedule...
	while (true) {


		if(queuePosition >= NUM_ENTRIES_TO_PROCESS && time > endTime){
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
			openJobs = lptHeuristic(openJobs);

		}


		//Got some openjobs, time to start scheduling with backfill
		for(i=0; i<openJobs.size(); i++){
			if(openJobs[i].numProc <= timeslot[time%numTimeSlots].cores){//I fit, time to allocate
				endTime = max(endTime,time+openJobs[i].runTime);
				endTimeForJob = time+openJobs[i].runTime;
				#ifndef SILENT
				printf("ptJob %i @ t %li\t c_reqd: %i\tc_avl: %i\t%li \tEnd:%li\n",openJobs[i].ID, time,openJobs[i].numProc, timeslot[time%numTimeSlots].cores,openJobs[i].runTime, endTimeForJob);
				#endif
				allocate(time,timeslot,openJobs[i].runTime, openJobs[i].numProc);

				slowDown[openJobs[i].ID] = (time - openJobs[i].submitTime + openJobs[i].runTime) / openJobs[i].runTime;
				waitTime[openJobs[i].ID] = time - openJobs[i].submitTime ;
				turnAroundTime[openJobs[i].ID] = time - openJobs[i].submitTime + openJobs[i].runTime;

				openJobs.erase(openJobs.begin()+i);
				i--;
			}
			//else continue since I can't fit this one but I might be able to fit the next one.
		}

		time += 1;
	}
	return endTime;
}


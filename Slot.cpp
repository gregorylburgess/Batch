Slot::Slot(int coreCount, int numProcesses) {
cores = coreCount;
events = new Event[2*numProcesses];
numEvents = 0;
}

void Slot::addEvent(Event *event) {
events[numEvents] = event;
numEvents ++;
}

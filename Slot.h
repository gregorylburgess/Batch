struct Slot {
  int cores;
  int numEvents;
  Event *events;

  Slot(int coreCount, int numProcesses);
  void addEvent(Event *event);
};


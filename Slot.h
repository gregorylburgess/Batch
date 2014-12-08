#include "Event.h"
struct Slot {
  int cores;
  int numEvents;
  Event *events;

  void init(int coreCount, int numProcesses);
  void addEvent(Event *event);
};


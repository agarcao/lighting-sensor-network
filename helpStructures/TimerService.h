#ifndef CASTALIA_TIMER
#define CASTALIA_TIMER

#define TIMER_MIN_SIZE 8
#define TIMER_MAX_SIZE 256

#include <omnetpp.h>
#include <vector>

#include "WSNMessages.h"
#include "TimerServiceMessage_m.h"

class TimerService: public virtual cSimpleModule {
 private:
	double timerDrift;
 protected:
	std::vector<TimerServiceMessage*> timerMessages;

	simtime_t getClock();
	void setTimerDrift(double new_drift);
	void setTimer(int index, simtime_t time);
	simtime_t getTimer(int index);
	void cancelTimer(int index);
	void handleTimerMessage(cMessage *);
	virtual void timerFiredCallback(int index);
};

#endif

#ifndef _NOMOBILITYMANAGER_H_
#define _NOMOBILITYMANAGER_H_

#include <omnetpp.h>
#include "DebugInfoWriter.h"
#include "VirtualMobilityManager.h"

using namespace std;

class NoMobilityManager: public VirtualMobilityManager {
 protected:
    void initialize();
	void handleMessage(cMessage * msg);
};

#endif

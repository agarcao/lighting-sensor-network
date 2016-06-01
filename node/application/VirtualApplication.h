#ifndef VIRTUALAPPLICATIONMODULE
#define VIRTUALAPPLICATIONMODULE

#include <sstream>
#include <string>
#include <omnetpp.h>
#include "ApplicationPacket_m.h"
#include "SensorManagerMessage_m.h"
#include "ResourceManager.h"
#include "Radio.h"
#include "VirtualMobilityManager.h"
#include "CastaliaModule.h"
#include "TimerService.h"

#define SELF_NETWORK_ADDRESS selfAddress.c_str()

using namespace std;

class VirtualApplication: public CastaliaModule, public TimerService {
 protected:
	/*--- The .ned file's parameters ---*/
	string applicationID;
	int priority;
	int packetHeaderOverhead;
	int constantDataPayload;
	bool isSink;
	double latencyMax;
	int latencyBuckets;

	/*--- Custom class parameters ---*/
	int self;								// the node's ID
	string selfAddress;						// the node's routing level address
	ResourceManager *resMgrModule;			// a pointer to the Resource Manager module
	VirtualMobilityManager *mobilityModule;	// a pointer to the mobility Manager module
	Radio *radioModule;						// a pointer to the Radio module
	bool disabled;
	double cpuClockDrift;

	virtual void initialize();
	virtual void startup() {}
	virtual void handleMessage(cMessage * msg);
	virtual void finish();
	virtual void finishSpecific() {}

	void requestSensorReading(int index = 0);
	void toNetworkLayer(cMessage *);
	void toNetworkLayer(cPacket *, const char *);

	ApplicationPacket *createGenericDataPacket(double, unsigned int, int = -1);
	virtual void fromNetworkLayer(ApplicationPacket *, const char *, double, double) = 0;
	virtual void handleSensorReading(SensorReadingMessage *) {}
	virtual void handleNetworkControlMessage(cMessage *) {}
	virtual void handleMacControlMessage(cMessage *) {}
	virtual void handleRadioControlMessage(RadioControlMessage *) {}
	virtual void handleSelfEvent() {}
};

#endif

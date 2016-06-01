#ifndef VIRTUALMACMODULE
#define VIRTUALMACMODULE

#define SELF_MAC_ADDRESS self

#include <queue>
#include <vector>
#include <omnetpp.h>

#include "RoutingPacket_m.h"
#include "MacPacket_m.h"
#include "RadioControlMessage_m.h"

#include "ResourceManager.h"
#include "Radio.h"
#include "TimerService.h"
#include "CastaliaModule.h"

using namespace std;

class VirtualMac: public CastaliaModule, public TimerService {
 protected:
	int self;		// the node's ID
	bool disabled;
	int macBufferSize;
	int macMaxFrameSize;
	int macFrameOverhead;
	unsigned int currentSequenceNumber;

	//a pointer to the object of the Radio Module (used for direct method calls)
	Radio *radioModule;

	//a pointer to the object of the Resource Manager Module (used for direct method calls)
	ResourceManager *resMgrModule;

	queue< cPacket* > TXBuffer;
	vector< unsigned int > pktHistory;

	void createAndSendRadioControlCommand(RadioControlCommand_type, double, const char *, BasicState_type);
	void initialize();
	void handleMessage(cMessage * msg);
	void finish();
	virtual void finishSpecific() {}
	virtual void startup() {}

	void toNetworkLayer(cMessage *);
	void toRadioLayer(cMessage *);
	virtual void fromNetworkLayer(cPacket *, int) = 0;
	virtual void fromRadioLayer(cPacket *, double, double) = 0;
	int bufferPacket(cPacket *);
	bool isNotDuplicatePacket(cPacket *);

	virtual int handleControlCommand(cMessage * msg);
	virtual int handleRadioControlMessage(cMessage * msg);

	void encapsulatePacket(cPacket *, cPacket *);
	cPacket *decapsulatePacket(cPacket *);
};

#endif

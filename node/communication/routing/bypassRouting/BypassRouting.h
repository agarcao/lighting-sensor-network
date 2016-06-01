#ifndef _BYPASSROUTING_H_
#define _BYPASSROUTING_H_

#include "VirtualRouting.h"
#include "BypassRoutingPacket_m.h"

using namespace std;

class BypassRouting: public VirtualRouting {
 protected:
	void fromApplicationLayer(cPacket *, const char *);
	void fromMacLayer(cPacket *, int, double, double);
};

#endif				//BYPASSROUTINGMODULE

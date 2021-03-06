#include "VirtualRouting.h"
void VirtualRouting::initialize()
{
	maxNetFrameSize = par("maxNetFrameSize");
	netDataFrameOverhead = par("netDataFrameOverhead");
	netBufferSize = par("netBufferSize");

	/* Get a valid references to the Resources Manager module and the
	 * Radio module, so that we can make direct calls to their public methods
	 */
	radioModule = check_and_cast <Radio*>(getParentModule()->getSubmodule("Radio"));
	resMgrModule = check_and_cast <ResourceManager*>(getParentModule()->getParentModule()->getSubmodule("ResourceManager"));

	if (!resMgrModule || !radioModule)
		opp_error("\n Virtual Routing init: Error in geting a valid reference module(s).");

	self = getParentModule()->getParentModule()->getIndex();
	// create the routing level address using self
	stringstream out; out << self; 	selfAddress = out.str();

	cpuClockDrift = resMgrModule->getCPUClockDrift();
	setTimerDrift(cpuClockDrift);
	pktHistory.clear();

	disabled = true;
	currentSequenceNumber = 0;

	declareOutput("Buffer overflow");
}

// A function to send control messages to lower layers
void VirtualRouting::toMacLayer(cMessage * msg)
{
	if (msg->getKind() == NETWORK_LAYER_PACKET)
		opp_error("toMacLayer() function used incorrectly to send NETWORK_LAYER_PACKET without destination MAC address");
	send(msg, "toMacModule");
}

// A function to send packets to MAC, requires a destination address
void VirtualRouting::toMacLayer(cPacket * pkt, int destination)
{
	RoutingPacket *netPacket = check_and_cast <RoutingPacket*>(pkt);
	netPacket->getNetMacInfoExchange().nextHop = destination;
	send(netPacket, "toMacModule");
	ev << "send to mac layer" << endl;
}

void VirtualRouting::toApplicationLayer(cMessage * msg)
{
    ev << "[Node #" << this->getParentModule()->getParentModule()->getIndex() << "::VirtualMac::toNetworkLayer] Segue para a Application Module" << endl;
	send(msg, "toCommunicationModule");
}

void VirtualRouting::encapsulatePacket(cPacket * pkt, cPacket * appPkt)
{
	RoutingPacket *netPkt = check_and_cast <RoutingPacket*>(pkt);
	// set the size to just the overhead. encapsulate(appPkt)
	// will add the size of the app packet automatically
	netPkt->setByteLength(netDataFrameOverhead);
	netPkt->setKind(NETWORK_LAYER_PACKET);
	netPkt->setSource(SELF_NETWORK_ADDRESS);
	netPkt->setSequenceNumber(currentSequenceNumber++);
	netPkt->encapsulate(appPkt);
}

cPacket* VirtualRouting::decapsulatePacket(cPacket * pkt)
{
	RoutingPacket *netPkt = check_and_cast <RoutingPacket*>(pkt);
	ApplicationPacket *appPkt = check_and_cast <ApplicationPacket*>(netPkt->decapsulate());

	appPkt->getAppNetInfoExchange().RSSI = netPkt->getNetMacInfoExchange().RSSI;
	appPkt->getAppNetInfoExchange().LQI = netPkt->getNetMacInfoExchange().LQI;
	appPkt->getAppNetInfoExchange().source = netPkt->getSource();
	return appPkt;
}

void VirtualRouting::handleMessage(cMessage * msg)
{
	int msgKind = msg->getKind();
	if (disabled && msgKind != NODE_STARTUP)
	{
		delete msg;
		return;
	}

	switch (msgKind) {

		case NODE_STARTUP:
		{
			disabled = false;
			send(new cMessage("Network --> Mac startup message", NODE_STARTUP), "toMacModule");
			startup();
			break;
		}

		/** Packet that come from application */
		case APPLICATION_PACKET:
		{
			ApplicationPacket *appPacket = check_and_cast <ApplicationPacket*>(msg);
			if (maxNetFrameSize > 0 && maxNetFrameSize < appPacket->getByteLength() + netDataFrameOverhead)
			{
				trace() << "Oversized packet dropped. Size:" << appPacket->getByteLength() <<
				    ", Network layer overhead:" << netDataFrameOverhead <<
				    ", max Network packet size:" << maxNetFrameSize;
				break;
			}
			trace() << "Received [" << appPacket->getName() << "] from application layer";
			ev << "Received [" << appPacket->getName() << "] from application layer" << endl;

			/* Control is now passed to a specific routing protocol by calling fromApplicationLayer()
			 * Notice that after the call we RETURN (not BREAK) so that the packet is not deleted.
			 * This is done since the packet will most likely be encapsulated and forwarded to the
			 * MAC layer. If the protocol specific function wants to discard the packet is has
			 * to delete it.
			 */
			fromApplicationLayer(appPacket, appPacket->getAppNetInfoExchange().destination.c_str());
			return;
		}

		/** Packet from network. Let's pass to the application */
		case NETWORK_LAYER_PACKET:
		{
		    ev << "[Node #" << this->getParentModule()->getParentModule()->getIndex() << "::VirtualRouting::handleMessage::NETWORK_LAYER_PACKET] Recebi msg do MAC" << endl;
			RoutingPacket *netPacket = check_and_cast <RoutingPacket*>(msg);
			trace() << "Received [" << netPacket->getName() << "] from MAC layer";
			NetMacInfoExchange_type info = netPacket->getNetMacInfoExchange();

			/* Control is now passed to a specific routing protocol by calling fromMacLayer()
			 * Notice that after the call we BREAK so that the NET packet gets deleted.
			 * This will not delete the encapsulated APP packet if it gets decapsulated
			 * by fromMacLayer(), i.e., the normal/expected action.
			 */
			fromMacLayer(netPacket, info.lastHop, info.RSSI, info.LQI);
			ev << "[Node #" << this->getParentModule()->getParentModule()->getIndex() << "::VirtualRouting::handleMessage::NETWORK_LAYER_PACKET] Fim de tratar msg" << endl;
			break;
		}

		case TIMER_SERVICE:{
			handleTimerMessage(msg);
			break;
		}

		case MAC_CONTROL_MESSAGE:{
			handleMacControlMessage(msg);
			return; // msg not deleted
		}

		case RADIO_CONTROL_MESSAGE:{
			handleRadioControlMessage(msg);
			return; // msg not deleted
		}

		case MAC_CONTROL_COMMAND:{
			toMacLayer(msg);
			return; // msg not deleted
		}

		case RADIO_CONTROL_COMMAND:{
			toMacLayer(msg);
			return; // msg not deleted
		}

		case NETWORK_CONTROL_COMMAND:{
			handleNetworkControlCommand(msg);
			break;
		}

		case OUT_OF_ENERGY:{
			disabled = true;
			break;
		}

		case DESTROY_NODE:{
			disabled = true;
			break;
		}

		default:{
			opp_error("Network module recieved unexpected message: [%s]", msg->getName());
		}
	}

	delete msg;
}

// handleMacControlMessage needs to either process and DELETE the message OR forward it
void VirtualRouting::handleMacControlMessage(cMessage * msg)
{
	toApplicationLayer(msg);
}

// handleRadioControlMessage needs to either process and DELETE the message OR forward it
void VirtualRouting::handleRadioControlMessage(cMessage * msg)
{
	toApplicationLayer(msg);
}

void VirtualRouting::finish()
{
	CastaliaModule::finish();
	cPacket *pkt;
	// clear the buffer from all remaining packets
	while (!TXBuffer.empty()) {
		pkt = TXBuffer.front();
		TXBuffer.pop();
		cancelAndDelete(pkt);
	}
}

int VirtualRouting::bufferPacket(cPacket * rcvFrame)
{
	if ((int)TXBuffer.size() >= netBufferSize) {
		collectOutput("Buffer overflow");
		cancelAndDelete(rcvFrame);
		return 0;
	} else {
		TXBuffer.push(rcvFrame);
		trace() << "Packet buffered from application layer, buffer state: " <<
				TXBuffer.size() << "/" << netBufferSize;
		return 1;
	}
}

int VirtualRouting::resolveNetworkAddress(const char *netAddr)
{
	if (!netAddr[0] || netAddr[0] < '0' || netAddr[0] > '9')
		return BROADCAST_MAC_ADDRESS;
	return atoi(netAddr);
}

bool VirtualRouting::isNotDuplicatePacket(cPacket * pkt)
{
	//extract source address and sequence number from the packet
	RoutingPacket *netPkt = check_and_cast <RoutingPacket*>(pkt);
	int src = resolveNetworkAddress(netPkt->getSource());
	unsigned int sn = netPkt->getSequenceNumber();

	//resize packet history vector if necessary
	if (src >= (int)pktHistory.size())
		pktHistory.resize(src+1);

	//search for this sequence number in the list, corresponding to address 'src'
	list<unsigned int>::iterator it1;
	for (it1 = pktHistory[src].begin(); it1 != pktHistory[src].end(); it1++) {
		//if such sequence number is found, packet is duplicate
		if (*it1 == sn) return false;
	}

	//no such sequence number found, this is new packet
	pktHistory[src].push_front(sn);
	if (pktHistory[src].size() > PACKET_HISTORY_SIZE)
		pktHistory[src].pop_back();
	return true;
}

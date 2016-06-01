#include "BypassMAC.h"

Define_Module(BypassMAC);

/* Handle packet received from upper (network) layer.
 * We need to create a MAC packet, (here it can just be the generic MacPacket)
 * and encapsulate the received network packet before forwarding it to RadioLayer
 */
void BypassMAC::fromNetworkLayer(cPacket * pkt, int destination)
{
	MacPacket *macFrame = new MacPacket("BypassRouting packet", MAC_LAYER_PACKET);
	encapsulatePacket(macFrame, pkt);
	macFrame->setSource(SELF_MAC_ADDRESS);
	macFrame->setDestination(destination);
	ev << "send to radio layer the MAC_LAYER_PACKET" << endl;
	toRadioLayer(macFrame);
	ev << "send to radio layer the MAC_LAYER_PACKET" << endl;
	toRadioLayer(createRadioCommand(SET_STATE, TX));
}

/* Handle packet received from lower (radio) layer.
 * We accept packets from all MAC protocols (cast to the base class MacPacket)
 * Then we filter by the destination field. By default we set the generic
 * destination field to broadcast when we encapsulate a NET packet. If a
 * specific protocol does not change that field then bypassMAC will be
 * operating in a promiscuous mode.
 */
void BypassMAC::fromRadioLayer(cPacket * pkt, double rssi, double lqi)
{
	MacPacket *macPkt = dynamic_cast <MacPacket*>(pkt);
	if (macPkt == NULL)
		return;
	ev << "[Node #" << this->getParentModule()->getParentModule()->getIndex() << "::BypassMAC::fromRadioLayer] Cheguei aqui" << endl;
	if (macPkt->getDestination() == SELF_MAC_ADDRESS ||
	    macPkt->getDestination() == BROADCAST_MAC_ADDRESS)
	{
	    ev << "[Node #" << this->getParentModule()->getParentModule()->getIndex() << "::BypassMAC::fromRadioLayer] Vou enviar para o toNetworkLayer" << endl;
		toNetworkLayer(decapsulatePacket(macPkt));
	}
}


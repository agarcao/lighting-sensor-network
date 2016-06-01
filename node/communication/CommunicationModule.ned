package wsn_lighting.node.communication;

module CommunicationModule {
 parameters:
 	string MACProtocolName = default ("BypassMAC");
	string RoutingProtocolName = default ("BypassRouting");

 gates:
 	output toApplicationModule;
	output toNodeContainerModule;
	input fromApplicationModule;
	input fromNodeContainerModule;
	input fromResourceManager2Net;
	input fromResourceManager2Mac;
	input fromResourceManager2Radio;

 submodules:
 	Radio: wsn_lighting.node.communication.radio.Radio;
	MAC: <MACProtocolName> like wsn_lighting.node.communication.mac.iMac;
	Routing: <RoutingProtocolName> like wsn_lighting.node.communication.routing.iRouting;

 connections:
 	fromApplicationModule --> Routing.fromCommunicationModule;
	Routing.toCommunicationModule --> toApplicationModule;
	Routing.toMacModule --> MAC.fromNetworkModule;
	MAC.toNetworkModule --> Routing.fromMacModule;
	MAC.toRadioModule --> Radio.fromMacModule;
	Radio.toMacModule --> MAC.fromRadioModule;
	
	fromNodeContainerModule --> Radio.fromCommunicationModule;
	Radio.toCommunicationModule --> toNodeContainerModule;

	fromResourceManager2Net --> Routing.fromCommModuleResourceMgr;
	fromResourceManager2Mac --> MAC.fromCommModuleResourceMgr;
	fromResourceManager2Radio --> Radio.fromCommModuleResourceMgr;
}


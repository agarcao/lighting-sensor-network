package wsn_lighting.node.communication.mac;

moduleinterface iMac {
 parameters:
 	bool collectTraceInfo;
	int macMaxPacketSize;	// in bytes
	int macBufferSize;		// in number of messages
	int macPacketOverhead;

 gates: 
 	output toNetworkModule;
	output toRadioModule;
	input fromNetworkModule;
	input fromRadioModule;
	input fromCommModuleResourceMgr;
}


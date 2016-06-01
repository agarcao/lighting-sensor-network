package wsn_lighting.node.communication.routing;

moduleinterface iRouting {
 parameters:
	bool collectTraceInfo;
	int maxNetFrameSize;		// in bytes
	int netDataFrameOverhead;	// in bytes
	int netBufferSize;			// in number of messages

 gates: 
	output toCommunicationModule;
	output toMacModule;
	input fromCommunicationModule;
	input fromMacModule;
	input fromCommModuleResourceMgr;
}


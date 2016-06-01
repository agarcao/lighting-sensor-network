package wsn_lighting.node.application;

// The sensor node module. Connects to the wireless channel in order to communicate 
// with other nodes. Connects to psysical processes so it can sample them.

moduleinterface iApplication {
     parameters:
    	string applicationID;
    	bool collectTraceInfo;
    	int priority;
    	int packetHeaderOverhead;	// in bytes
    	int constantDataPayload;	// in bytes
    	
    	bool coneLightingIsActive;
    	int radiousLighting;
    
     gates: 
     	output toCommunicationModule;
    	output toSensorDeviceManager;
    	output toResourceManager;
    	input fromCommunicationModule;
    	input fromSensorDeviceManager;
    	input fromResourceManager;
}


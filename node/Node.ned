package wsn_lighting.node;

// The sensor node module. Connects to the wireless channel in order to communicate 
// with other nodes. Connects to psysical processes so it can sample them.

module Node {
 	parameters:
     	//node location is defined by five parameters below
    	double xCoor = default (0);
    	double yCoor = default (0);
    	double zCoor = default (0);
    	double phi = default (0);
    	double theta = default (0);
    	
    	double startupOffset = default (0);							//node startup offset (i.e. delay), in seconds 
    	double startupRandomization = default (0.05);				//node startup randomisation, in seconds
    	// Node will become active startupOffset + random(startupRandomization) 
    	// seconds after the start of simulation
    
    	string ApplicationName = default ("WsnLogic");		//the name of the implemented Application Module
    	string MobilityManagerName = default ("NoMobilityManager");	//the name of the implemented Mobility Module
    	
    	// This config came from SensorNetwork.ned
    	int sensor_radius;
    	int numHorizontalCells;
   		int numVerticalCells;
   		
   		bool nearObstacle = default(false); // Diz-nos se o nó está perto dum obstaculo ou não (Tem de estar aqui pois tanto o WSNLogic como o ResourceManager precisa disto)
   		
 	gates:
    	output toWirelessChannel;
    	output toPhysicalProcess[];
    	input fromWirelessChannel;
    	input fromPhysicalProcess[];

 	submodules:
     	MobilityManager: <MobilityManagerName> like wsn_lighting.node.mobilityManager.iMobilityManager;
    	ResourceManager: wsn_lighting.node.resourceManager.ResourceManager;
    	SensorManager: wsn_lighting.node.sensorManager.SensorManager 
    	{
        	 gates:
        		fromNodeContainerModule[sizeof(toPhysicalProcess)];
        		toNodeContainerModule[sizeof(toPhysicalProcess)];
    	} 
    	Communication: wsn_lighting.node.communication.CommunicationModule;
    	Application: <ApplicationName> like wsn_lighting.node.application.iApplication;

 	connections:
    	Communication.toNodeContainerModule --> toWirelessChannel;
    	fromWirelessChannel --> Communication.fromNodeContainerModule;
    	Application.toCommunicationModule --> Communication.fromApplicationModule;
    	Application.toSensorDeviceManager --> SensorManager.fromApplicationModule;
    	Communication.toApplicationModule --> Application.fromCommunicationModule;
    	SensorManager.toApplicationModule --> Application.fromSensorDeviceManager;

    	for i = 0..sizeof(toPhysicalProcess) - 1 {
    		fromPhysicalProcess[i] --> SensorManager.fromNodeContainerModule[i];
    		SensorManager.toNodeContainerModule[i] --> toPhysicalProcess[i];
    	}

    	ResourceManager.toSensorDevManager --> SensorManager.fromResourceManager;
    	ResourceManager.toApplication --> Application.fromResourceManager;
    	ResourceManager.toNetwork --> Communication.fromResourceManager2Net;
    	ResourceManager.toMac --> Communication.fromResourceManager2Mac;
    	ResourceManager.toRadio --> Communication.fromResourceManager2Radio;
    	Application.toResourceManager --> ResourceManager.fromApplication;
}


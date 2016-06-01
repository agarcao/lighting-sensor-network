package wsn_lighting;

network SensorNetwork {
	
	parameters:
    	int field_z = default (0);			// the height of the deployment field (2-D field by default)
    
    	int numNodes;						// the number of sensor nodes
    	int numPersons;						// the number of person nodes
    	int numObstacles;					// the number of obstacles
    
    	string deployment = default ("");
    
    	int numPhysicalProcesses = default (1);
    	string physicalProcessName = default ("PersonsPhysicalProcess");
    	
    	string debugInfoFileName = default ("Castalia-Trace.txt");
    	
    	int horizontal_cells = default(10);
    	int vertical_cells = default(10);
    	int cell_size = default(75);
    	
    	// Argumentos necessários para a criação do mapa
        int field_x = horizontal_cells * cell_size; // the length of the deployment field
        int field_y = vertical_cells * cell_size; // the width of the deployment field
        
        string obstaclesName = default ("Wall");
    	
    	// Com argumentos vindos do omnet.ini
        @display("bgb=$field_x,$field_y,#FFF;bgg=$cell_size,1;");

     submodules:	
		obstacle[numObstacles]: <obstaclesName> like wsn_lighting.obstacles.iObstacle
    	{
    	}

    	physicalProcess[numPhysicalProcesses]: <physicalProcessName> like wsn_lighting.physicalProcess.iPhysicalProcess
    	{
    	    parameters:
    	        @display("i=block/circle_vs;p=-20,-20;");
    		gates:
        		toNode[numNodes];
        		fromNode[numNodes];
        		fromPerson[numPersons];
    	}

    	sensorNode[numNodes]:wsn_lighting.node.Node {
    	    parameters:
    	        sensor_radius = default(50);
    	        
    	        numHorizontalCells = horizontal_cells;
    	        numVerticalCells = vertical_cells;
    	        
    	        @display("i=status/off_25;is=vs;r=$sensor_radius,,#8080FF;p=50,50;");
        	gates:
        		toPhysicalProcess[numPhysicalProcesses];
        		fromPhysicalProcess[numPhysicalProcesses];
    	}
    	
    	person[numPersons]:wsn_lighting.personNode.PersonNode
    	{
    		parameters:
    		    field_x = field_x;
    		    field_y = field_y;
    		    
    	        @display("i=block/user_vs;is=vs;");
	        gates:        		
        		toPhysicalProcess[numPhysicalProcesses];
    	}   	
    	
    	wirelessChannel:wsn_lighting.wirelessChannel.WirelessChannel 
    	{
    	    parameters:
    	        @display("i=block/circle_vs;p=$field_x,-20;");
    	 	gates:
        		toNode[numNodes];
        		fromNode[numNodes];
    	} 

 connections:
	for i = 0..numNodes - 1 {
		sensorNode[i].toWirelessChannel --> { @display("ls=,0"); } --> wirelessChannel.fromNode[i];
		sensorNode[i].fromWirelessChannel <-- { @display("ls=,0"); } <-- wirelessChannel.toNode[i];
	}

	for i = 0..numNodes - 1, for j = 0..numPhysicalProcesses - 1 {
		sensorNode[i].toPhysicalProcess[j] --> { @display("ls=,0"); } --> physicalProcess[j].fromNode[i];
		sensorNode[i].fromPhysicalProcess[j] <-- { @display("ls=,0"); } <-- physicalProcess[j].toNode[i];
	}
	
	for i = 0..numPersons - 1, for j = 0..numPhysicalProcesses - 1 {
		person[i].toPhysicalProcess[j] --> { @display("ls=,0"); } --> physicalProcess[j].fromPerson[i];
	}	
}

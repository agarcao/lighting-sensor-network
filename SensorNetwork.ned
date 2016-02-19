//********************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2010                             *
//*  Developed at the ATP lab, Networked Systems research theme                  *
//*  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis, Yuriy Tselishchev  *
//*  This file is distributed under the terms in the attached LICENSE file.      *
//*  If you do not find this file, copies can be found by writing to:            *
//*                                                                              *
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia                 *
//*      Attention:  License Inquiry.                                            *
//*                                                                              *
//*******************************************************************************/
package wsn_lighting;

network SensorNetwork {
	
	parameters:
    	int field_z = default (0);			// the height of the deployment field (2-D field by default)
    
    	int numNodes;						// the number of sensor nodes
    	int numPersons;						// the number of person nodes
    
    	string deployment = default ("");
    
    	int numPhysicalProcesses = default (1);
    	string physicalProcessName = default ("PersonsPhysicalProcess");
    	string debugInfoFileName = default ("Castalia-Trace.txt");
    	
    	int horizontal_cells = default(2);
    	int vertical_cells = default(2);
    	int cell_size = default(75);
    	
    	// Argumentos necessários para a criação do mapa
        int field_x = horizontal_cells * cell_size; // the length of the deployment field
        int field_y = vertical_cells * cell_size; // the width of the deployment field
    	
    	// Com argumentos vindos do omnet.ini
        @display("bgb=$field_x,$field_y,#FFF;bgg=$cell_size,1;");

     submodules:
    	wirelessChannel:wsn_lighting.wirelessChannel.WirelessChannel 
    	{
    	    parameters:
    	        @display("i=block/circle_vs,blue;p=$field_x,-20;");
    	 	gates:
        		toNode[numNodes];
        		fromNode[numNodes];
    	} 

    	physicalProcess[numPhysicalProcesses]: <physicalProcessName> like wsn_lighting.physicalProcess.iPhysicalProcess
    	{
    	    parameters:
    	        @display("i=block/circle_vs,blue;p=-20,-20;");
    		gates:
        		toNode[numNodes];
        		fromNode[numNodes];
        		fromPerson[numPersons];
    	}

    	sensorNode[numNodes]:wsn_lighting.node.Node {
    	    parameters:
    	        sensor_radius = default(50);
    	        
    	        @display("i=status/off,blue,0;is=vs;r=$sensor_radius,,#8080FF;p=50,50;");
        	gates:
        		toPhysicalProcess[numPhysicalProcesses];
        		fromPhysicalProcess[numPhysicalProcesses];
    	}
    	
    	person[numPersons]:wsn_lighting.personNode.PersonNode
    	{
    		parameters:
    		    field_x = field_x;
    		    field_y = field_y;
    		    
    	        @display("i=block/user_vs,blue;is=vs;");
	        gates:        		
        		toPhysicalProcess[numPhysicalProcesses];
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

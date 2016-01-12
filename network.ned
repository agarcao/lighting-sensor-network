// @author André Garção
// @version 1.0

package wsn_lighting;

simple statisticsNode
{
    parameters:
        int numNodes;
}

simple personNode
{
    parameters:
        int id;
        
        int fieldX;
        int fieldY;
        
        int guiX;
        int guiY;
}

simple sensorNode
{
	parameters:
	    int id;
	    
	   	int fieldX;
        int fieldY;
        
        int guiX;
        int guiY;
        
        int lightingRadius;
        int sensorRadius;
}

network LightingSensorNetwork
{
    parameters:
        int field_x;
        int field_y;
        
        int numSensorNodes;
        int numPersonNodes;

		// Com argumentos vindos do omnet.ini
        @display("bgb=$field_x,$field_y,#FFF;bgg=20,1;");
        
    submodules:
        statistics: statisticsNode {
            parameters:
                numNodes = numSensorNodes;
                
                @display("i=block/circle_vs,blue;p=$field_x,-20");
        }

        person1: personNode {
            parameters:
                id = 1;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                @display("i=block/user_vs,blue;is=vs;");
        }
        
        person2: personNode {
            parameters:
                id = 2;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                @display("i=block/user_vs,blue;is=vs;");
        }
        
        sensor1: sensorNode {
            parameters:
                id = 1;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius;");
        }
        sensor2: sensorNode {
            parameters:
                id = 1;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius;");
        }
        sensor3: sensorNode {
            parameters:
                id = 1;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius;");
        }
        sensor4: sensorNode {
            parameters:
                id = 1;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius;");
        }
        sensor5: sensorNode {
            parameters:
                id = 1;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius;");
        }
        sensor6: sensorNode {
            parameters:
                id = 1;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius;");
        }
}

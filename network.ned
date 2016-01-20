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
        int horizontalCells;
        int verticalCells;
        int cellSize;
                
        int numSensorNodes;
        int numPersonNodes;
        
        int field_x = horizontalCells * cellSize;
        int field_y = verticalCells * cellSize;

		// Com argumentos vindos do omnet.ini
        @display("bgb=$field_x,$field_y,#000;bgg=$cellSize,1;");
        
    submodules:
        statistics: statisticsNode {
            parameters:
                numNodes = numSensorNodes;
                
                @display("i=block/circle_vs,blue;p=$field_x,-20;b=50,50,rect,#FFF,#FFF");
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
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius,,#8080FF;");
        }
       sensor2: sensorNode {
            parameters:
                id = 2;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius,,#8080FF;");
        }
        sensor3: sensorNode {
            parameters:
                id = 3;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius,,#8080FF;");
        }
        sensor4: sensorNode {
            parameters:
                id = 4;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius,,#8080FF;");
        }
        sensor5: sensorNode {
            parameters:
                id = 5;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius,,#8080FF;");
        }
        sensor6: sensorNode {
            parameters:
                id = 6;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                lightingRadius = 100;
        		sensorRadius = 50;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius,,#8080FF;");
        }
}

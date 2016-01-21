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
        
        int horizontalCells;
        int verticalCells;
        int cellSize;
        
        int lightingRadius;
        int sensorRadius;
}

network LightingSensorNetwork
{
    parameters:
        // Argumentos vindos do omnetpp.ini
        int horizontal_cells;
        int vertical_cells;
        int cell_size;
                
        int numSensorNodes;
        int numPersonNodes;
        
        // Argumentos necessários para a criação do mapa
        int fieldXSize = horizontal_cells * cell_size;
        int fieldYSize = vertical_cells * cell_size;

		// Com argumentos vindos do omnet.ini
        @display("bgb=$fieldXSize,$fieldYSize,#000;bgg=$cell_size,1;");
        
    submodules:
        statistics: statisticsNode {
            parameters:
                numNodes = numSensorNodes;
                
                @display("i=block/circle_vs,blue;p=$fieldXSize,-20;b=50,50,rect,#FFF,#FFF");
        }

        person1: personNode {
            parameters:
                id = 1;
                
                fieldX = fieldXSize;
                fieldY = fieldYSize;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                @display("i=block/user_vs,blue;is=vs;");
        }
        
        person2: personNode {
            parameters:
                id = 2;
                
                fieldX = fieldXSize;
                fieldY = fieldYSize;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                @display("i=block/user_vs,blue;is=vs;");
        }
        
        sensor1: sensorNode {
            parameters:
                id = 100;
                
                lightingRadius = 100;
        		sensorRadius = 50;
        		
        		horizontalCells = horizontal_cells;
        		verticalCells = vertical_cells;
        		cellSize = cell_size;
                
                @display("i=block/user_vs,blue;is=vs;r=$sensorRadius,,#8080FF;p=-1000,-1000;b=$cellSize,$cellSize,rect,#FFF,#FFF");
        }
}

// @author André Garção
// @version 1.0

package wsn_lighting;

simple statisticsModule
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

network LightingSensorNetwork
{
    parameters:
        int field_x;
        int field_y;
        int numSensorNodes;
        int numPersonNodes;

		// Com argumentos vindos do omnet.ini
        @display("bgb=$field_x,$field_y,#FFF;bgg=5,1,grey95;");
        
    submodules:
        statistics: statisticsModule {
            parameters:
                numNodes = numSensorNodes;
                
                @display("i=block/circle_vs,blue;p=-10,-10");
        }

        person1: personNode {
            parameters:
                id = 1;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                @display("i=block/user_s,blue;is=vs;");
        }
        
        person2: personNode {
            parameters:
                id = 1;
                
                fieldX = field_x;
                fieldY = field_y;
                
                // Default values
                guiX = 0;
                guiY = 0;
                
                @display("i=block/user_s,blue;is=vs;");
        }
}

//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package wsn_lighting.personNode;

// The sensor node module. Connects to the wireless channel in order to communicate 
// with other nodes. Connects to psysical processes so it can sample them.

simple PersonNode {
 	parameters:
 	    int field_x;
 	    int field_y;
 	    
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
    	
    	int timeBetweenMoves = default(1000);
    	
	gates:
	    output toPhysicalProcess[];
}
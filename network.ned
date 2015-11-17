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

// @author André Garção
// @version 1.0

package wsn_lighting;

simple statisticsModule
{
    parameters:
        double fieldMaxX;
        double fieldMaxY;
        int numNodes;
}

simple personNode
{
    parameters:
        int id;
}

network LightingSensorNetwork
{
    parameters:
        double field_x;
        double field_y;
        int numSensorNodes;
        int numPersonNodes;

        @display("bgb=500,500");
    submodules:
        statistics: statisticsModule {
            parameters:
                fieldMaxX = field_x * 10;
                fieldMaxY = field_y * 10;
                numNodes = numSensorNodes;
                @display("i=block/circle_vs,blue;p=-10,-10");
        }

        person1: personNode {
            parameters:
                id = 1;
                @display("i=block/user_s,blue;p=100,100");
        }
}

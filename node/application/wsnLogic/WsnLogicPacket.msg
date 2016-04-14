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

cplusplus {{
#include "ApplicationPacket_m.h"
}}

class ApplicationPacket;

struct WsnLogicData {
    unsigned short type;				// message type (could be ONLY_LIGHT_RADIOUS, ONLY_LIGHT_CONE or LIGHT_RADIOUS_CONE)
    unsigned short originNodeID;		// Node ID from where the msg is originated from
	unsigned short senderNodeID;		// Node ID from the sender node
	int destinationNodesID[8];			// List of Node ID's that are the destination of the message. All the others discard the msg
	int hop;							// msg hop number in the network
} 

packet WsnLogicDataPacket extends ApplicationPacket {
	WsnLogicData extraData;
}
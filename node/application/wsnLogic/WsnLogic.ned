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

package wsn_lighting.node.application.wsnLogic;

// The sensor node module. Connects to the wireless channel in order to communicate 
// with other nodes. Connects to psysical processes so it can sample them.

simple WsnLogic like wsn_lighting.node.application.iApplication {
 parameters:
 	string applicationID = default ("WsnLogic");
	bool collectTraceInfo = default (true);
	int priority = default (1);
	int packetHeaderOverhead = default (8);	// in bytes
	int constantDataPayload = default (12);	// in bytes
	int maxSampleInterval = default (60000);// in msec
	int minSampleInterval = default (1000);	// in msec
	bool isSink = default (false);

	int timeToDiminishLightIntensity = default(5); // Time (in sec) for light to diminish its intensity
	
	// This configuration came from .ini
	bool coneLightingIsActive;	// Tell if we must create a cone of lighting when we detect a person
   	int radiousLighting;		// Tell us how many neighboor nodes must be also turn on when a person is detected
   		
 gates:
 	output toCommunicationModule;
	output toSensorDeviceManager;
	output toResourceManager;
	input fromCommunicationModule;
	input fromSensorDeviceManager;
	input fromResourceManager;
}

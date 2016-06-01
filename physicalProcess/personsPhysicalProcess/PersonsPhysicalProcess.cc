/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Yuri Tselishchev                                             *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *  
 ****************************************************************************/

#include "PersonsPhysicalProcess.h"

#define K_PARAM 0.1
#define A_PARAM 1

Define_Module(PersonsPhysicalProcess);

void PersonsPhysicalProcess::initialize()
{
	readIniFileParameters();

	// Get the number of persons
	cModule *fatherNode = this->getParentModule(); // Sensor Network
	this->numSensorNodes = fatherNode->par("numNodes");

	ev << "Oh para mim com " << this->numSensorNodes << " sensor node(s)" << endl;

	// Initialize the cArray with the number of Sensor Nodes
    this->sensorNodes = new cArray(NULL, this->numSensorNodes, 0);

    scheduleAt(simTime() + 10, new ResourceManagerMessage("Get all stats", GET_ALL_STATS));
}

void PersonsPhysicalProcess::handleMessage(cMessage * msg)
{
	switch (msg->getKind()) {
		case PHYSICAL_PROCESS_SAMPLING: {
		    ev << "Chegou aqui a msg" << endl;
			/*PhysicalProcessMessage *phyMsg = check_and_cast < PhysicalProcessMessage * >(msg);
			// int nodeIndex = phyMsg->getSrcID();
			// int sensorIndex = phyMsg->getSensorIndex();

			// get the sensed value based on node location
			phyMsg->setValue(calculateScenarioReturnValue(
				phyMsg->getXCoor(), phyMsg->getYCoor(), phyMsg->getSendingTime()));
			// Send reply back to the node who made the request
			send(phyMsg, "toNode", phyMsg->getSrcID());*/
			return;
		}

		case TIMER_SERVICE: {
		    /*int pos = -1;
			for (int i = 0; pos == -1 && i < max_num_cars; i++) {
				if (sources_snapshots[i][1].time < simTime())
					pos = i;
			}

			if (pos != -1) {
				trace() << "New car arrives on the bridge, index " << pos;
				if (genk_dblrand(0) > 0.5) {
					sources_snapshots[pos][0].x = point1_x_coord;
					sources_snapshots[pos][0].y = point1_y_coord;
					sources_snapshots[pos][1].x = point2_x_coord;
					sources_snapshots[pos][1].y = point2_y_coord;
				} else {
					sources_snapshots[pos][0].x = point2_x_coord;
					sources_snapshots[pos][0].y = point2_y_coord;
					sources_snapshots[pos][1].x = point1_x_coord;
					sources_snapshots[pos][1].y = point1_y_coord;
				}
				sources_snapshots[pos][0].time = simTime();
				sources_snapshots[pos][1].time = simTime() + road_length / car_speed;
				collectOutput("Cars generated on the road");
			}

			double arrival = genk_dblrand(0) * car_interarrival + car_interarrival / 2;
			scheduleAt(simTime() + arrival,	msg);*/
			return;
		}

		case GET_ALL_STATS:
		{
		    cObject *sensorNodeObject;
            cModule *sensorNodeModule, *resourceManagerModule;
            ResourceManager *resourceManager;
            double energySpend = 0;
            int i;

            for (i = 0; i < this->sensorNodes->size(); i++)
            {
                sensorNodeObject = this->sensorNodes->get(i);
                sensorNodeModule = this->getParentModule()->getModuleByPath(sensorNodeObject->getFullPath().c_str());
                resourceManagerModule = sensorNodeModule->getSubmodule("ResourceManager");
                resourceManager = check_and_cast<ResourceManager*>(resourceManagerModule);

                energySpend =  energySpend + (resourceManager->initialEnergy - resourceManager->remainingEnergy);
            }

            ev << "[PersonsPhysical::handleMessage::GET_ENERGY] Energia consumida até aos " << simTime() << " s é " << energySpend << endl;
            scheduleAt(simTime() + 10, new ResourceManagerMessage("Get all stats", GET_ALL_STATS));
            break;
		}

		default: {
			opp_error(":\n Physical Process received message other than PHYSICAL_PROCESS_SAMPLING");
		}
	}
}

void PersonsPhysicalProcess::finishSpecific()
{
    /*int i;
	for (i = 0; i < max_num_cars; i++) {
		delete[]sources_snapshots[i];
	}*/
	delete[]sources_snapshots;
}

void PersonsPhysicalProcess::readIniFileParameters(void)
{
	this->description = par("description");
}

double PersonsPhysicalProcess::calculateScenarioReturnValue(const double &x_coo,
							 const double &y_coo, const simtime_t &stime)
{
	double retVal = 0.0f;
	/*int i;
	double linear_coeff, distance, x, y;

	for (i = 0; i < max_num_cars; i++) {
		if (sources_snapshots[i][1].time >= stime) {
			linear_coeff = (stime - sources_snapshots[i][0].time) /
			    (sources_snapshots[i][1].time - sources_snapshots[i][0].time);
			x = sources_snapshots[i][0].x + linear_coeff * 
				(sources_snapshots[i][1].x - sources_snapshots[i][0].x);
			y = sources_snapshots[i][0].y + linear_coeff * 
				(sources_snapshots[i][1].y - sources_snapshots[i][0].y);
			distance = sqrt((x_coo - x) * (x_coo - x) +
				 (y_coo - y) * (y_coo - y));
			retVal += pow(K_PARAM * distance + 1, -A_PARAM) * car_value;
		}
	}*/
	return retVal;
}

void PersonsPhysicalProcess::registrySensorNodes(cModule *s)
{
    this->sensorNodes->add(s);
}

void PersonsPhysicalProcess::checkPersonNodeDetection(PersonNode *p)
{
    Enter_Method_Silent("checkPersonNodeDetection(personNode *p)");

    //ev << "[Persons Physical Process Module] Let see if any sensor detect the person #" << p->getIndex() << endl;

    int i, centerX, centerY, dist;
    int personNodeXCoord = p->xCoor;
    int personNodeYCoord = p->yCoor;

    cObject *sensorNodeObject;
    cModule *sensorNodeModule, *sensorManagerModule;
    SensorManager *sensorManager;

    for (i = 0; i < this->sensorNodes->size(); i++)
    {
        // Vejo se o ponto está dentro do raio de deteção ou não
        sensorNodeObject = this->sensorNodes->get(i);
        sensorNodeModule = this->getParentModule()->getModuleByPath(sensorNodeObject->getFullPath().c_str());
        sensorManagerModule = sensorNodeModule->getSubmodule("SensorManager");
        sensorManager = check_and_cast<SensorManager*>(sensorManagerModule);

        centerX = sensorNodeModule->par("xCoor");
        centerY = sensorNodeModule->par("yCoor");
        dist = sqrt( (double)(centerX-personNodeXCoord)*(centerX-personNodeXCoord) + (centerY-personNodeYCoord)*(centerY-personNodeYCoord));

        if (dist <= sensorManager->getSensorRadius())
        {
            // Se tiver temos de desplotar as acções que acontecem após o sensor encontrar movimento
            //ev << "[Persons Physical Process Module] Sensor #" << sensorNodeModule->getIndex() << " detected the person node #" << p->getIndex() << endl;
            sensorManager->detectedMovement();
        }
        else
        {
            //ev << "[Persons Physical Process Module] Sensor #" << sensorNodeModule->getIndex() << " do NOT detected the person node #" << p->getIndex() << endl;
        }
    }
}

void PersonsPhysicalProcess::updateGUI()
{
    cDisplayString &ds = this->getDisplayString();

    ds.parse(ds.str());

}

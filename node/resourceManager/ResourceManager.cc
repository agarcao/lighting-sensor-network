/*******************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                            *
 *  Developed at the ATP lab, Networked Systems research theme                 *
 *  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis, Yuriy Tselishchev *
 *  This file is distributed under the terms in the attached LICENSE file.     *
 *  If you do not find this file, copies can be found by writing to:           *
 *                                                                             *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia                *
 *      Attention:  License Inquiry.                                           *
 *                                                                             *  
 *******************************************************************************/

#include "ResourceManager.h"
#include "PersonsPhysicalProcess.h"

Define_Module(ResourceManager);

void ResourceManager::initialize()
{
	sigmaCPUClockDrift = par("sigmaCPUClockDrift");
	//using the "0" rng generator of the ResourceManager module
	cpuClockDrift = normal(0, sigmaCPUClockDrift);
	/* Crop any values beyond +/- 3 sigmas. Some protocols (e.g., MAC) rely on
	 * bounded cpuClockDrift. Although the bounds are conservative (usually 3sigmas),
	 * if you instantiate thousands of nodes (in multiple runs) we will get a
	 * couple of nodes that will be beyond this bound. Limiting/Croping the drift
	 * is actually realistic, since usually there is some kind of quality
	 * control on quartz crystals or the boards that use them (sensor node)
	 */
	if (cpuClockDrift > 3 * sigmaCPUClockDrift)
		cpuClockDrift = 3 * sigmaCPUClockDrift;
	if (cpuClockDrift < -3 * sigmaCPUClockDrift)
		cpuClockDrift = -3 * sigmaCPUClockDrift;

	initialEnergy = par("initialEnergy");
	ramSize = par("ramSize");
	baselineNodePower = par("baselineNodePower");
	periodicEnergyCalculationInterval = (double)par("periodicEnergyCalculationInterval") / 1000;

	if (baselineNodePower < 0 || periodicEnergyCalculationInterval < 0)
		opp_error("Illegal values for baselineNodePower and/or periodicEnergyCalculationInterval in resource manager module");

	currentNodePower = baselineNodePower;
	remainingEnergy = initialEnergy;
	totalRamData = 0;
	disabled = true;
	this->lightIntensity = par("lightIntensity");
}

void ResourceManager::calculateEnergySpent()
{
	if (remainingEnergy > 0) {
		simtime_t timePassed = simTime() - timeOfLastCalculation;
		trace() << "energy consumed in the last " << timePassed << 
			"s is " <<(timePassed * currentNodePower);
		consumeEnergy(SIMTIME_DBL(timePassed * currentNodePower / 1000.0));
		timeOfLastCalculation = simTime();

		cancelEvent(energyMsg);
		scheduleAt(simTime() + periodicEnergyCalculationInterval, energyMsg);
	}
}

/* The ResourceManager module has only one "unconnected" port where it can receive messages that
 * update the power drawn by a module, or a NODE_STARTUP message. If disabled we still process
 * messages because we want to have the latest power drawn from any module.
 */
void ResourceManager::handleMessage(cMessage * msg)
{

	switch (msg->getKind()) {

		case NODE_STARTUP:{
			disabled = false;
			timeOfLastCalculation = simTime();
			energyMsg = new cMessage("Periodic energy calculation", TIMER_SERVICE);
        		scheduleAt(simTime() + periodicEnergyCalculationInterval, energyMsg);
			break;
		}
	
		case TIMER_SERVICE:{
			calculateEnergySpent();
			return;
		}

		case RESOURCE_MANAGER_DRAW_POWER:{
			ResourceManagerMessage *resMsg = check_and_cast<ResourceManagerMessage*>(msg);
			int id = resMsg->getSenderModuleId();
			double oldPower = storedPowerConsumptions[id];
			trace() << "New power consumption, id = " << id << ", oldPower = " << 
					currentNodePower << ", newPower = " << 
					currentNodePower - oldPower + resMsg->getPowerConsumed();
			if (!disabled)
				calculateEnergySpent();
			currentNodePower = currentNodePower - oldPower + resMsg->getPowerConsumed();
			storedPowerConsumptions[id] = resMsg->getPowerConsumed();
			break;
		}

		case RESOURCE_MANAGER_LIGHT:{
		    ResourceManagerMessage *resMsg = check_and_cast<ResourceManagerMessage*>(msg);
		    this->changeLightIntensity(resMsg->getIncreaseLightIntensity());
		    break;
		}

		default:{
			opp_error("ERROR: Unexpected message received by resource manager: %s", msg->getKind());
		}
	}
	delete msg;
}

void ResourceManager::finishSpecific()
{
	calculateEnergySpent();
	declareOutput("Consumed Energy");
	collectOutput("Consumed Energy", "", getSpentEnergy());
}

double ResourceManager::getSpentEnergy(void)
{
	Enter_Method("getSpentEnergy()");
	return (initialEnergy - remainingEnergy);
}

double ResourceManager::getCPUClockDrift(void)
{
	Enter_Method("getCPUClockDrift(void)");
	return (1.0f + cpuClockDrift);
}

void ResourceManager::consumeEnergy(double amount)
{
	Enter_Method("consumeEnergy(double amount)");

	if (remainingEnergy <= amount) {
		remainingEnergy = 0;
		send(new cMessage("Destroy node message", OUT_OF_ENERGY), "toSensorDevManager");
		send(new cMessage("Destroy node message", OUT_OF_ENERGY), "toApplication");
		send(new cMessage("Destroy node message", OUT_OF_ENERGY), "toNetwork");
		send(new cMessage("Destroy node message", OUT_OF_ENERGY), "toMac");
		send(new cMessage("Destroy node message", OUT_OF_ENERGY), "toRadio");
                remainingEnergy = 0;
	} else
		remainingEnergy -= amount;
}

void ResourceManager::destroyNode(void)
{
	Enter_Method("destroyNode(void)");

	send(new cMessage("Destroy node message", DESTROY_NODE), "toSensorDevManager");
	send(new cMessage("Destroy node message", DESTROY_NODE), "toApplication");
	send(new cMessage("Destroy node message", DESTROY_NODE), "toNetwork");
	send(new cMessage("Destroy node message", DESTROY_NODE), "toMac");
	send(new cMessage("Destroy node message", DESTROY_NODE), "toRadio");
}

int ResourceManager::RamStore(int numBytes)
{
	Enter_Method("RamStore(int numBytes)");

	int ramHasSpace = ((totalRamData + numBytes) <= ramSize) ? 1 : 0;
	if (!ramHasSpace) {
		trace() << "\n[Resource Manager] t= " << simTime() <<
				": WARNING: Data not stored to Ram. Not enough space to store them.";
		return 0;
	} else
		totalRamData += numBytes;
	return 1;
}

void ResourceManager::RamFree(int numBytes)
{
	Enter_Method("RamFree(int numBytes)");
	totalRamData -= numBytes;
	totalRamData = (totalRamData < 0) ? 0 : totalRamData;
}

/*
 * Function to see if we must change the node's light intensity and make also this changes in the GUI
 */
void ResourceManager::changeLightIntensity(bool increase)
{
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Resource Module] Enter in changeLightIntensity function" << endl;

    string displayString;

    // 1st - Check if boolean is true or not
    if(increase)
    {
        ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Resource Module] Increase msg receive. We gonna put the light on" << endl;
        this->lightIntensity = 1;
        displayString = "status/yellow_25";
    }
    else
    {
        ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Resource Module] Light intensity gonna decrease" << endl;
        this->lightIntensity = 0;
        displayString = "status/off_25";
    }

    // Put in GUI
    cDisplayString &nodeDS = getParentModule()->getDisplayString();
    nodeDS.setTagArg("i",0, displayString.c_str());

    // TODO: Temos que fazer o update ao GUI
    cModule *grandFatherNode = this->getParentModule()->getParentModule();
    cModule *physicalProcess = grandFatherNode->getModuleByPath("SensorNetwork.physicalProcess[0]");
    PersonsPhysicalProcess *ppp = check_and_cast<PersonsPhysicalProcess *>(physicalProcess);

    ppp->updateGUI();

    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Resource Module] Exiting changeLightIntensity function" << endl;
}

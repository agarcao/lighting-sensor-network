/*
 * @author André Garção
 * @version 1.0
 */

#include "sensorNode.h"
#include "statisticsNode.h"

// The module class needs to be registered with OMNeT++
Define_Module(sensorNode);

void sensorNode::initialize()
{
    ev << "Initialize Sensor Node" << endl;

    this->id = par("id");

    // Nr of horizontal and vertical cells in the GUI. And the size of one cell
    this->horizontalCells = par("horizontalCells");
    this->verticalCells = par("verticalCells");
    this->cellSize = par("cellSize");

    // Radius
    this->lightingRadius = par("lightingRadius");
    this->sensorRadius = par("sensorRadius");

    // Light Intensity - Should start off
    this->lightIntensity = 0;

    // Time (ticks) until light diminish it intensity if don't detect any movement
    this->timeToDiminishLightIntensity = 10;

    int numCellsInGui = this->horizontalCells * this->verticalCells;

    if (this->id > 0 && this->id <= numCellsInGui)
    {
        // We must discover where the sensor must be draw
        //// 1st - What line it should be draw
        int lineToDraw = this->id / this->horizontalCells;
        this->guiX = (lineToDraw * this->cellSize) + (this->cellSize / 2);

        //// 2nd - What line it should be draw
        int collumnToDraw = (this->id - 1) % this->horizontalCells;
        this->guiY = (collumnToDraw * this->cellSize) + (this->cellSize / 2);

        ev << "Eu (Sensor Node #" << this->id << " ) inicializei-me na posição (" << this->guiX << ", " << this->guiY << ")" << endl;

        char s[10];
        cDisplayString &nodeDS = getDisplayString();

        // Set x
        sprintf(s,"%d",this->guiX);
        nodeDS.setTagArg("p",0,s);

        // Set y
        sprintf(s,"%d",this->guiY);
        nodeDS.setTagArg("p",1,s);

        // Regist in Statists Module
        cModule *statisticsNodeModule = this->getParentModule()->getSubmodule("statistics");
        statisticsNode *sM = check_and_cast<statisticsNode *>(statisticsNodeModule);

        sM->registrySensorNodes(this);
    }
    // If node isn't between 0 and num of cells in GUI - 1 then can't be draw
    else
    {
        ev << "Eu (Sensor Node #" << this->id << " ) não tenho ID entre 0 e " << numCellsInGui << ". Não inicializei-me!" << endl;
    }

    ev << "End the initialization Sensor Node" << endl;
}

void sensorNode::handleMessage(cMessage *msg)
{
    if (strcmp("diminishLightIntensity", msg->getName())== 0)
    {
        ev << "[Sensor Node #" << this->id << "] Light is on. Let turn it off" << endl;
        // Change the light intensity - In a 1st phase just on/off
        this->lightIntensity = 0;

        ev << "[Sensor Node #" << this->id << "] Light is now off. Make the changes in GUI" << endl;
        // Represent this change in the GUI
        cDisplayString &nodeDS = getDisplayString();
        nodeDS.setTagArg("r",1,"");
    }
}

void sensorNode::finish()
{}

/**
 * This function is called when this sensor detect a movement.
 *
 * */
void sensorNode::detectedMovement()
{
    Enter_Method_Silent("detectedMovement()");

    ev << "[Sensor Node #" << this->id << "] Movement detected. Entering in detectMovement function" << endl;

    // Check if sensor node is off
    if(!this->lightIntensity)
    {
        ev << "[Sensor Node #" << this->id << "] Light is off. Let turn it on" << endl;
        // Change the light intensity - In a 1st phase just on/off
        this->lightIntensity = 1;

        ev << "[Sensor Node #" << this->id << "] Light is now on. Make the changes in GUI" << endl;
        // Represent this change in the GUI
        cDisplayString &nodeDS = getDisplayString();
        nodeDS.setTagArg("r",1,"#FFF");
    }

    /* We should cancel the changeLightIntensity event, a restart it all over again with the initial ticks */
    ev << "[Sensor Node #" << this->id << "] Send new selfMessage to light diminish it intensity" << endl;

    // Cancel and Delete the self event (this way we dont need to check if event is already create or not
    cancelAndDelete(this->selfEvent);

    // Create new message
    this->selfEvent = new cMessage("diminishLightIntensity");

    // Set new timer
    scheduleAt(simTime() + this->timeToDiminishLightIntensity, this->selfEvent);


    // TODO: Should send message to neighbors sensor nodes to inform them that movement have been detected and they should react to it

    ev << "[Sensor Node #" << this->id << "] Exiting detectMovement function" << endl;
}

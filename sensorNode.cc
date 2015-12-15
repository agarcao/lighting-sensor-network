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

    // Field Size
    this->fieldX = par("fieldX");
    this->fieldY = par("fieldY");

    // Radius
    this->lightingRadius = par("lightingRadius");
    this->sensorRadius = par("sensorRadius");

    // Light Intensity - Should start off
    this->lightIntensity = 0;

    // Random Values for initial Person position on the map
    this->guiX = rand() % this->fieldX;
    this->guiY = rand() % this->fieldY;

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

    ev << "End the initialization Sensor Node" << endl;
}

void sensorNode::handleMessage(cMessage *msg)
{

}

void sensorNode::finish()
{}

/**
 * This function is called when this sensor detect a movement.
 *
 * */
void sensorNode::detectedMovement()
{
    ev << "[Sensor Node #" << this->id << "] Movement detected. Entering in detectMovement function" << endl;

    // Change the light intensity - In a 1s phase just on/off
    this->lightIntensity = 1

    // TODO: Should send message to neighbors sensor nodes to inform them that movement have been detected and they should react to it

    ev << "[Sensor Node #" << this->id << "] Exiting detectMovement function" << endl;
}

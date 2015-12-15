/*
 * @author André Garção
 * @version 1.0
 */

#include "statisticsNode.h"

// The module class needs to be registered with OMNeT++
Define_Module(statisticsNode);

void statisticsNode::initialize()
{
    std::cout << "Initialize Statistics Module" << endl;

    // Get number of sensor nodes
    this->numSensorNodes = par("numNodes");

    // Initialize the cArray with the number of Sensor Nodes
    this->sensorNodes = new cArray(NULL, this->numSensorNodes, 0);

    std::cout << "End the initialization Statistics Module" << endl;
}

void statisticsNode::handleMessage(cMessage *msg)
{}

void statisticsNode::finish()
{}

void statisticsNode::registrySensorNodes(sensorNode *s)
{
    this->sensorNodes->add(s);
}

bool statisticsNode::checkPersonNodeDetection(personNode *p)
{
    int i, centerX, centerY, dist;
    int personNodeXCoord = p->guiX;
    int personNodeYCoord = p->guiY;

    cObject *cO;
    sensorNode *s;

    for (i = 0; i < this->sensorNodes->size(); i++)
    {
        cO = this->sensorNodes->get(i);
        s = check_and_cast<sensorNode *>(cO);

        centerX = s->guiX;
        centerY = s->guiY;
        dist = sqrt( (double)(centerX-personNodeXCoord)*(centerX-personNodeXCoord) + (centerY-personNodeYCoord)*(centerY-personNodeYCoord));

        if (dist <= s->sensorRadius)
        {
            return true;
        }
    }

    return false;
}

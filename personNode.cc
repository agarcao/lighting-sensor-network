/*
 * @author André Garção
 * @version 1.0
 */

#include "personNode.h"

// The module class needs to be registered with OMNeT++
Define_Module(personNode);

void personNode::initialize()
{
    this->id = par("id");

    // Field Size
    this->fieldX = par("fieldX");
    this->fieldY = par("fieldY");

    // Random Values for initial Person position on the map
    this->guiX = rand() % this->fieldX;
    this->guiY = rand() % this->fieldY;

    ev << "Eu (Person Node #" << this->id << " ) inicializei-me na posição ("
            << this->guiX << ", " << this->guiY << ")" << endl;

    char s[10];
    setDisplayString("i=block/user_s,blue;is=vs;");
    cDisplayString &nodeDS = getDisplayString();

    // Set x
    sprintf(s,"%d",this->guiX);
    nodeDS.setTagArg("p",0,s);

    // Set y
    sprintf(s,"%d",this->guiY);
    nodeDS.setTagArg("p",1,s);

    // Finaly send message to itself to move
    this->event = new cMessage("move");
    scheduleAt(simTime() + 5, this->event);

    ev << "Send move message" << endl;
}

void personNode::handleMessage(cMessage *msg)
{
    // Se é uma mensagem do tipo 'move' o person node tem que se mover
    if (strcmp("move", msg->getName())== 0)
    {

        ev << "Eu (Person Node #" << this->id << " ) recebi msg para mover" << endl;

        // Mudamos a posição da pessoa (person node)
        this->guiX = (this->guiX + ((rand() % 11) + (-5))) % this->fieldX;
        this->guiY = (this->guiY + ((rand() % 11) + (-5))) % this->fieldY;

        ev << "Move to (" << this->guiX << ", " <<  this->guiY << ")" << endl;

        char s[10];
        cDisplayString &nodeDS = getDisplayString();

        // Set x
        sprintf(s,"%d",this->guiX);
        nodeDS.setTagArg("p",0,s);

        sprintf(s,"%d",this->guiY);
        nodeDS.setTagArg("p",1,s);

        ev << "Send NEW move message" << endl;
        scheduleAt(simTime() + 5, this->event);
    }
}

void personNode::finish()
{}

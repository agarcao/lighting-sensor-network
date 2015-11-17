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

    // Random Values for initial Person position on the map
    this->guiX = 10;
    this->guiY = 20;

    std::cout << "Initialize Person Nr:. " << this->id <<  endl;
}

void personNode::handleMessage(cMessage *msg)
{}

void personNode::finish()
{}

/*
 * @author André Garção
 * @version 1.0
 */

#include "statisticsNode.h"

// The module class needs to be registered with OMNeT++
Define_Module(statisticsModule);

void statisticsModule::initialize()
{
    std::cout << "Initialize Statistics Module" << endl;
}

void statisticsModule::handleMessage(cMessage *msg)
{}

void statisticsModule::finish()
{}

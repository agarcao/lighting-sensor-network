/*
 * @author André Garção
 * @version 1.0
 */

#ifndef __PERSON_H__
#define __PERSON_H__

#include <omnetpp.h>
#include <iostream>

#include "PhysicalProcessMessage_m.h"
#include "CastaliaModule.h"

class PersonNode : public cSimpleModule
{
    // Event that come to Person Module
    cMessage *event;

    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
        virtual void initialize();

    public:
        // Node position
        int xCoor;
        int yCoor;
};

#endif

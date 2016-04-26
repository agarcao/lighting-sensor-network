#ifndef _WALL_H_
#define _WALL_H_

#include <omnetpp.h>
#include <iostream>

#include "CastaliaModule.h"

using namespace std;

class Wall : public cSimpleModule
{
 private:

    int numNodes;

 protected:
    // The following redefined virtual function holds the algorithm.
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void initialize();
};

#endif

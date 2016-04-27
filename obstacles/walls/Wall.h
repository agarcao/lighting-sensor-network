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

    // Ponto medio onde se encontra o obstaculo
    int medianX;
    int medianY;

 protected:
    // The following redefined virtual function holds the algorithm.
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void initialize();

 public:
    // Diz-nos entre que cells o obstaculo está
    int betweenCells[2];

    // Diz-nos qual é a posição do obstaculo em relação a betweenCells[0]
    int wallPosition;
};

#endif

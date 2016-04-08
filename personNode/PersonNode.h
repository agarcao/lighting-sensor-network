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

enum PersonMovementDirections {
    NORTH = 1,
    EAST = 2,
    SOUTH = 3,
    WEST = 4,
};

class PersonNode : public cSimpleModule
{
    // Event that come to Person Module
    cMessage *event;

    protected:
        // Parametros do modulo (podem tbm ser definidos no sensorNetwork.ini)
        int timeBetweenMoves;                   // o nó move se de x em x segundos (neste caso 1s)
        int moveVelocity;                       // qtos metros é que o nó se move (neste caso 2.5m por _timeBetweenMoves_ seg)
        double changeDirectionProbability;      // probabilidade de o nó mudar de direção em cada movimento (de _timeBetweenMoves_ em _timeBetweenMoves_)
        int movementDirection;                  // direção para onde o nó se desloca

        // The following redefined virtual function holds the algorithm.
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
        virtual void initialize();

        // Helpers
        int checkDirection(string movementDirection);
        int changeDirection(bool force = false);
        int getDirectionX(int direction);
        int getDirectionY(int direction);
    public:
        // Node position
        int xCoor;
        int yCoor;
};

#endif

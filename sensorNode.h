/*
 * @author André Garção
 * @version 1.0
 */

#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <omnetpp.h>
#include <iostream>

class sensorNode : public cSimpleModule
{
    // Event that come to Person Module
    cMessage *event;

    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
        virtual void initialize();

    public:
        int id;

        int fieldX;
        int fieldY;

        int guiX;
        int guiY;

        int lightingRadius;
        int sensorRadius;
};

#endif

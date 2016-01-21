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
    cMessage *selfEvent;

    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
        virtual void initialize();

    public:
        int id;

        // Where icon are draw in GUI
        int guiX;
        int guiY;

        int lightingRadius;
        int sensorRadius;

        // Say if light of Sensor Node is on or off
        int lightIntensity;
        int timeToDiminishLightIntensity;

        // Nr of horizontal and vertical cells in the GUI. And the size of one cell
        int horizontalCells;
        int verticalCells;
        int cellSize;

        /**************
         * FUNCTIONS *
         **************/

        void detectedMovement();
};

#endif

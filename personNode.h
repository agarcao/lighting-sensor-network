/*
 * @author André Garção
 * @version 1.0
 */

#ifndef __PERSON_H__
#define __PERSON_H__

#include <omnetpp.h>
#include <iostream>

class personNode : public cSimpleModule
{
    // Event that come to Person Module
    cMessage *event;

    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

    public:
        // Person Id
        int id;

        // Node position in the gui (Map)
        int guiX;
        int guiY;

        // Field Size
        int fieldX;
        int fieldY;
};

#endif

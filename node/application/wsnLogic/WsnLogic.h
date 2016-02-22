#ifndef _WSNLOGIC_H_
#define _WSNLOGIC_H_

#include "VirtualApplication.h"
#include "WsnLogicPacket_m.h"

using namespace std;

enum WsnLogicTimers {
    REQUEST_SAMPLE = 1,
    SEND_DATA = 2,
};

class WsnLogic: public VirtualApplication {
 private:
    cMessage *selfEvent;                // Where we put the event to diminish the light intensity
    int timeToDiminishLightIntensity;   // Time (in msec) for light to diminish its intensity

    double maxSampleInterval;
    double minSampleInterval;

    int routingLevel;
    double lastSensedValue;
    int currSentSampleSN;

    double randomBackoffIntervalFraction;
    bool sentOnce;

 protected:
    void startup();
    void fromNetworkLayer(ApplicationPacket *, const char *, double, double);
    void handleSensorReading(SensorReadingMessage *);
    void timerFiredCallback(int);
    void handleSelfEvent();
};

#endif              // _WSNLOGIC_H_

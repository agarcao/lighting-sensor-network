#ifndef _WSNLOGIC_H_
#define _WSNLOGIC_H_

#include "VirtualApplication.h"
#include "WsnLogicPacket_m.h"

using namespace std;

enum WsnLogicTimers {
    DIMINISH_LIGHT = 1
};

enum WSNLogicMessageTypes {
    ONLY_LIGHT_RADIOUS = 1,
    ONLY_LIGHT_CONE = 2,
    LIGHT_RADIOUS_CONE = 3
};

class WsnLogic: public VirtualApplication {
 private:
    int timeToDiminishLightIntensity;   // Time (in msec) for light to diminish its intensity

    double maxSampleInterval;
    double minSampleInterval;

    int routingLevel;
    double lastSensedValue;
    int currSentSampleSN;

    double randomBackoffIntervalFraction;
    bool sentOnce;

    // This values came from .ini
    bool coneLightingIsActive;  // Tell if we must create a cone of lighting when we detect a person
    int radiousLighting;        // Tell us how many neighbors nodes must be also turn on when a person is detected

 protected:
    void startup();
    void fromNetworkLayer(ApplicationPacket *, const char *, double, double);
    void handleSensorReading(SensorReadingMessage *);
    void timerFiredCallback(int);
    void handleSelfEvent();
};

#endif              // _WSNLOGIC_H_

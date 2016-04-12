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

enum PersonMovementDirections {
    NORTH = 0,
    NORTHEAST = 1,
    EAST = 2,
    SOUTHEAST = 3,
    SOUTH = 4,
    SOUTHWEST = 5,
    WEST = 6,
    NORTHWEST = 7,
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
    bool coneLightingIsActive;          // Tell if we must create a cone of lighting when we detect a person
    int radiousLighting;                // Tell us how many neighbors nodes must be also turn on when a person is detected

    int movementDirection;              // Diz nos, em 1º lugar, se estamos à espera de detectar uma pessoa (person node) e qual é a direção deste
    map<int, int> neighborsNodesIds;   // Contem o mapeamento entre o localização dos nós vizinhos e o seu ID

    /* FUNCTIONS */
    void turnOnTheLight();
    int getMovementDirectionFromSenderNodeID(int senderNodeID);

 protected:
    void startup();
    void fromNetworkLayer(ApplicationPacket *, const char *, double, double);
    void handleSensorReading(SensorReadingMessage *);
    void timerFiredCallback(int);
    void handleSelfEvent();
};

#endif              // _WSNLOGIC_H_

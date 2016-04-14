#ifndef _WSNLOGIC_H_
#define _WSNLOGIC_H_

#include "VirtualApplication.h"
#include "WsnLogicPacket_m.h"

using namespace std;

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

enum WsnLogicTimers {
    CONE_NORTH_DIRECTION = PersonMovementDirections::NORTH,
    CONE_NORTHEAST_DIRECTION = PersonMovementDirections::NORTHEAST,
    CONE_EAST_DIRECTION = PersonMovementDirections::EAST,
    CONE_SOUTHEAST_DIRECTION = PersonMovementDirections::SOUTHEAST,
    CONE_SOUTH_DIRECTION = PersonMovementDirections::SOUTH,
    CONE_SOUTHWEST_DIRECTION = PersonMovementDirections::SOUTHWEST,
    CONE_WEST_DIRECTION = PersonMovementDirections::WEST,
    CONE_NORTHWEST_DIRECTION = PersonMovementDirections::NORTHWEST,
    DIMINISH_LIGHT = 8
};

class WsnLogic: public VirtualApplication {
 private:
    int timeToDiminishLightIntensity;   // Time (in sec) for light to diminish its intensity

    double maxSampleInterval;
    double minSampleInterval;

    int routingLevel;
    double lastSensedValue;
    int currSentSampleSN;

    double randomBackoffIntervalFraction;
    bool sentOnce;

    map<int, int> neighborsNodesIds;        // Contem o mapeamento entre o localização dos nós vizinhos e o seu ID
    list<int> movementDirections;           // Contem o mapeamento entre o localização dos nós vizinhos e o seu ID

    float timeToDeleteMovementDirection;  // This tell us the seconds that a movement direction stay in the movementDirections list

    // This values came from .ini
    bool coneLightingIsActive;          // Tell if we must create a cone of lighting when we detect a person
    int radiousLighting;                // Tell us how many neighbors nodes must be also turn on when a person is detected

    /* FUNCTIONS */
    void turnOnTheLight();
    int getMovementDirectionFromSenderNodeID(int senderNodeID);
    void coneLightingLogic(WsnLogicDataPacket* broadcastDataPacket);

 protected:
    void startup();
    void fromNetworkLayer(ApplicationPacket *, const char *, double, double);
    void handleSensorReading(SensorReadingMessage *);
    void timerFiredCallback(int);
    void handleSelfEvent();
};

#endif              // _WSNLOGIC_H_

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

enum NodeNeighborsDirections {
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
    CONE_NORTH_DIRECTION = NodeNeighborsDirections::NORTH,
    CONE_NORTHEAST_DIRECTION = NodeNeighborsDirections::NORTHEAST,
    CONE_EAST_DIRECTION = NodeNeighborsDirections::EAST,
    CONE_SOUTHEAST_DIRECTION = NodeNeighborsDirections::SOUTHEAST,
    CONE_SOUTH_DIRECTION = NodeNeighborsDirections::SOUTH,
    CONE_SOUTHWEST_DIRECTION = NodeNeighborsDirections::SOUTHWEST,
    CONE_WEST_DIRECTION = NodeNeighborsDirections::WEST,
    CONE_NORTHWEST_DIRECTION = NodeNeighborsDirections::NORTHWEST,
    DIMINISH_LIGHT = 8
};

class WsnLogic: public VirtualApplication {
 private:

    ResourceManager *resMgrModule; // Pointer p/ o Resource Mangager deste nó (essencial para saber qual a intensidade da luz)

    // Parametros necessários para a lógica da luz
    int timeToDiminishLightIntensity;   // Time (in sec) for light to diminish its intensity
    int lightIntensity;                 // Diz-nos em que intensidade de luz estamos no momento (0 - Apagada; 1 - Media luz; 2 - Máxima intensidade)
    bool nearObstacle;                  // Diz-nos se este nó está perto dum obstaculo (se tiver a luz nunca deve estar apagada)

    double maxSampleInterval;
    double minSampleInterval;

    int routingLevel;
    double lastSensedValue;
    int currSentSampleSN;

    double randomBackoffIntervalFraction;
    bool sentOnce;

    list<int> movementDirections;           // Tem a lista de direcções recebidas

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

 public:
    map<int, int> neighborsNodesIds;        // Contem o mapeamento entre o localização dos nós vizinhos e o seu ID
};

#endif              // _WSNLOGIC_H_

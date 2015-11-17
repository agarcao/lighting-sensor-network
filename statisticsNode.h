/*
 * @author André Garção
 * @version 1.0
 */

#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <omnetpp.h>
#include <iostream>

class statisticsModule : public cSimpleModule
{
    cMessage *event;

    protected:
        // The following redefined virtual function holds the algorithm.
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();

    public:
        int numNodes;
        int fieldMaxX;
        int fieldMaxY;

        // to get the percentage of coverage
        /*
        cOutVector percentageCoverageStatVector;
        cOutVector percentageActiveNodeStatVector;
        cOutVector stddevEnergyLevelStatVector;
        cDoubleHistogram meanPercentageCoversetCoverage;
        cDoubleHistogram meanNumberCoverset;
        cDoubleHistogram meanCaptureRate;
        cDoubleHistogram meanEnergyLevel;


        double initialCoverage;
        double roundCoverage;

        noeud *allNodes;
        int nbRegisteredSensorNodeCounter; // count the number of registered sensor nodes

        obstacleManager obstacleList; // store and manage the obstacle

        float distance(Point , Point);
        double percentageCoverage(noeud* , int);
    #ifdef SCHEDULE_PROB
        bool probWakeNode(unsigned int, int);
        bool probTestNode(unsigned int, int);
    #endif
        double percentageCoverageRound(noeud* , int);
        void fillNodes(Point p, Point v, Point b, Point c, int id, int nbCamera, int cameraIndex, VideoSensorNode* theNode);
        void updateNodesStatus(int id, bool isActive, bool isAlive=true);
        void nodeFinish(int);
        noeud* getNodeInfo(int);
    #ifdef CRITICALITY_ADAPTATIVE_MAC
        void countAlertNumber(int id);
        void countAlertResponse(int id, int alertNb);
    #endif
    */
};

#endif

#ifndef _WIRELESSCHANNEL_H
#define _WIRELESSCHANNEL_H

#include "WirelessChannelMessages_m.h"
#include "WirelessChannelTemporal.h"
#include "VirtualMobilityManager.h"
#include "CastaliaModule.h"

#include "WsnLogic.h"
#include "Wall.h"
#include "PersonsPhysicalProcess.h"

#include "time.h"
#include <list>

using namespace std;

class PathLossElement {
    public:
        int cellID;
        float avgPathLoss;
        float lastObservedDiffFromAvgPathLoss;
        simtime_t lastObservationTime;

        PathLossElement(int c, float PL) {
            cellID = c;
            avgPathLoss = PL;
            lastObservedDiffFromAvgPathLoss = PL;
            lastObservationTime = 0.0;
        };

        ~PathLossElement() {
        };
};

class WirelessChannel: public CastaliaModule {
    private:

        /*--- variables corresponding to .ned file's parameters ---*/
        int numOfNodes;

        double xFieldSize;
        double yFieldSize;
        double zFieldSize;
        double xCellSize;
        double yCellSize;
        double zCellSize;

        // variables corresponding to Wireless Channel module parameters
        double pathLossExponent;	// the path loss exponent
        double noiseFloor;			// in dBm
        double PLd0;				// Power loss at a reference distance d0 (in dBm)
        double d0;					// reference distance (in meters)
        double sigma;				// std of a zero-mean Gaussian RV
        double bidirectionalSigma;	// std of a zero-mean Gaussian RV

        const char *pathLossMapFile;
        const char *temporalModelParametersFile;
        double signalDeliveryThreshold;
        bool onlyStaticNodes;
        double receiverSensitivity;
        double maxTxPower;			// this is derived, by reading all the Tx power levels

        /*--- other class member variables ---*/
        int numOfXCells, numOfYCells, numOfZCells;
        int numOfSpaceCells;
        int xIndexIncrement, yIndexIncrement, zIndexIncrement;

        list <PathLossElement*>*pathLoss;		// an array of lists (numOfSpaceCels long)
                                                // holding info on path loss. Element i of the
                                                // array is a list elements that describe which
                                                // cells are affected (and how) when a
                                                // node in cell i transmits.

        list <int>*nodesAffectedByTransmitter;	// an array of lists (numOfNodes long). The list
                                                // at array element i holds the node IDs that are
                                                // affected when node i transmits.

        list <int>*cellOccupation;				// an array of lists (numOfSpaceCels long) that
                                                // tells us which nodes are in cell i.

        NodeLocation_type *nodeLocation;		// an array (numOfNodes long) that gives the
                                                // location for each node.

        bool temporalModelDefined;
        channelTemporalModel *temporalModel;

    protected:
        virtual void initialize(int);
        virtual void handleMessage(cMessage * msg);
        virtual void finishSpecific();

        void readIniFileParameters(void);
        void parsePathLossMap(void);
        int parseInt(const char *, int *);
        int parseFloat(const char *, float *);
        void printRxSignalTable(void);
        void updatePathLossElement(int, int, float);
        float calculateProb(float, int);

        int numInitStages() const;
};

#endif				//_WIRELESSCHANNEL_H

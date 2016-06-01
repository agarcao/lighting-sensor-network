#ifndef _APPLICATIONMODULESIMPLE_H_
#define _APPLICATIONMODULESIMPLE_H_

#include "SensorManagerMessage_m.h"
#include "PhysicalProcessMessage_m.h"

#include "VirtualMobilityManager.h"
#include "CastaliaModule.h"

using namespace std;

class SensorManager: public CastaliaModule {
 private:
	/*--- The .ned file's parameters ---*/
	bool printDebugInfo;
	vector<int> corrPhyProcess;
	vector<double> pwrConsumptionPerDevice;
	vector<simtime_t> minSamplingIntervals;
	vector<string> sensorTypes;
	vector<double> sensorBiasSigma;
	vector<double> sensorNoiseSigma;
	vector<double> sensorSensitivity;
	vector<double> sensorResolution;
	vector<double> sensorSaturation;

	/*--- Custom class member variables ---*/
	int self;		// the node's ID
	int totalSensors;
	vector<simtime_t> sensorlastSampleTime;
	vector<double> sensorLastValue;
	vector<double> sensorBias;
	VirtualMobilityManager *nodeMobilityModule;
	int disabled;

	int sensorRadius;

 protected:
	virtual void initialize();
	virtual void handleMessage(cMessage * msg);
	void parseStringParams(void);

 public:
	double getSensorDeviceBias(int index);
	int getSensorRadius();
	void detectedMovement();

};

#endif				// _APPLICATIONMODULESIMPLE_H_

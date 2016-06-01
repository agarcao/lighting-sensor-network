#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include <map>
#include "CastaliaModule.h"
#include "ResourceManagerMessage_m.h"
#include "Wall.h"

using namespace std;

enum ResoruceManagerTimers {
	PERIODIC_ENERGY_CALCULATION = 1,
};

class ResourceManager: public CastaliaModule {
 private:
	// parameters and variables
	/*--- The .ned file's parameters ---*/
	double sigmaCPUClockDrift;
	double cpuClockDrift;
	double ramSize;
	double baselineNodePower;
	double currentNodePower;
	simtime_t timeOfLastCalculation;
	double periodicEnergyCalculationInterval;

	/*--- Custom class parameters ---*/
	double totalRamData;

	map<int,double> storedPowerConsumptions;

	cMessage *energyMsg;
	bool disabled;

	// .ini Parameters
	int lightIntensity;
	bool nearObstacle;

 protected:
	virtual void initialize();
	virtual void handleMessage(cMessage * msg);
	virtual void finishSpecific();
	void calculateEnergySpent();
	void changeLightIntensity(bool increase);

 public:
	double initialEnergy;
	double remainingEnergy;

	double getCPUClockDrift(void);
	void consumeEnergy(double amount);
	double getSpentEnergy(void);
	void destroyNode(void);
	int RamStore(int numBytes);
	void RamFree(int numBytes);
	int getLightIntensity(void);
};

#endif				// _RESOURCEGENERICMANAGER_H_

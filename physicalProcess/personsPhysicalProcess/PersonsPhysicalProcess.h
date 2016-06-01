/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Yuri Tselishchev                                             *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *  
 ****************************************************************************/

#ifndef _CARSPHYSICALPROCESS_H_
#define _CARSPHYSICALPROCESS_H_

#define SIMTIME_STEP 0.01

#include "CastaliaModule.h"
#include "PhysicalProcessMessage_m.h"

#include "PersonNode.h"
#include "SensorManager.h"
#include "ResourceManager.h"

using namespace std;

typedef struct {
	simtime_t time;
	double x;
	double y;
} sourceSnapshot;

class PersonsPhysicalProcess: public CastaliaModule {
 private:
	bool printDebugInfo;

	sourceSnapshot *sources_snapshots;

	const char *description;
	int numSensorNodes;

	double defaultValue;
	double *valuesTable;

 protected:
	virtual void initialize();
	virtual void handleMessage(cMessage * msg);
	virtual void finishSpecific();
	double calculateScenarioReturnValue(const double &x_coo,
					    const double &y_coo, const simtime_t & stime);
	void readIniFileParameters(void);

 public:
	// Where pointers to sensorNodes gonna be. This is needed to simulate the movement sensor part
    cArray *sensorNodes;

	// Function to Sensor Nodes be added to the Array when they initialized themselves
    void registrySensorNodes(cModule *s);

    // Function to check if Person is detect for any node
    void checkPersonNodeDetection(PersonNode *p);

    // Function to update the GUI when a change occurs
    void updateGUI();
};

#endif

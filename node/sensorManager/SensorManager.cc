#include <boost/lexical_cast.hpp>
#include "SensorManager.h"

Define_Module(SensorManager);

void SensorManager::initialize()
{
	self = getParentModule()->getIndex();

	// get a valid reference to the object of the Resources Manager module so that 
	// we can make direct calls to its public methods instead of using extra
	// messages & message types for tighlty couplped operations.
//      resMgrModule = check_and_cast<ResourceGenericManager*>(getParentModule()->getSubmodule("nodeResourceMgr"));
	nodeMobilityModule = check_and_cast<VirtualMobilityManager*>(getParentModule()->getSubmodule("MobilityManager"));
	disabled = 1;

	parseStringParams();	//read the string params in omnet.ini and initialize the vectors

	sensorlastSampleTime.clear();
	sensorLastValue.clear();
	double theBias;
	for (int i = 0; i < totalSensors; i++) {
		sensorLastValue.push_back(-1);
		sensorlastSampleTime.push_back(-100000.0);

		theBias = normal(0, sensorBiasSigma[i]);	// using rng generator --> "0" 
		sensorBias.push_back(theBias);
	}

	sensorRadius = getParentModule()->par("sensor_radius");
}

void SensorManager::handleMessage(cMessage * msg)
{
	int msgKind = msg->getKind();

	if (disabled && msgKind != NODE_STARTUP) {
		delete msg;
		msg = NULL;	// safeguard
		return;
	}

	switch (msgKind) {

		/*---------------------------------------------------------------------------------------------------
		 * Sent by the Application submodule in order to start/switch-on the Sensor Device Manager submodule.
		 *---------------------------------------------------------------------------------------------------*/
		case NODE_STARTUP:
		{
			disabled = 0;
			trace() << "Received STARTUP MESSAGE";
			break;
		}

		/*
		 * Sent from Application to initiate a new sample request to the physical process.
		 * The message contains information (its index) about the specific sensor device that 
		 * is asking the sample request. Each sensor device has its own sample rate and so its
		 * sample requests occur/are schedules with different intervals that are held inside 
		 * vector minSamplingIntervals[].
		 */
		case SENSOR_READING_MESSAGE:{
			SensorReadingMessage *rcvPacket = check_and_cast<SensorReadingMessage*>(msg);
			int sensorIndex = rcvPacket->getSensorIndex();

			simtime_t currentTime = simTime();
			simtime_t interval = currentTime - sensorlastSampleTime[sensorIndex];
			int getNewSample = (interval < minSamplingIntervals[sensorIndex]) ? 0 : 1;

			if (getNewSample) {	//the last request for sample was more than minSamplingIntervals[sensorIndex] time ago
				PhysicalProcessMessage *requestMsg =
						new PhysicalProcessMessage("sample request", PHYSICAL_PROCESS_SAMPLING);
				requestMsg->setSrcID(self);	//insert information about the ID of the node
				requestMsg->setSensorIndex(sensorIndex);	//insert information about the index of the sensor
				requestMsg->setXCoor(nodeMobilityModule->getLocation().x);
				requestMsg->setYCoor(nodeMobilityModule->getLocation().y);

				// send the request to the physical process (using the appropriate 
				// gate index for the respective sensor device )
				send(requestMsg, "toNodeContainerModule", corrPhyProcess[sensorIndex]);

				// update the remaining energy of the node
				// CONSUME_ENERGY(pwrConsumptionPerDevice[sensorIndex]);
				// +++ will need to change to powerDrawn();

				// update the most recent sample times in sensorlastSampleTime[]
				sensorlastSampleTime[sensorIndex] = currentTime;
			} else {	// send back the old sample value
				//SensorDevMgr_GenericMessage *readingMsg;
				//readingMsg = new SensorDevMgr_GenericMessage("sensor reading", SDM_2_APP_SENSOR_READING);
				//readingMsg->setSensorType(sensorTypes[sensorIndex].c_str());
				//readingMsg->setSensedValue(sensorLastValue[sensorIndex]);
				//readingMsg->setSensorIndex(sensorIndex);
				//send(readingMsg, "toApplicationModule"); //send the sensor reading to the Application module
				rcvPacket->setSensorType(sensorTypes[sensorIndex].c_str());
				rcvPacket->setSensedValue(sensorLastValue[sensorIndex]);
				send(rcvPacket, "toApplicationModule");
				return;
			}

			break;
		}

		/*
		 * Message received by the physical process. It holds a value and the index of the sensor 
		 * that sent the sample request.
		 */
		case PHYSICAL_PROCESS_SAMPLING:
		{
			PhysicalProcessMessage *phyReply = check_and_cast<PhysicalProcessMessage*>(msg);
			int sensorIndex = phyReply->getSensorIndex();
			double theValue = phyReply->getValue();

			// add the sensor's Bias and the random noise 
			theValue += sensorBias[sensorIndex];
			theValue += normal(0, sensorNoiseSigma[sensorIndex], 1);

			// process the limitations of the sensing device (sensitivity, resoultion and saturation)
			if (theValue < sensorSensitivity[sensorIndex])
				theValue = sensorSensitivity[sensorIndex];
			if (theValue > sensorSaturation[sensorIndex])
				theValue = sensorSaturation[sensorIndex];

			theValue = sensorResolution[sensorIndex] * lrint(theValue / sensorResolution[sensorIndex]);
			sensorLastValue[sensorIndex] = theValue;

			SensorReadingMessage *readingMsg =
					new SensorReadingMessage("sensor reading", SENSOR_READING_MESSAGE);
			readingMsg->setSensorType(sensorTypes[sensorIndex].c_str());
			readingMsg->setSensedValue(theValue);
			readingMsg->setSensorIndex(sensorIndex);

			send(readingMsg, "toApplicationModule");	//send the sensor reading to the Application module

			break;
		}

		case OUT_OF_ENERGY:{
			disabled = 1;
			break;
		}

		case DESTROY_NODE:
		{
			disabled = 1;
			send(msg->dup(), "toApplicationModule");
			break;
		}

		default:
		{
			trace() << "WARNING: received packet of unknown type";
			break;
		}
	}

	delete msg;
	msg = NULL;		// safeguard
}

int SensorManager::getSensorRadius()
{
    return this->sensorRadius;
}

void SensorManager::parseStringParams(void)
{
	const char *parameterStr, *token;
	simtime_t sampleInterval;

	//get the physical process index that each sensor device is monitoring
	corrPhyProcess.clear();
	parameterStr = par("corrPhyProcess");
	cStringTokenizer phyTokenizer(parameterStr);
	while ((token = phyTokenizer.nextToken()) != NULL)
		corrPhyProcess.push_back(atoi(token));

	//get the power consumption of each sensor device
	pwrConsumptionPerDevice.clear();
	parameterStr = par("pwrConsumptionPerDevice");
	cStringTokenizer pwrTokenizer(parameterStr);
	while ((token = pwrTokenizer.nextToken()) != NULL)
		pwrConsumptionPerDevice.push_back(((double)atof(token)) / 1000.0f);

	//get the samplerate for each sensor device and calculate the minSamplingIntervals 
	//(that is every how many ms to request a sample from the physical process)
	minSamplingIntervals.clear();
	parameterStr = par("maxSampleRates");
	cStringTokenizer ratesTokenizer(parameterStr);
	while ((token = ratesTokenizer.nextToken()) != NULL) {
		sampleInterval = (double)(1.0f / atof(token));
		minSamplingIntervals.push_back(sampleInterval);
	}

	//get the type of each sensor device (just a description e.g.: light, temperature etc.)
	sensorTypes.clear();
	parameterStr = par("sensorTypes");
	cStringTokenizer typesTokenizer(parameterStr);
	while ((token = typesTokenizer.nextToken()) != NULL) {
		string sensorType(token);
		sensorTypes.push_back(sensorType);
	}

	// get the bias sigmas for each sensor device
	sensorBiasSigma.clear();
	parameterStr = par("devicesBias");
	cStringTokenizer biasSigmaTokenizer(parameterStr);
	while ((token = biasSigmaTokenizer.nextToken()) != NULL)
		sensorBiasSigma.push_back((double)atof(token));

	// get the bias sigmas for each sensor device
	sensorNoiseSigma.clear();
	parameterStr = par("devicesNoise");
	cStringTokenizer noiseSigmaTokenizer(parameterStr);
	while ((token = noiseSigmaTokenizer.nextToken()) != NULL)
		sensorNoiseSigma.push_back((double)atof(token));

	sensorSensitivity.clear();
	parameterStr = par("devicesSensitivity");
	cStringTokenizer sensitivityTokenizer(parameterStr);
	while ((token = sensitivityTokenizer.nextToken()) != NULL)
		sensorSensitivity.push_back((double)atof(token));

	sensorResolution.clear();
	parameterStr = par("devicesResolution");
	cStringTokenizer resolutionTokenizer(parameterStr);
	while ((token = resolutionTokenizer.nextToken()) != NULL)
		sensorResolution.push_back((double)atof(token));

	sensorSaturation.clear();
	parameterStr = par("devicesSaturation");
	cStringTokenizer saturationTokenizer(parameterStr);
	while ((token = saturationTokenizer.nextToken()) != NULL)
		sensorSaturation.push_back((double)atof(token));

	totalSensors = par("numSensingDevices");

	int totalPhyProcesses = gateSize("toNodeContainerModule");

	//check for malformed parameter string in the omnet.ini file
	int aSz, bSz, cSz, dSz, eSz, fSz, mSz, rSz, sSz;
	aSz = (int)pwrConsumptionPerDevice.size();
	bSz = (int)minSamplingIntervals.size();
	cSz = (int)sensorTypes.size();
	dSz = (int)corrPhyProcess.size();
	eSz = (int)sensorBiasSigma.size();
	fSz = (int)sensorNoiseSigma.size();
	mSz = (int)sensorSensitivity.size();
	rSz = (int)sensorResolution.size();
	sSz = (int)sensorSaturation.size();

	if ((totalPhyProcesses < totalSensors) || (aSz != totalSensors)
	    || (bSz != totalSensors) || (cSz != totalSensors)
	    || (dSz != totalSensors) || (eSz != totalSensors)
	    || (fSz != totalSensors) || (mSz != totalSensors)
	    || (rSz != totalSensors) || (sSz != totalSensors))
		opp_error("\n[Sensor Device Manager]: The parameters of the sensor device manager are not initialized correctly in omnet.ini file.");
}

double SensorManager::getSensorDeviceBias(int index)
{
	Enter_Method("getSensorDeviceBias()");
	if ((totalSensors <= index) || (index < 0)) {
		trace() << "WARNING: Sensor index out of bound ( direct method call : getSensorDeviceBias() )";
		return -1.0f;
	} else
		return sensorBiasSigma[index];
}

/***************************************************************
 * This function is called when this sensor detect a movement. *
 **************************************************************/
void SensorManager::detectedMovement()
{
    Enter_Method_Silent("detectedMovement()");

    cout << "Entro no detectedMovement\n";
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Sensor Manager Module] Movement detected. Entering in detectMovement function" << endl;

    // Tenho que mandar msg para a Application (onde está a logica do nó) p/ ele decidir o que fazer.
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Sensor Manager Module] We must send this information to the Application module. Sending..." << endl;

    // 1st - Build the sensor reading msg
    ostringstream s;
    s << "detectPersonBySensor#" << getParentModule()->getIndex();
    string msg = s.str();
    SensorReadingMessage *sensorReadingMsg = new SensorReadingMessage(msg.c_str(), SENSOR_READING_MESSAGE);

    cout << "[detectedMovement] Construi a msg\n";

    // 2st - Send to application
    send(sensorReadingMsg, "toApplicationModule");

    cout << "[detectedMovement] Mandei a msg\n";

    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Sensor Manager Module] Sensor Reading Msg sended." << endl;

    // Check if sensor node is off
    //if(!this->lightIntensity)
    //{
        //ev << "[Sensor Node #" << getParentModule()->getIndex() << "] Light is off. Let turn it on" << endl;
        // TODO: Change the light intensity - In a 1st phase just on/off
        //this->lightIntensity = 1;

        //ev << "[Sensor Node #" << getParentModule()->getIndex() << "] Light is now on. Make the changes in GUI" << endl;
        // Represent this change in the GUI
        //cDisplayString &nodeDS = getParentModule()->getDisplayString();
        //nodeDS.setTagArg("i",0,"status/yellow_25");
    //}

    // We should cancel the changeLightIntensity event, a restart it all over again with the initial ticks
    //ev << "[Sensor Node #" << this->id << "] Send new selfMessage to light diminish it intensity" << endl;

    // TODO: Cancel and Delete the self event (this way we dont need to check if event is already create or not
    //cancelAndDelete(this->selfEvent);

    // TODO: Create new message
    //this->selfEvent = new cMessage("diminishLightIntensity");

    // TODO: Set new timer
    //scheduleAt(simTime() + this->timeToDiminishLightIntensity, this->selfEvent);

    // TODO: Create the Broadcast Msg to send
    //this->broadCastMsg = new BroadcastMsg("broadcastMsg");
    //this->broadCastMsg->setSource(this->id);

    //ev << "[Sensor Node #" << this->id << "] Send the broadcast Msg" << endl;

    // TODO: Broadcast the msg
    //send(this->broadCastMsg, "out");

    cout << "[detectedMovement] Sai daqui\n";
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Sensor Manager Module] Exiting detectMovement function" << endl;
}


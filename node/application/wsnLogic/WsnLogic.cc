#include "WsnLogic.h"

Define_Module(WsnLogic);

void WsnLogic::startup()
{
    maxSampleInterval = ((double)par("maxSampleInterval")) / 1000.0;
    minSampleInterval = ((double)par("minSampleInterval")) / 1000.0;
    currSentSampleSN = 0;
    randomBackoffIntervalFraction = genk_dblrand(0);
    sentOnce = false;

    // Set node ID
    this->self = this->getParentModule()->getIndex();

    this->timeToDiminishLightIntensity = par("timeToDiminishLightIntensity");

    //setTimer(REQUEST_SAMPLE, maxSampleInterval * randomBackoffIntervalFraction);
}

void WsnLogic::timerFiredCallback(int index)
{
    switch (index)
    {
        case WsnLogicTimers::DIMINISH_LIGHT:
        {
            handleSelfEvent();
            break;
        }
    }
}

void WsnLogic::fromNetworkLayer(ApplicationPacket * genericPacket,
         const char *source, double rssi, double lqi)
{
    WsnLogicDataPacket *rcvPacket = check_and_cast<WsnLogicDataPacket*>(genericPacket);
    WsnLogicData theData = rcvPacket->getExtraData();
    if (isSink)
        trace() << "Sink received from: " << theData.nodeID << " \tvalue=" << rcvPacket->getData();
}

void WsnLogic::handleSensorReading(SensorReadingMessage * rcvReading)
{
    // TODO: Receive SensorReadingMsg and do things
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] Enter handleSensorReading function" << endl;

    // 1st - We must say to the resource module to set the light up
    //// Construct the msg
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] We must send msg to Resource Module to set the light up" << endl;

    ostringstream s;
    s << "increaseLightSensor#" << getParentModule()->getIndex();
    string msg = s.str();
    ResourceManagerMessage *resourceManagerMsg = new ResourceManagerMessage(msg.c_str(), RESOURCE_MANAGER_LIGHT);
    resourceManagerMsg->setIncreaseLightIntensity(true);

    //// Send the msg to Resource
    send(resourceManagerMsg, "toResourceManager");
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] Msg to Resource Module sent" << endl;

    // 2nd - We must now create a self event to decrease the light intensity
    //// Cancel and Delete the self event (this way we dont need to check if event is already create or not
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] Create a new self event to diminish the light intensity" << endl;

    //// Set new timer
    setTimer(WsnLogicTimers::DIMINISH_LIGHT, this->timeToDiminishLightIntensity);

    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] Self event to diminish the light intensity schedule" << endl;

    // TODO: 3rd - We need now to broadcast a msg to the neigboor nodes
    WsnLogicData tmpData;
    tmpData.nodeID = (unsigned short)this->self;
    tmpData.locX = mobilityModule->getLocation().x;
    tmpData.locY = mobilityModule->getLocation().y;

    ostringstream s1;
    s1 << "Broadcast msg from node #" << this->self;
    msg = s1.str();

    WsnLogicDataPacket *packet2Net = new WsnLogicDataPacket(msg.c_str(), APPLICATION_PACKET);
    packet2Net->setExtraData(tmpData);
    packet2Net->setSequenceNumber(this->currSentSampleSN);
    this->currSentSampleSN++;

    toNetworkLayer(packet2Net, SINK_NETWORK_ADDRESS);
    this->sentOnce = true;


    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] Exiting handleSensorReading function" << endl;
    // int sensIndex =  rcvReading->getSensorIndex();
    // string sensType(rcvReading->getSensorType());
    //double sensValue = rcvReading->getSensedValue();

    // schedule the TX of the value
    //trace() << "Sensed = " << sensValue;

    //WsnLogicData tmpData;
    //tmpData.nodeID = (unsigned short)self;
    //tmpData.locX = mobilityModule->getLocation().x;
    //tmpData.locY = mobilityModule->getLocation().y;

   // WsnLogicDataPacket *packet2Net =
        //new WsnLogicDataPacket("Value reporting pck", APPLICATION_PACKET);
   // packet2Net->setExtraData(tmpData);
    //packet2Net->setData(sensValue);
    //packet2Net->setSequenceNumber(currSentSampleSN);
    //currSentSampleSN++;

    //toNetworkLayer(packet2Net, SINK_NETWORK_ADDRESS);
    //sentOnce = true;
}

/*
 * Function to handle the self event
 */
void WsnLogic::handleSelfEvent()
{
    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] Receive a diminishLightIntensityEvent. Let send info to Resource Module" << endl;

    // Send msg to Resource Module
    ostringstream s;
    s << "decreaseLightSensor#" << getParentModule()->getIndex();
    string msg = s.str();
    ResourceManagerMessage *resourceManagerMsg = new ResourceManagerMessage(msg.c_str(), RESOURCE_MANAGER_LIGHT);
    send(resourceManagerMsg, "toResourceManager");

    ev << "[Sensor Node #" << getParentModule()->getIndex() << " - Application Module] Msg to diminish the light intensity send to Resource Module" << endl;
}

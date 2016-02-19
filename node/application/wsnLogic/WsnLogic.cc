#include "WsnLogic.h"

Define_Module(WsnLogic);

void WsnLogic::startup()
{
    maxSampleInterval = ((double)par("maxSampleInterval")) / 1000.0;
    minSampleInterval = ((double)par("minSampleInterval")) / 1000.0;
    currSentSampleSN = 0;
    randomBackoffIntervalFraction = genk_dblrand(0);
    sentOnce = false;

    //setTimer(REQUEST_SAMPLE, maxSampleInterval * randomBackoffIntervalFraction);
}

void WsnLogic::timerFiredCallback(int index)
{
    switch (index) {

        case REQUEST_SAMPLE:{
            requestSensorReading();
            setTimer(REQUEST_SAMPLE, maxSampleInterval);
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
    // int sensIndex =  rcvReading->getSensorIndex();
    // string sensType(rcvReading->getSensorType());
    double sensValue = rcvReading->getSensedValue();

    // schedule the TX of the value
    trace() << "Sensed = " << sensValue;

    WsnLogicData tmpData;
    tmpData.nodeID = (unsigned short)self;
    tmpData.locX = mobilityModule->getLocation().x;
    tmpData.locY = mobilityModule->getLocation().y;

    WsnLogicDataPacket *packet2Net =
        new WsnLogicDataPacket("Value reporting pck", APPLICATION_PACKET);
    packet2Net->setExtraData(tmpData);
    packet2Net->setData(sensValue);
    packet2Net->setSequenceNumber(currSentSampleSN);
    currSentSampleSN++;

    toNetworkLayer(packet2Net, SINK_NETWORK_ADDRESS);
    sentOnce = true;
}

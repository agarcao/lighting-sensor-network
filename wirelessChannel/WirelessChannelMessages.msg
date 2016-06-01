message WirelessChannelSignalBegin {
	int nodeID;
	double power_dBm;
	double carrierFreq;
	double bandwidth;
	int modulationType;
	int encodingType;
}

// NOTE: signal end is of type 'packet' so we can encapsulate the 
// actual packet being transfered
packet WirelessChannelSignalEnd {
	int nodeID;		//references the original nodeID in signal begin
}

message WirelessChannelNodeMoveMessage {
	int nodeID;
	double x;
	double y;
	double z;
	double phi;
	double theta;
}


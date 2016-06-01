
enum MacControlMessage_type {
	MAC_BUFFER_FULL = 1;
}

// We need to pass information between MAC and the Radio which is external
// to the packet i.e. not carried by a real packet (e.g., what was the 
// RSSI for the packet received) but this information is related to the
// specific packet. Since information is passed between modules with
// messages/packets, we decided to encode this kind of external info as a 
// separate structure in the packet. The fields there are handled by the
// virtualMAC code, setting a framework of interaction.

struct MacRadioInfoExchange_type {
	double RSSI;
	double LQI;
}

packet MacPacket {
	MacRadioInfoExchange_type macRadioInfoExchange;
	
	int source;
	int destination;
	unsigned int sequenceNumber;	
} 

message MacControlMessage {
	int macControlMessageKind enum (MacControlMessage_type);
}


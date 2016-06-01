
cplusplus {{
#include "MacPacket_m.h"
}}

class MacPacket;

enum TunableMacFrameTypeDef {
	DATA_FRAME = 1;
	BEACON_FRAME = 2;
}

packet TunableMacPacket extends MacPacket {
	int frameType enum (TunableMacFrameTypeDef);
}


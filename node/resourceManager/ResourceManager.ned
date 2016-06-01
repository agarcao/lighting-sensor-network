package wsn_lighting.node.resourceManager;

simple ResourceManager {
 parameters:
	bool collectTraceInfo = default (false);
	double ramSize = default (0.0);			//in kB
	double flashSize = default (0.0);		//in kB
	double flashWriteCost = default (0.0);	//per kB
	double flashReadCost = default (0.0);	//per kB
	double imageSize = default (0.0);		//the space that the OS (e.g. Contiki or TinyOS) occupies in the flash

	string cpuPowerSpeedLevelNames = default ("");
	string cpuPowerPerLevel = default ("");	//spent energy per time unit
	string cpuSpeedPerLevel = default ("");
	int cpuInitialPowerLevel = default (-1);	// index for the cpuPowerLevels array
	double sigmaCPUClockDrift = default (0.00003);	// the standard deviation of the Drift of the CPU

	double initialEnergy = default (10000000);
	// energy of the node in Joules, default value corresponds to two AA batteries
	// source http://www.allaboutbatteries.com/Energy-tables.html

	double baselineNodePower = default (100);	// periodic energy consumption of node, in mWatts
	double periodicEnergyCalculationInterval = default (1000);	// interval for energy calculation, in msec     
	
	//int lightIntensity = default(0); // Says what is the intensity of the actuator (light)
 gates:
	output toSensorDevManager;
	output toApplication;
	output toNetwork;
	output toMac;
	output toRadio;
	input fromApplication;
	input powerConsumption @ directIn;
}


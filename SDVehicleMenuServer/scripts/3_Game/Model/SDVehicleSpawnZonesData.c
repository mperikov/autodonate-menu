class SDVehicleSpawnZonesData
{
	/** 1 = all players may spawn donate vehicles, 0 = globally disabled. */
	int cars_on;
	float SpawnRadius;
	ref array<string> SpawnZones;

	void SDVehicleSpawnZonesData()
	{
		cars_on = 1;
		SpawnRadius = 75;
		SpawnZones = new array<string>;
	}
}

class PluginSDVehicleSpawnZoneManager : PluginBase
{
	private const string CONFIG_PATH = "$profile:KTLoadoutData/Cars/spawnzones.json";

	private float m_Radius;
	private ref TStringArray m_ZoneStrings;
	private bool m_Loaded;
	private bool m_CarsOn;

	void PluginSDVehicleSpawnZoneManager()
	{
		m_ZoneStrings = new TStringArray;
		LoadConfig();
	}

	void ReloadConfig()
	{
		LoadConfig();
	}

	bool IsCarsSpawnEnabled()
	{
		return m_CarsOn;
	}

	bool HasZones()
	{
		return m_Loaded && m_ZoneStrings && m_ZoneStrings.Count() > 0;
	}

	float GetRadius()
	{
		return m_Radius;
	}

	TStringArray GetZoneStrings()
	{
		return m_ZoneStrings;
	}

	void ApplyToStorage(SDVehicleListStorage storage)
	{
		if (!storage)
			return;

		storage.m_SpawnZoneRadius = m_Radius;
		storage.m_SpawnZonePositions = m_ZoneStrings;
		storage.m_CarsOn = m_CarsOn;
	}

	bool IsPlayerInSpawnZone(PlayerBase player)
	{
		if (!player || !HasZones())
			return false;

		return SDVehicleSpawnZoneHelper.IsPositionInZones(player.GetPosition(), m_Radius, m_ZoneStrings);
	}

	int GetDistanceOutsideZone(PlayerBase player)
	{
		if (!player || !HasZones())
			return -1;

		return SDVehicleSpawnZoneHelper.DistanceToNearestZone(player.GetPosition(), m_Radius, m_ZoneStrings);
	}

	private void LoadConfig()
	{
		m_Loaded = false;
		m_CarsOn = true;
		m_Radius = 75;
		m_ZoneStrings.Clear();

		PluginSDLoadoutDataManager.CreateFolders("$profile:KTLoadoutData/Cars/");

		if (!FileExist(CONFIG_PATH))
			CreateDefaultConfig();

		SDVehicleSpawnZonesData data = new SDVehicleSpawnZonesData();
		JsonFileLoader<SDVehicleSpawnZonesData>.JsonLoadFile(CONFIG_PATH, data);

		if (!data)
			return;

		m_CarsOn = data.cars_on != 0;

		if (data.SpawnRadius > 0)
			m_Radius = data.SpawnRadius;

		if (data.SpawnZones)
		{
			foreach (string zoneRaw : data.SpawnZones)
			{
				if (zoneRaw != string.Empty)
					m_ZoneStrings.Insert(zoneRaw);
			}
		}

		m_Loaded = m_ZoneStrings.Count() > 0;
	}

	private void CreateDefaultConfig()
	{
		SDVehicleSpawnZonesData data = new SDVehicleSpawnZonesData();
		data.cars_on = 1;
		data.SpawnRadius = 75;
		data.SpawnZones.Insert("7505.0 293.0 5582.0");
		data.SpawnZones.Insert("12001.0 55.0 9055.0");
		data.SpawnZones.Insert("13444.0 5.0 6073.0");
		data.SpawnZones.Insert("2697.0 206.0 5666.0");
		data.SpawnZones.Insert("10440.0 6.0 2276.0");

		JsonFileLoader<SDVehicleSpawnZonesData>.JsonSaveFile(CONFIG_PATH, data);
	}
}

PluginSDVehicleSpawnZoneManager GetSDVehicleSpawnZoneManager()
{
	return PluginSDVehicleSpawnZoneManager.Cast(GetPlugin(PluginSDVehicleSpawnZoneManager));
}

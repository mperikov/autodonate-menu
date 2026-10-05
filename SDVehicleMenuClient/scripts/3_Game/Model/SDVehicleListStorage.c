class SDVehicleListStorage
{
	ref TStringArray m_Vehicles;
	/** @deprecated Legacy FIFO list; kept for script compile compatibility. */
	ref TStringArray m_SpawnedClassNames;
	/** Slot indices with a live spawned vehicle this restart (synced from server). */
	ref TIntArray m_SpawnedSlotIndices;
	/** Synced from server spawnzones.json for client-side hints. */
	float m_SpawnZoneRadius;
	ref TStringArray m_SpawnZonePositions;
	bool m_CarsOn;

	void SDVehicleListStorage()
	{
		m_Vehicles = new TStringArray;
		m_SpawnedClassNames = new TStringArray;
		m_SpawnedSlotIndices = new TIntArray;
		m_SpawnZoneRadius = 0;
		m_SpawnZonePositions = new TStringArray;
		m_CarsOn = true;
	}

	bool IsCarsSpawnEnabled()
	{
		return m_CarsOn;
	}

	bool HasSpawnZones()
	{
		return m_SpawnZonePositions && m_SpawnZonePositions.Count() > 0 && m_SpawnZoneRadius > 0;
	}

	bool HasVehicles()
	{
		return m_Vehicles && m_Vehicles.Count() > 0;
	}

	bool IsValidSlot(int slotIndex)
	{
		return m_Vehicles && slotIndex >= 0 && slotIndex < m_Vehicles.Count();
	}

	string GetVehicleAt(int slotIndex)
	{
		if (!IsValidSlot(slotIndex))
			return string.Empty;

		return m_Vehicles[slotIndex];
	}

	bool Contains(string className)
	{
		if (!m_Vehicles || className == string.Empty)
			return false;

		return m_Vehicles.Find(className) != -1;
	}

	bool IsSlotConsumedThisRestart(int slotIndex)
	{
		if (!IsValidSlot(slotIndex) || !m_SpawnedSlotIndices)
			return false;

		return m_SpawnedSlotIndices.Find(slotIndex) != -1;
	}
}

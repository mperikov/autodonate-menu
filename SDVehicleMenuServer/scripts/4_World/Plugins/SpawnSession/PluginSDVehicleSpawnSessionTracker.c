/** Per slot: tracks spawned vehicle entity; slot is free again when entity is gone. */
class PluginSDVehicleSpawnSessionTracker : PluginBase
{
	private ref map<string, ref map<int, EntityAI>> m_SpawnedVehiclesByPlainId;

	void PluginSDVehicleSpawnSessionTracker()
	{
		m_SpawnedVehiclesByPlainId = new map<string, ref map<int, EntityAI>>;
	}

	void AnnotateStorage(PlayerBase player, SDVehicleListStorage storage)
	{
		if (!storage || !player || !player.GetIdentity())
			return;

		if (storage.m_SpawnedSlotIndices)
			storage.m_SpawnedSlotIndices.Clear();

		if (storage.m_SpawnedClassNames)
			storage.m_SpawnedClassNames.Clear();

		string id = player.GetIdentity().GetPlainId();
		if (!m_SpawnedVehiclesByPlainId.Contains(id))
			return;

		map<int, EntityAI> slots = m_SpawnedVehiclesByPlainId.Get(id);
		if (!slots)
			return;

		for (int i = 0; i < slots.Count(); i++)
		{
			int slotIndex = slots.GetKey(i);
			EntityAI vehicle = slots.GetElement(i);

			if (IsSpawnedVehicleAlive(vehicle))
			{
				if (storage.m_SpawnedSlotIndices)
					storage.m_SpawnedSlotIndices.Insert(slotIndex);

				if (storage.m_SpawnedClassNames)
					storage.m_SpawnedClassNames.Insert(storage.GetVehicleAt(slotIndex));
			}
		}
	}

	bool IsSlotConsumedThisRestart(PlayerBase player, SDVehicleListStorage storage, int slotIndex)
	{
		if (!storage || !storage.IsValidSlot(slotIndex) || !player || !player.GetIdentity())
			return true;

		string id = player.GetIdentity().GetPlainId();
		if (!m_SpawnedVehiclesByPlainId.Contains(id))
			return false;

		map<int, EntityAI> slots = m_SpawnedVehiclesByPlainId.Get(id);
		if (!slots || !slots.Contains(slotIndex))
			return false;

		return IsSpawnedVehicleAlive(slots.Get(slotIndex));
	}

	void RegisterSuccessfulSpawn(PlayerBase player, int slotIndex, EntityAI vehicle)
	{
		if (!player || !player.GetIdentity() || !vehicle || slotIndex < 0)
			return;

		string id = player.GetIdentity().GetPlainId();
		map<int, EntityAI> slots;

		if (!m_SpawnedVehiclesByPlainId.Contains(id))
		{
			slots = new map<int, EntityAI>;
			m_SpawnedVehiclesByPlainId.Set(id, slots);
		}
		else
		{
			slots = m_SpawnedVehiclesByPlainId.Get(id);
		}

		slots.Set(slotIndex, vehicle);
	}

	private bool IsSpawnedVehicleAlive(EntityAI vehicle)
	{
		return vehicle && !vehicle.IsSetForDeletion();
	}
}

PluginSDVehicleSpawnSessionTracker GetSDVehicleSpawnSessionTracker()
{
	return PluginSDVehicleSpawnSessionTracker.Cast(GetPlugin(PluginSDVehicleSpawnSessionTracker));
}

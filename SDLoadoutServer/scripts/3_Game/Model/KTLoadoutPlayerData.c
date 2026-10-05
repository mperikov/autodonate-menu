class KTLoadoutGearSlotData
{
	string SlotName;
	ref TStringArray Items;

	void KTLoadoutGearSlotData()
	{
		Items = new TStringArray;
	}

	bool HasItem()
	{
		return Items && Items.Count() > 0 && Items[0] != string.Empty;
	}

	string GetFirstItem()
	{
		if (!HasItem())
			return string.Empty;

		return Items[0];
	}
}

class KTLoadoutPlayerData
{
	ref array<ref KTLoadoutGearSlotData> Gear;
	ref TStringArray Locations;
	ref TStringArray ExtraItems;

	void KTLoadoutPlayerData()
	{
		Gear = new array<ref KTLoadoutGearSlotData>;
		Locations = new TStringArray;
		ExtraItems = new TStringArray;
	}

	bool HasGear()
	{
		return Gear && Gear.Count() > 0;
	}

	bool HasLocation(string locationId)
	{
		if (!Locations || locationId == string.Empty)
			return false;

		return Locations.Find(locationId) != -1;
	}
}

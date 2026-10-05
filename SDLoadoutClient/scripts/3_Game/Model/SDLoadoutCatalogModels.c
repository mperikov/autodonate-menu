class SDLoadoutGearEntry
{
	string SlotName;
	string Item;
}

class SDLoadoutSlotCatalogEntry
{
	string SlotName;
	string Label;
	ref TStringArray Items;

	void SDLoadoutSlotCatalogEntry()
	{
		Items = new TStringArray;
	}

	bool HasItems()
	{
		return Items && Items.Count() > 0;
	}
}

class SDLoadoutCatalogData
{
	ref TStringArray Characters;
	ref array<ref SDLoadoutSlotCatalogEntry> Slots;
	ref SDLoadoutSelection DefaultSelection;

	void SDLoadoutCatalogData()
	{
		Characters = new TStringArray;
		Slots = new array<ref SDLoadoutSlotCatalogEntry>;
		DefaultSelection = null;
	}

	bool IsValid()
	{
		return Slots && Slots.Count() > 0;
	}

	SDLoadoutSlotCatalogEntry GetSlotByName(string slotName)
	{
		if (!Slots || slotName == string.Empty)
			return null;

		foreach (SDLoadoutSlotCatalogEntry entry : Slots)
		{
			if (entry && entry.SlotName == slotName)
				return entry;
		}

		return null;
	}
}

class SDLoadoutSelection
{
	string Character;
	ref array<ref SDLoadoutGearEntry> Gear;

	void SDLoadoutSelection()
	{
		Character = string.Empty;
		Gear = new array<ref SDLoadoutGearEntry>;
	}

	string GetItemForSlot(string slotName)
	{
		if (!Gear || slotName == string.Empty)
			return string.Empty;

		string key = SDLoadoutSlotHelper.NormalizeSlotKey(slotName);

		foreach (SDLoadoutGearEntry entry : Gear)
		{
			if (entry && SDLoadoutSlotHelper.NormalizeSlotKey(entry.SlotName) == key)
				return entry.Item;
		}

		return string.Empty;
	}

	void SetItemForSlot(string slotName, string item)
	{
		if (slotName == string.Empty)
			return;

		string key = SDLoadoutSlotHelper.NormalizeSlotKey(slotName);

		foreach (SDLoadoutGearEntry entry : Gear)
		{
			if (entry && SDLoadoutSlotHelper.NormalizeSlotKey(entry.SlotName) == key)
			{
				entry.SlotName = key;
				entry.Item = item;
				return;
			}
		}

		SDLoadoutGearEntry created = new SDLoadoutGearEntry();
		created.SlotName = key;
		created.Item = item;
		Gear.Insert(created);
	}
}

class SDLoadoutClientPreset
{
	string Name;
	string Character;
	ref array<ref SDLoadoutGearEntry> Gear;

	void SDLoadoutClientPreset()
	{
		Name = string.Empty;
		Character = string.Empty;
		Gear = new array<ref SDLoadoutGearEntry>;
	}

	SDLoadoutSelection ToSelection()
	{
		SDLoadoutSelection selection = new SDLoadoutSelection();

		if (!Gear)
			return selection;

		foreach (SDLoadoutGearEntry entry : Gear)
		{
			if (!entry || entry.SlotName == "CHARACTER")
				continue;

			SDLoadoutGearEntry copy = new SDLoadoutGearEntry();
			copy.SlotName = entry.SlotName;
			copy.Item = entry.Item;
			selection.Gear.Insert(copy);
		}

		return selection;
	}

	void CopyFromSelection(string presetName, SDLoadoutSelection selection)
	{
		Name = presetName;
		Character = string.Empty;
		Gear.Clear();

		if (!selection || !selection.Gear)
			return;

		foreach (SDLoadoutGearEntry entry : selection.Gear)
		{
			if (!entry || entry.SlotName == "CHARACTER")
				continue;

			SDLoadoutGearEntry copy = new SDLoadoutGearEntry();
			copy.SlotName = entry.SlotName;
			copy.Item = entry.Item;
			Gear.Insert(copy);
		}
	}
}

class SDLoadoutClientPresetFile
{
	ref array<ref SDLoadoutClientPreset> Presets;
	string LastSelectedPresetName;

	void SDLoadoutClientPresetFile()
	{
		Presets = new array<ref SDLoadoutClientPreset>;
		LastSelectedPresetName = string.Empty;
	}
}

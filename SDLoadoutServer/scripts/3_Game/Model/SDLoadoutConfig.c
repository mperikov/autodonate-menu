class SDLoadoutConfig
{
	ref TStringArray DefaultItems;
	ref TStringArray Characters;
	ref array<ref SDLoadoutSlotCatalogEntry> SlotCatalog;
	ref array<ref array<ref SDLoadoutGearEntry>> Gear;

	void SDLoadoutConfig()
	{
		DefaultItems = new TStringArray;
		Characters = new TStringArray;
		SlotCatalog = new array<ref SDLoadoutSlotCatalogEntry>;
		Gear = new array<ref array<ref SDLoadoutGearEntry>>;
	}

	bool HasGear()
	{
		return Gear && Gear.Count() > 0;
	}

	bool HasDefaultItems()
	{
		return DefaultItems && DefaultItems.Count() > 0;
	}

	bool HasSlotCatalog()
	{
		return SlotCatalog && SlotCatalog.Count() > 0;
	}

	array<ref SDLoadoutGearEntry> GetRandomLoadout()
	{
		if (!HasGear())
			return null;

		return Gear.GetRandomElement();
	}
}

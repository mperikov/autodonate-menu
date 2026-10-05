class SDLoadoutSelectionValidator
{
	private static const string CHARACTER_SLOT = "CHARACTER";
	private static const string SD80_CLASS_SUFFIX = "_80";

	static ref SDLoadoutSelection Validate(PlayerIdentity identity, SDLoadoutSelection submitted)
	{
		if (!GetGame().IsServer() || !identity || !submitted)
			return null;

		ref SDLoadoutCatalogData catalog = SDLoadoutCatalogBuilder.BuildForIdentity(identity);
		if (!catalog || !catalog.IsValid())
		{
			Print("[SDLoadout] Cannot validate loadout for " + identity.GetName() + " — catalog is empty.");
			return null;
		}

		SDLoadoutSelection sanitized = new SDLoadoutSelection();
		int rejectedCount = 0;

		if (!submitted.Gear)
			return sanitized;

		foreach (SDLoadoutGearEntry entry : submitted.Gear)
		{
			if (!entry)
				continue;

			string slotName = NormalizeSubmittedSlot(entry.SlotName);
			if (slotName == string.Empty || slotName == CHARACTER_SLOT)
				continue;

			SDLoadoutSlotCatalogEntry slotEntry = catalog.GetSlotByName(slotName);
			if (!slotEntry)
			{
				rejectedCount++;
				Print("[SDLoadout] Rejected unknown slot '" + entry.SlotName + "' from " + identity.GetName());
				continue;
			}

			string itemClass = entry.Item;
			if (itemClass == string.Empty || itemClass == "None")
			{
				InsertGearEntry(sanitized, slotName, "None");
				continue;
			}

			if (!IsItemAllowedInSlot(slotEntry, itemClass))
			{
				rejectedCount++;
				Print("[SDLoadout] Rejected item '" + itemClass + "' for slot '" + slotName + "' from " + identity.GetName());
				InsertGearEntry(sanitized, slotName, "None");
				continue;
			}

			string spawnClass = ResolveSpawnClass(itemClass);
			if (!g_Game.ConfigIsExisting("CfgVehicles " + spawnClass))
			{
				rejectedCount++;
				Print("[SDLoadout] Rejected missing class '" + spawnClass + "' for slot '" + slotName + "' from " + identity.GetName());
				InsertGearEntry(sanitized, slotName, "None");
				continue;
			}

			InsertGearEntry(sanitized, slotName, itemClass);
		}

		if (rejectedCount > 0)
			Print("[SDLoadout] Sanitized loadout for " + identity.GetName() + " — rejected entries: " + rejectedCount.ToString());

		return sanitized;
	}

	protected static string NormalizeSubmittedSlot(string slotName)
	{
		if (slotName == string.Empty)
			return string.Empty;

		return SDLoadoutCatalogBuilder.NormalizeCatalogSlotName(SDLoadoutSlotHelper.NormalizeSlotKey(slotName));
	}

	protected static bool IsItemAllowedInSlot(SDLoadoutSlotCatalogEntry slotEntry, string itemClass)
	{
		if (!slotEntry || !slotEntry.Items)
			return false;

		if (slotEntry.Items.Find(itemClass) != -1)
			return true;

		if (EndsWithSd80Suffix(itemClass))
		{
			string baseClass = StripSd80Suffix(itemClass);
			if (baseClass != string.Empty && slotEntry.Items.Find(baseClass) != -1)
				return true;
		}

		string sd80Class = itemClass + SD80_CLASS_SUFFIX;
		if (slotEntry.Items.Find(sd80Class) != -1)
			return true;

		return false;
	}

	protected static string ResolveSpawnClass(string itemClass)
	{
		if (EndsWithSd80Suffix(itemClass))
		{
			string baseClass = StripSd80Suffix(itemClass);
			if (baseClass != string.Empty && g_Game.ConfigIsExisting("CfgVehicles " + itemClass))
				return itemClass;

			if (baseClass != string.Empty)
				return baseClass;
		}

		return itemClass;
	}

	protected static bool EndsWithSd80Suffix(string itemClass)
	{
		int len = itemClass.Length();
		int suffixLen = SD80_CLASS_SUFFIX.Length();
		if (len < suffixLen)
			return false;

		return itemClass.Substring(len - suffixLen, suffixLen) == SD80_CLASS_SUFFIX;
	}

	protected static string StripSd80Suffix(string itemClass)
	{
		if (!EndsWithSd80Suffix(itemClass))
			return itemClass;

		return itemClass.Substring(0, itemClass.Length() - SD80_CLASS_SUFFIX.Length());
	}

	protected static void InsertGearEntry(SDLoadoutSelection selection, string slotName, string itemClass)
	{
		if (!selection)
			return;

		if (!selection.Gear)
			selection.Gear = new array<ref SDLoadoutGearEntry>;

		foreach (SDLoadoutGearEntry existing : selection.Gear)
		{
			if (existing && existing.SlotName == slotName)
			{
				existing.Item = itemClass;
				return;
			}
		}

		SDLoadoutGearEntry created = new SDLoadoutGearEntry();
		created.SlotName = slotName;
		created.Item = itemClass;
		selection.Gear.Insert(created);
	}
}

class SDLoadoutPlayerProfile
{
	ref SDLoadoutSelection LastSelection;

	void SDLoadoutPlayerProfile()
	{
		LastSelection = null;
	}
}

class SDLoadoutPlayerStorage
{
	private static const string PLAYERS_DIR = "$profile:SDLoadout/players/";

	static void EnsureDirectory()
	{
		if (!FileExist(PLAYERS_DIR))
			MakeDirectory(PLAYERS_DIR);
	}

	static string GetPath(string steamId)
	{
		return PLAYERS_DIR + steamId + ".json";
	}

	static void SaveLastSelection(string steamId, SDLoadoutSelection selection)
	{
		if (!GetGame().IsServer() || steamId == string.Empty || !selection)
			return;

		EnsureDirectory();

		SDLoadoutPlayerProfile profile = new SDLoadoutPlayerProfile();
		profile.LastSelection = CopySelection(selection);

		JsonFileLoader<SDLoadoutPlayerProfile>.JsonSaveFile(GetPath(steamId), profile);
	}

	static SDLoadoutSelection LoadLastSelection(string steamId)
	{
		if (!GetGame().IsServer() || steamId == string.Empty)
			return null;

		if (!FileExist(GetPath(steamId)))
			return null;

		SDLoadoutPlayerProfile profile = null;
		JsonFileLoader<SDLoadoutPlayerProfile>.JsonLoadFile(GetPath(steamId), profile);

		if (!profile || !profile.LastSelection || !profile.LastSelection.Gear || profile.LastSelection.Gear.Count() == 0)
			return null;

		return CopySelection(profile.LastSelection);
	}

	protected static SDLoadoutSelection CopySelection(SDLoadoutSelection source)
	{
		SDLoadoutSelection copy = new SDLoadoutSelection();
		if (!source || !source.Gear)
			return copy;

		copy.Character = source.Character;

		foreach (SDLoadoutGearEntry entry : source.Gear)
		{
			if (!entry)
				continue;

			SDLoadoutGearEntry gearCopy = new SDLoadoutGearEntry();
			gearCopy.SlotName = entry.SlotName;
			gearCopy.Item = entry.Item;
			copy.Gear.Insert(gearCopy);
		}

		return copy;
	}
}

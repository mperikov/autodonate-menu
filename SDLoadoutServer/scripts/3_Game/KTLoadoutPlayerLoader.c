class KTLoadoutPlayerLoader
{
	private static const string PLAYERS_DIR = "$profile:KTLoadouts/Players/";
	private static const string LEGACY_PLAYERS_DIR = "$profile:KTLoadoutData/Players/";
	private static const string PLAYER_PATH = "$profile:KTLoadouts/Players/%1.json";
	private static const string LEGACY_PLAYER_PATH = "$profile:KTLoadoutData/Players/%1.json";

	static bool LoadBySteamId(string steamId, out KTLoadoutPlayerData data)
	{
		data = null;

		if (steamId == string.Empty)
			return false;

		EnsurePlayersDirectory();

		string path = string.Format(PLAYER_PATH, steamId);
		if (!FileExist(path))
			path = string.Format(LEGACY_PLAYER_PATH, steamId);

		if (!FileExist(path))
			return false;

		KTLoadoutPlayerData loaded = null;
		JsonFileLoader<KTLoadoutPlayerData>.JsonLoadFile(path, loaded);

		if (!loaded)
		{
			ErrorEx("[SDLoadout] Failed to load player KT loadout (" + steamId + "): " + path);
			return false;
		}

		if (!loaded)
			return false;

		data = loaded;
		return true;
	}

	private static void EnsurePlayersDirectory()
	{
		if (!FileExist("$profile:KTLoadouts/"))
			MakeDirectory("$profile:KTLoadouts/");

		if (!FileExist(PLAYERS_DIR))
			MakeDirectory(PLAYERS_DIR);

		if (!FileExist("$profile:KTLoadoutData/"))
			MakeDirectory("$profile:KTLoadoutData/");

		if (!FileExist(LEGACY_PLAYERS_DIR))
			MakeDirectory(LEGACY_PLAYERS_DIR);
	}
}

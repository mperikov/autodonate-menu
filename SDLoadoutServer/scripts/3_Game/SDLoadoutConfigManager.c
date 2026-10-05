class SDLoadoutConfigManager
{
	private static const string CONFIG_DIR = "$profile:SDLoadout/";
	private static const string CONFIG_PATH = "$profile:SDLoadout/loadouts.json";

	private static bool s_Initialized;
	private static bool s_ConfigLoaded;
	private static ref SDLoadoutConfig s_Config;
	private static string s_CachedRawContent;

	static void Init()
	{
		if (s_Initialized)
			return;

		s_Initialized = true;
		s_Config = new SDLoadoutConfig();
		LoadConfigIfChanged();
	}

	static bool IsReady()
	{
		return s_ConfigLoaded && s_Config && (s_Config.HasGear() || s_Config.HasSlotCatalog() || s_Config.HasDefaultItems());
	}

	static ref SDLoadoutConfig GetConfig()
	{
		return s_Config;
	}

	static void LoadConfigIfChanged()
	{
		EnsureConfigDirectory();

		if (!FileExist(CONFIG_PATH))
		{
			if (!s_ConfigLoaded)
				Print("[SDLoadout] Config not found: " + CONFIG_PATH);
			return;
		}

		string raw = ReadConfigRaw(CONFIG_PATH);
		if (raw == s_CachedRawContent)
			return;

		if (!ParseConfig())
		{
			ErrorEx("[SDLoadout] Failed to load config, keeping previous version.");
			s_CachedRawContent = raw;
			return;
		}

		s_CachedRawContent = raw;
	}

	private static bool ParseConfig()
	{
		SDLoadoutConfig loaded = null;
		JsonFileLoader<SDLoadoutConfig>.JsonLoadFile(CONFIG_PATH, loaded);

		if (!loaded)
		{
			ErrorEx("[SDLoadout] Failed to parse config: " + CONFIG_PATH);
			return false;
		}

		if (!loaded || (!loaded.HasGear() && !loaded.HasSlotCatalog() && !loaded.HasDefaultItems()))
		{
			ErrorEx("[SDLoadout] Config has no gear presets, slot catalog, or default items.");
			return false;
		}

		s_Config = loaded;
		s_ConfigLoaded = true;

		int gearCount = 0;
		if (s_Config.Gear)
			gearCount = s_Config.Gear.Count();

		int defaultCount = 0;
		if (s_Config.DefaultItems)
			defaultCount = s_Config.DefaultItems.Count();

		Print("[SDLoadout] Reloaded " + gearCount.ToString() + " gear preset(s), " + defaultCount.ToString() + " default item(s).");
		return true;
	}

	private static string ReadConfigRaw(string path)
	{
		FileHandle file = OpenFile(path, FileMode.READ);
		if (file == 0)
			return string.Empty;

		string content = string.Empty;
		string line;
		while (FGets(file, line) > 0)
		{
			if (content != string.Empty)
				content += "\n";

			content += line;
		}

		CloseFile(file);
		return content;
	}

	private static void EnsureConfigDirectory()
	{
		if (FileExist(CONFIG_DIR))
			return;

		MakeDirectory(CONFIG_DIR);
	}
}

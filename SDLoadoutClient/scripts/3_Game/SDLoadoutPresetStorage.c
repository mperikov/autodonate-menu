class SDLoadoutPresetStorage
{
	private static const string PRESET_DIR = "$profile:SDLoadout/";
	private static const string PRESET_PATH = "$profile:SDLoadout/client_presets.json";

	static void EnsureDirectory()
	{
		if (!FileExist(PRESET_DIR))
			MakeDirectory(PRESET_DIR);
	}

	static ref SDLoadoutClientPresetFile Load()
	{
		SDLoadoutClientPresetFile empty = new SDLoadoutClientPresetFile();

		if (!GetGame().IsClient())
			return empty;

		EnsureDirectory();

		if (!FileExist(PRESET_PATH))
			return empty;

		SDLoadoutClientPresetFile loaded = null;
		JsonFileLoader<SDLoadoutClientPresetFile>.JsonLoadFile(PRESET_PATH, loaded);

		if (!loaded)
		{
			ErrorEx("[SDLoadout] Failed to load client presets from " + PRESET_PATH);
			return empty;
		}

		return loaded;
	}

	static void Save(SDLoadoutClientPresetFile file)
	{
		if (!GetGame().IsClient())
			return;

		EnsureDirectory();

		SDLoadoutClientPresetFile presetFile = file;
		if (!presetFile)
			presetFile = new SDLoadoutClientPresetFile();

		JsonFileLoader<SDLoadoutClientPresetFile>.JsonSaveFile(PRESET_PATH, presetFile);
	}

	static string GeneratePresetName(SDLoadoutClientPresetFile file)
	{
		int index = 1;
		if (file && file.Presets)
			index = file.Presets.Count() + 1;

		return "preset" + index.ToString();
	}

	static SDLoadoutSelection BuildLastPresetSelection()
	{
		SDLoadoutClientPresetFile file = Load();
		if (!file || file.LastSelectedPresetName == string.Empty)
			return null;

		if (!file.Presets)
			return null;

		foreach (SDLoadoutClientPreset preset : file.Presets)
		{
			if (preset && preset.Name == file.LastSelectedPresetName)
				return preset.ToSelection();
		}

		return null;
	}
}

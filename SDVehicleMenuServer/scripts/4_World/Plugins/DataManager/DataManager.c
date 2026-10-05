class PluginSDLoadoutDataManager : PluginBase
{
	private const string DB_DIR = "$profile:KTLoadoutData/Players/";
	private const string DB_TEMPLATE_PATH = "$profile:KTLoadoutData/Players/%1.json";

	void PluginSDLoadoutDataManager()
	{
		CreateFolders("$profile:KTLoadoutData/");
		CreateFolders(DB_DIR);
	}

	SDVehicleListStorage GetPlayerVehicleStorage(PlayerBase player)
	{
		SDVehicleListStorage storage = new SDVehicleListStorage();
		string path = GetRecordPath(player);

		if (!path || !FileExist(path))
			return storage;

		SDLoadoutPlayerData data = new SDLoadoutPlayerData();
		JsonFileLoader<SDLoadoutPlayerData>.JsonLoadFile(path, data);

		if (data && data.ExtraItems)
		{
			foreach (string className : data.ExtraItems)
			{
				if (className != string.Empty)
					storage.m_Vehicles.Insert(className);
			}
		}

		return storage;
	}

	private string GetRecordPath(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return string.Empty;

		return string.Format(DB_TEMPLATE_PATH, player.GetIdentity().GetPlainId());
	}

	static void CreateFolders(string fullpath)
	{
		if (FileExist(fullpath))
			return;

		string sep = "\\";
		string root = "$profile:";
		string clientRoot = "$saves:";
		if (fullpath.Contains(clientRoot))
			root = clientRoot;

		string path = fullpath;
		path.Replace("/", sep);
		path.Replace(root, string.Empty);
		path.Replace(clientRoot, string.Empty);

		TStringArray subfolders = {};
		path.Split(sep, subfolders);

		path = root;
		foreach (string subfolder : subfolders)
		{
			path += sep + subfolder;
			if (!FileExist(path))
				MakeDirectory(path);
		}
	}
}

PluginSDLoadoutDataManager GetSDLoadoutDataManager()
{
	return PluginSDLoadoutDataManager.Cast(GetPlugin(PluginSDLoadoutDataManager));
}
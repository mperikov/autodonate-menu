class SDVehicleLocalization
{
	static const string MENU_TITLE = "#STR_SDVM_MENU_TITLE";
	static const string EMPTY_LIST = "#STR_SDVM_EMPTY_LIST";
	static const string SELECT_VEHICLE = "#STR_SDVM_SELECT_VEHICLE";
	static const string BTN_SPAWN = "#STR_SDVM_BTN_SPAWN";
	static const string NOT_SELECTED = "#STR_SDVM_NOT_SELECTED";
	static const string OPEN_COOLDOWN = "#STR_SDVM_OPEN_COOLDOWN";
	static const string WAIT_SERVER = "#STR_SDVM_WAIT_SERVER";
	static const string INPUT_OPEN_MENU = "#STR_SDVM_INPUT_OPEN_MENU";
	static const string SUCCESS = "#STR_SDVM_SUCCESS";
	static const string EMPTY_REQUEST = "#STR_SDVM_EMPTY_REQUEST";
	static const string VEHICLE_404 = "#STR_SDVM_VEHICLE_404";
	static const string NO_VEHICLES = "#STR_SDVM_NO_VEHICLES";
	static const string PLAYER_DEAD = "#STR_SDVM_PLAYER_DEAD";
	static const string UNCONSCIOUS = "#STR_SDVM_UNCONSCIOUS";
	static const string INCAR = "#STR_SDVM_INCAR";
	static const string INWATER = "#STR_SDVM_INWATER";
	static const string NOT_ENOUGH_SPACE = "#STR_SDVM_NOT_ENOUGH_SPACE";
	static const string OUT_OF_SPAWN_ZONE = "#STR_SDVM_OUT_OF_SPAWN_ZONE";
	static const string SPAWN_ZONES_NOT_CONFIGURED = "#STR_SDVM_SPAWN_ZONES_NOT_CONFIGURED";
	static const string CARS_SPAWN_DISABLED = "#STR_SDVM_CARS_SPAWN_DISABLED";
	static const string ALREADY_SPAWNED = "#STR_SDVM_ALREADY_SPAWNED";
	static const string INVALID_CLASS = "#STR_SDVM_INVALID_CLASS";
	static const string UNKNOWN_ERROR = "#STR_SDVM_UNKNOWN_ERROR";

	static string T(string key)
	{
		if (key == string.Empty)
			return string.Empty;

		string result = Widget.TranslateString(key);
		if (!IsUntranslated(result, key))
			return result;

		string altKey = key;
		if (key.Length() > 1 && key.Substring(0, 1) == "#")
			altKey = key.Substring(1, key.Length() - 1);

		if (altKey != key)
		{
			result = Widget.TranslateString(altKey);
			if (!IsUntranslated(result, altKey))
				return result;
		}

		return key;
	}

	private static bool IsUntranslated(string result, string key)
	{
		if (result == string.Empty)
			return true;

		if (result == key)
			return true;

		if (result.IndexOf("STR_SDVM_") == 0)
			return true;

		return false;
	}
}

class SDLoadoutDisplayNames
{
	static string GetItemDisplayName(string className)
	{
		if (className == string.Empty || className == "None")
			return "None";

		string displayName;
		if (GetGame().ConfigGetText("CfgVehicles " + className + " displayName", displayName) && displayName != string.Empty)
			return displayName;

		return className;
	}

	static string GetCharacterDisplayName(string className)
	{
		if (className == string.Empty || className == "None")
			return "None";

		string displayName = GetItemDisplayName(className);
		if (displayName != className)
			return displayName;

		if (className.Contains("Judy"))
			return "Judy";
		if (className.Contains("Mirek"))
			return "Mirek";

		int underscore = className.IndexOf("_");
		if (underscore >= 0 && underscore < className.Length() - 1)
			return className.Substring(underscore + 1, className.Length() - underscore - 1);

		return className;
	}
}

class CfgMods
{
	class SDLoadoutServer
	{
		dir = "SDLoadoutServer";
		hideName = 0;
		hidePicture = 0;
		name = "SD Loadout Server";
		credits = "";
		author = "";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"SDLoadoutServer/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"SDLoadoutServer/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"SDLoadoutServer/scripts/5_Mission"};
			};
		};
	};
};

class CfgPatches
{
	class SDLoadoutServer_Scripts
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts",
			"SDLoadoutClient_Scripts"
		};
	};
};

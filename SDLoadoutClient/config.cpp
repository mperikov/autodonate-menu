class CfgPatches
{
	class SDLoadoutClient_Scripts
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"DZ_Scripts",
			"SpawnSelect_Scripts"
		};
	};
};

class CfgMods
{
	class SDLoadoutClient
	{
		dir = "SDLoadoutClient";
		hideName = 0;
		hidePicture = 0;
		name = "SD Loadout Client";
		credits = "";
		author = "";
		version = "1.0";
		extra = 0;
		type = "mod";
		dependencies[] = {"Game", "World", "Mission"};

		class defs
		{
			class widgetStyles
			{
				files[] = {"SDLoadoutClient/gui/looknfeel/SDLoadout.styles"};
			};
			class gameScriptModule
			{
				value = "";
				files[] = {"SDLoadoutClient/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"SDLoadoutClient/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"SDLoadoutClient/scripts/5_Mission"};
			};
		};
	};
};

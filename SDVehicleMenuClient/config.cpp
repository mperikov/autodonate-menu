class CfgMods
{
	class SDVehicleMenuClient
	{
		dir = "SDVehicleMenuClient";
		hideName = 0;
		hidePicture = 0;
		name = "SDVehicle Menu Client";
		credits = "";
		author = "";
		authorID = "";
		inputs = "SDVehicleMenuClient/assets/inputs/modded_Inputs.xml";
		version = "1.0";
		extra = 0;
		type = "mod";
		defines[] =
		{
			"SD_VEHICLE_MENU_CLIENT"
		};
		dependencies[] =
		{
			"Game",
			"World",
			"Mission"
		};

		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] =
				{
					"SDVehicleMenuClient/scripts/3_Game"
				};
			};
			class worldScriptModule
			{
				value = "";
				files[] =
				{
					"SDVehicleMenuClient/scripts/4_World"
				};
			};
			class missionScriptModule
			{
				value = "";
				files[] =
				{
					"SDVehicleMenuClient/scripts/5_Mission"
				};
			};
		};
	};
};

class CfgPatches
{
	class SDVehicleMenuClient_Scripts
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data"
		};
	};
};

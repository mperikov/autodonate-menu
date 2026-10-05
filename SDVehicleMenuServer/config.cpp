class CfgMods
{
	class SDVehicleMenuServer
	{
		dir = "SDVehicleMenuServer";
		hideName = 0;
		hidePicture = 0;
		name = "SDVehicle Menu Server";
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
				files[] = {"SDVehicleMenuServer/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"SDVehicleMenuServer/scripts/4_World"};
			};
		};
	};
};

class CfgPatches
{
	class SDVehicleMenuServer_Scripts
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] =
		{
			"DZ_Data",
			"SDVehicleMenuClient_Scripts"
		};
	};
};
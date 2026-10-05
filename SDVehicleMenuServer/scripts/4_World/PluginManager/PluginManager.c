modded class PluginManager
{
	override void Init()
	{
		super.Init();
		RegisterPlugin("PluginSDLoadoutDataManager", false, true);
		RegisterPlugin("PluginSDVehicleSpawnZoneManager", false, true);
		RegisterPlugin("PluginSDVehicleSpawnValidator", false, true);
		RegisterPlugin("PluginSDVehicleSpawnSessionTracker", false, true);
		RegisterPlugin("PluginSDVehicleRPCServerHandler", false, true);
		RegisterPlugin("PluginSDVehicleSpawner", false, true);
	}
}
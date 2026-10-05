modded class PluginManager
{
	override void Init()
	{
		super.Init();
		RegisterPlugin("PluginSDVehicleMenuHandler", true, false);
		RegisterPlugin("PluginSDVehicleRPCClientHandler", true, false);
		RegisterPlugin("PluginSDVehicleSpawnValidator", true, true);
	}
}

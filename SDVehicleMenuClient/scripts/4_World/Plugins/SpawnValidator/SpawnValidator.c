class PluginSDVehicleSpawnValidator : PluginBase
{
	eSDVehicleResultCode CanSpawn(PlayerBase player)
	{
		if (!player)
			return eSDVehicleResultCode.INTERNAL_ERROR;

		if (!player.IsAlive())
			return eSDVehicleResultCode.PLAYER_DEAD;

		if (player.IsUnconscious())
			return eSDVehicleResultCode.UNCONSCIOUS;

		if (player.IsInVehicle())
			return eSDVehicleResultCode.INCAR;

		if (player.IsInWater())
			return eSDVehicleResultCode.INWATER;

		eSDVehicleResultCode zoneCode = CheckSpawnZoneClient(player);
		if (zoneCode != eSDVehicleResultCode.SUCCESS)
			return zoneCode;

		vector spawnPos = player.GetPosition() + (player.GetDirection() * 5);
		float surfaceHeight = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);
		vector size = "3 5 9";

		if (GetGame().IsBoxColliding(spawnPos, vector.Zero, size, null, null) || Math.AbsFloat(surfaceHeight - spawnPos[1]) >= 1)
			return eSDVehicleResultCode.NOT_ENOUGH_SPACE;

		return eSDVehicleResultCode.SUCCESS;
	}

	protected eSDVehicleResultCode CheckSpawnZoneClient(PlayerBase player)
	{
		PluginSDVehicleMenuHandler menuHandler = GetSDVehicleMenuHandler();
		if (!menuHandler)
			return eSDVehicleResultCode.SUCCESS;

		SDVehicleListStorage storage = menuHandler.GetStorage();
		if (!storage)
			return eSDVehicleResultCode.SUCCESS;

		if (!storage.IsCarsSpawnEnabled())
			return eSDVehicleResultCode.CARS_SPAWN_DISABLED;

		if (!storage.HasSpawnZones())
			return eSDVehicleResultCode.SPAWN_ZONES_NOT_CONFIGURED;

		if (!SDVehicleSpawnZoneHelper.IsPositionInZones(player.GetPosition(), storage.m_SpawnZoneRadius, storage.m_SpawnZonePositions))
			return eSDVehicleResultCode.OUT_OF_SPAWN_ZONE;

		return eSDVehicleResultCode.SUCCESS;
	}
}

PluginSDVehicleSpawnValidator GetSDVehicleSpawnValidator()
{
	return PluginSDVehicleSpawnValidator.Cast(GetPlugin(PluginSDVehicleSpawnValidator));
}

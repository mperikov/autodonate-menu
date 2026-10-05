modded class PluginSDVehicleSpawnValidator
{
	override eSDVehicleResultCode CanSpawn(PlayerBase player)
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

		PluginSDVehicleSpawnZoneManager zoneManager = GetSDVehicleSpawnZoneManager();
		if (zoneManager)
		{
			if (!zoneManager.IsCarsSpawnEnabled())
				return eSDVehicleResultCode.CARS_SPAWN_DISABLED;

			if (!zoneManager.HasZones())
				return eSDVehicleResultCode.SPAWN_ZONES_NOT_CONFIGURED;

			if (!zoneManager.IsPlayerInSpawnZone(player))
				return eSDVehicleResultCode.OUT_OF_SPAWN_ZONE;
		}

		vector spawnPos = player.GetPosition() + (player.GetDirection() * 5);
		float surfaceHeight = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);
		vector size = "3 5 9";

		if (GetGame().IsBoxColliding(spawnPos, vector.Zero, size, null, null) || Math.AbsFloat(surfaceHeight - spawnPos[1]) >= 1)
			return eSDVehicleResultCode.NOT_ENOUGH_SPACE;

		return eSDVehicleResultCode.SUCCESS;
	}
}

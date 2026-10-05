class PluginSDVehicleSpawner : PluginBase
{
	void PluginSDVehicleSpawner()
	{
		SDVehicleSlotRegistry.EnsureInitialized();
	}

	eSDVehicleResultCode SpawnVehicle(PlayerBase player, string className, out EntityAI spawnedVehicle)
	{
		spawnedVehicle = null;

		if (!player || className == string.Empty)
			return eSDVehicleResultCode.EMPTY_REQUEST;

		SDVehicleListStorage storage = GetSDLoadoutDataManager().GetPlayerVehicleStorage(player);
		if (!storage || !storage.Contains(className))
			return eSDVehicleResultCode.VEHICLE_404;

		vector spawnPos = player.GetPosition() + (player.GetDirection() * 5);
		spawnPos[1] = GetGame().SurfaceY(spawnPos[0], spawnPos[2]);

		int spawnFlags = ECE_SETUP | ECE_KEEPHEIGHT | ECE_PLACE_ON_SURFACE | ECE_CREATEPHYSICS;
		EntityAI vehicle = EntityAI.Cast(GetGame().CreateObjectEx(className, spawnPos, spawnFlags));

		if (!vehicle)
			return eSDVehicleResultCode.INVALID_CLASS;

		CarScript car = CarScript.Cast(vehicle);
		BoatScript boat = BoatScript.Cast(vehicle);

		if (!car && !boat)
		{
			GetGame().ObjectDelete(vehicle);
			return eSDVehicleResultCode.INVALID_CLASS;
		}

		vehicle.OnDebugSpawn();

		if (SDVehicleSlotRegistry.HasMissingAttachments(vehicle))
			SDVehicleSlotRegistry.EquipMissingAttachments(vehicle);

		SDVehicleSlotRegistry.RemoveAccessoryAttachments(vehicle);
		SDVehicleSlotRegistry.ClearVehicleStorage(vehicle);

		if (car)
		{
			car.Fill(CarFluid.FUEL, 1000);
			car.Fill(CarFluid.OIL, 1000);
			car.Fill(CarFluid.BRAKE, 1000);
			car.Fill(CarFluid.COOLANT, 1000);
			car.Synchronize();
		}

		if (boat)
		{
			boat.Fill(BoatFluid.FUEL, 1000);
			boat.Synchronize();
		}

		vehicle.SetSynchDirty();
		spawnedVehicle = vehicle;
		return eSDVehicleResultCode.SUCCESS;
	}
}

PluginSDVehicleSpawner GetSDVehicleSpawner()
{
	return PluginSDVehicleSpawner.Cast(GetPlugin(PluginSDVehicleSpawner));
}

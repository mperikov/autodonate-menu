class PluginSDVehicleRPCServerHandler : PluginBase
{
	void PluginSDVehicleRPCServerHandler()
	{
		DayZGame.Event_OnRPC.Insert(OnRPC);
	}

	void ~PluginSDVehicleRPCServerHandler()
	{
		DayZGame.Event_OnRPC.Remove(OnRPC);
	}

	void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		PlayerBase player = PlayerBase.Cast(target);
		if (!player)
			return;

		switch (rpc_type)
		{
			case eSDVehicleRPC.GRAB_VEHICLES:
				GrabVehicles(player);
			break;
			case eSDVehicleRPC.REQUEST_SPAWN:
				RequestSpawnHandle(player, ctx);
			break;
		}
	}

	private void GrabVehicles(PlayerBase player)
	{
		SDVehicleListStorage storage = GetSDLoadoutDataManager().GetPlayerVehicleStorage(player);
		GetSDVehicleSpawnSessionTracker().AnnotateStorage(player, storage);
		PluginSDVehicleSpawnZoneManager zoneManager = GetSDVehicleSpawnZoneManager();
		if (zoneManager)
		{
			zoneManager.ReloadConfig();
			zoneManager.ApplyToStorage(storage);
		}
		GetGame().RPCSingleParam(null, eSDVehicleRPC.GRAB_VEHICLES, new Param1<SDVehicleListStorage>(storage), true, player.GetIdentity());
	}

	private void RequestSpawnHandle(PlayerBase player, ParamsReadContext ctx)
	{
		Param1<int> data;
		if (!ctx.Read(data))
			return;

		int slotIndex = data.param1;

		SDVehicleListStorage storage = GetSDLoadoutDataManager().GetPlayerVehicleStorage(player);
		if (!storage || !storage.HasVehicles())
		{
			Response(player.GetIdentity(), eSDVehicleResultCode.EMPTY_STORAGE);
			return;
		}

		if (!storage.IsValidSlot(slotIndex))
		{
			Response(player.GetIdentity(), eSDVehicleResultCode.VEHICLE_404);
			return;
		}

		if (GetSDVehicleSpawnSessionTracker().IsSlotConsumedThisRestart(player, storage, slotIndex))
		{
			Response(player.GetIdentity(), eSDVehicleResultCode.ALREADY_SPAWNED_THIS_RESTART);
			return;
		}

		string className = storage.GetVehicleAt(slotIndex);
		if (className == string.Empty)
		{
			Response(player.GetIdentity(), eSDVehicleResultCode.EMPTY_REQUEST);
			return;
		}

		PluginSDVehicleSpawnZoneManager zoneManager = GetSDVehicleSpawnZoneManager();
		if (zoneManager)
			zoneManager.ReloadConfig();

		eSDVehicleResultCode spawnCheck = GetSDVehicleSpawnValidator().CanSpawn(player);
		if (spawnCheck != eSDVehicleResultCode.SUCCESS)
		{
			Response(player.GetIdentity(), spawnCheck);
			return;
		}

		EntityAI spawnedVehicle;
		eSDVehicleResultCode spawnResult = GetSDVehicleSpawner().SpawnVehicle(player, className, spawnedVehicle);
		if (spawnResult == eSDVehicleResultCode.SUCCESS)
			GetSDVehicleSpawnSessionTracker().RegisterSuccessfulSpawn(player, slotIndex, spawnedVehicle);

		Response(player.GetIdentity(), spawnResult);
	}

	private void Response(PlayerIdentity identity, eSDVehicleResultCode code)
	{
		GetGame().RPCSingleParam(null, eSDVehicleRPC.RESPONSE, new Param1<eSDVehicleResultCode>(code), true, identity);
	}
}
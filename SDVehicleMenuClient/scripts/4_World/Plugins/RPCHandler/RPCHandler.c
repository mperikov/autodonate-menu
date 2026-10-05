class PluginSDVehicleRPCClientHandler : PluginBase
{
	void PluginSDVehicleRPCClientHandler()
	{
		DayZGame.Event_OnRPC.Insert(OnRPC);
	}

	void ~PluginSDVehicleRPCClientHandler()
	{
		DayZGame.Event_OnRPC.Remove(OnRPC);
	}

	private void OnRPC(PlayerIdentity sender, Object target, int rpc_type, ParamsReadContext ctx)
	{
		switch (rpc_type)
		{
			case eSDVehicleRPC.GRAB_VEHICLES:
				GrabVehiclesHandle(ctx);
			break;
			case eSDVehicleRPC.RESPONSE:
				ResponseHandle(ctx);
			break;
		}
	}

	private void GrabVehiclesHandle(ParamsReadContext ctx)
	{
		Param1<SDVehicleListStorage> data;

		if (!ctx.Read(data))
			return;

		GetSDVehicleMenuHandler().OnReceiveStorage(data.param1);
	}

	private void ResponseHandle(ParamsReadContext ctx)
	{
		Param1<eSDVehicleResultCode> data;

		if (!ctx.Read(data))
			return;

		GetSDVehicleMenuHandler().OnReceiveResponse(data.param1);
	}
}

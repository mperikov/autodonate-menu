class SDLoadoutRpcHandler
{
	void SDLoadoutRpcHandler()
	{
		if (!GetGame().IsServer())
			return;

		GetRPCManager().AddRPC(SDLoadoutRPC.MOD_NAME, SDLoadoutRPC.REQUEST_CATALOG, this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(SDLoadoutRPC.MOD_NAME, SDLoadoutRPC.SUBMIT_LOADOUT, this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(SDLoadoutRPC.MOD_NAME, SDLoadoutRPC.APPLY_RANDOM, this, SingleplayerExecutionType.Server);
	}

	void ApplyRandom(int rpc_type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (!GetGame().IsServer() || !sender)
			return;

		if (!SDLoadoutRpcGuard.TryApplyRandom(sender))
			return;

		SDLoadoutMissionHandler.OnApplyRandomRequested(sender);
	}

	void RequestCatalog(int rpc_type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (!GetGame().IsServer() || !sender)
			return;

		if (!SDLoadoutRpcGuard.TryRequestCatalog(sender))
			return;

		SDLoadoutMissionHandler.OnCatalogRequested(sender);
	}

	void SubmitLoadout(int rpc_type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (!GetGame().IsServer() || !sender)
			return;

		if (!SDLoadoutRpcGuard.TrySubmitLoadout(sender))
			return;

		Param1<ref SDLoadoutSelection> data;
		if (!ctx.Read(data) || !data.param1)
			return;

		SDLoadoutMissionHandler.OnLoadoutSubmitted(sender, data.param1);
	}
}

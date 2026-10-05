modded class MissionServer
{
	protected ref SDLoadoutRpcHandler m_SDLoadoutRpcHandler;

	override void OnInit()
	{
		super.OnInit();
		SDLoadoutConfigManager.Init();
		m_SDLoadoutRpcHandler = new SDLoadoutRpcHandler();
	}

	override PlayerBase OnClientNewEvent(PlayerIdentity identity, vector pos, ParamsReadContext ctx)
	{
		PlayerBase player = super.OnClientNewEvent(identity, pos, ctx);

		if (player)
			SDLoadoutMissionHandler.OnPlayerSpawned(identity, player);

		return player;
	}
}

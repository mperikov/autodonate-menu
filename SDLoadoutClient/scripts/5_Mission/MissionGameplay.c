modded class MissionGameplay
{
	protected ref SDLoadoutMenu m_SDLoadoutMenu;

	ScriptCallQueue qew = GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM);

	void MissionGameplay()
	{
		if (GetGame().IsClient())
		{
			SDLoadoutSpawnFlow.Shutdown();
			GetRPCManager().AddRPC(SDLoadoutRPC.MOD_NAME, SDLoadoutRPC.OPEN_MENU, this, SingleplayerExecutionType.Client);
		}
	}

	override void OnMissionFinish()
	{
		SDLoadoutSpawnFlow.Shutdown();
		super.OnMissionFinish();
	}

	override void StartSpawnSelectionFromSev(int rpc_type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (!GetGame().IsClient())
			return;

		if (SDLoadoutState.IsWaitingForPostLoadoutSpawnNotify())
		{
			SDLoadoutState.SetWaitingForPostLoadoutSpawnNotify(false);
			SDLoadoutState.SetSpawnSelectPending(false);
			super.StartSpawnSelectionFromSev(rpc_type, ctx, sender, target);
			return;
		}

		SDLoadoutState.BeginSpawnCycle();
		SDLoadoutState.SetSpawnSelectPending(true);
		SDLoadoutState.SetMenuOpen(false);
		SDLoadoutSpawnFlow.ClearCachedCatalog();
		SDLoadoutSpawnFlow.RequestCatalogFromServer();

		SDLoadoutMenu existingMenu = SDLoadoutMenu.GetInstance();
		if (existingMenu)
		{
			SDLoadoutState.SetSuppressCancelOnHide(true);
			existingMenu.Close();
			SDLoadoutState.SetSuppressCancelOnHide(false);
		}

		SD_StopLoadoutRetry();
	}

	void SD_StopLoadoutRetry()
	{
		qew.Remove(this.initLoadoutMenu);
	}

	void SD_StopSpawnSelectRetry()
	{
		qew.Remove(this.initSSMenu);
	}

	void initLoadoutMenu()
	{
		if (GetGame().GetUIManager().GetMenu() != NULL)
			return;

		if (g_Game.GetMissionState() != DayZGame.MISSION_STATE_GAME || !GetHud())
			return;

		if (!SDLoadoutState.IsAwaitingPostSpawnLoadout())
		{
			SD_StopLoadoutRetry();
			return;
		}

		ref SDLoadoutCatalogData catalog = SDLoadoutSpawnFlow.GetCachedCatalog();
		if (!catalog || !catalog.IsValid())
			return;

		m_SDLoadoutMenu = new SDLoadoutMenu();
		m_SDLoadoutMenu.SetCatalog(catalog);
		GetGame().GetUIManager().ShowScriptedMenu(m_SDLoadoutMenu, NULL);

		SD_StopLoadoutRetry();
	}

	void SD_OpenSpawnSelectAfterLoadout()
	{
		if (!GetGame().IsClient())
			return;

		SD_StopSpawnSelectRetry();
		SDLoadoutState.SetAwaitingPostSpawnLoadout(false);
		SDLoadoutState.SetSpawnSelectPending(false);

		if (g_Game.GetMissionState() == DayZGame.MISSION_STATE_GAME && GetHud())
			qew.CallLater(this.initSSMenu, 400, true);
	}

	void OpenMenu(int rpc_type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		if (!GetGame().IsClient())
			return;

		SDLoadoutState.BeginSpawnCycle();
		SDLoadoutState.SetMenuOpen(false);

		Param1<ref SDLoadoutCatalogData> data;
		if (!ctx.Read(data) || !data.param1)
		{
			Print("[SDLoadout] OpenMenu RPC: failed to read catalog.");
			SDLoadoutSpawnFlow.OnCatalogReceiveFailed();
			return;
		}

		if (!data.param1.IsValid())
		{
			Print("[SDLoadout] Catalog has no gear slots — check server $profile:SDLoadout/loadouts.json");
			SDLoadoutSpawnFlow.OnCatalogReceiveFailed();
			return;
		}

		SDLoadoutSpawnFlow.SetCachedCatalog(data.param1);

		SD_StopLoadoutRetry();
		qew.CallLater(this.initLoadoutMenu, 1750, true);
	}
}

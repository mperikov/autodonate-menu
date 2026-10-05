class SDLoadoutSpawnFlow
{
	protected static ref SDLoadoutCatalogData s_CachedCatalog;

	static ref SDLoadoutCatalogData GetCachedCatalog()
	{
		return s_CachedCatalog;
	}

	static void SetCachedCatalog(SDLoadoutCatalogData catalog)
	{
		s_CachedCatalog = catalog;
	}

	static void ClearCachedCatalog()
	{
		s_CachedCatalog = null;
	}

	static void RequestCatalogFromServer()
	{
		if (!GetGame().IsClient())
			return;

		GetRPCManager().SendRPC(SDLoadoutRPC.MOD_NAME, SDLoadoutRPC.REQUEST_CATALOG, null, true);
	}

	static void CancelPendingUi()
	{
		ScriptCallQueue guiQueue = GetGame().GetCallQueue(CALL_CATEGORY_GUI);
		guiQueue.Remove(OpenSpawnSelectAfterLoadout);

		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission)
		{
			mission.SD_StopLoadoutRetry();
			mission.SD_StopSpawnSelectRetry();
		}
	}

	static void Shutdown()
	{
		CancelPendingUi();
		s_CachedCatalog = null;
		SDLoadoutState.ResetAll();
		SDLoadoutPreviewHelper.DestroyPreview();

		SDLoadoutMenu menu = SDLoadoutMenu.GetInstance();
		if (menu)
			menu.Close();
	}

	static void OnCatalogReceiveFailed()
	{
		Print("[SDLoadout] Could not open loadout menu — loadout already applied on server, opening spawn select.");
		SkipLoadoutMenuAndOpenSpawn();
	}

	static void ConfirmSelection(SDLoadoutSelection selection)
	{
		if (!selection)
			return;

		CancelPendingUi();
		SDLoadoutState.SetSuppressCancelOnHide(true);
		SDLoadoutState.SetAwaitingPostSpawnLoadout(false);
		SDLoadoutState.SetMenuOpen(false);
		SDLoadoutState.CompleteSpawnCycleLoadout();
		SDLoadoutState.SetWaitingForPostLoadoutSpawnNotify(true);

		SDLoadoutMenu menu = SDLoadoutMenu.GetInstance();
		if (menu)
			menu.Close();

		GetRPCManager().SendRPC(SDLoadoutRPC.MOD_NAME, SDLoadoutRPC.SUBMIT_LOADOUT, new Param1<ref SDLoadoutSelection>(selection), true);
	}

	static void CancelWithoutSelection()
	{
		if (!SDLoadoutState.IsAwaitingPostSpawnLoadout())
			return;

		SDLoadoutState.SetAwaitingPostSpawnLoadout(false);
		SDLoadoutState.SetMenuOpen(false);
		SDLoadoutState.CompleteSpawnCycleLoadout();
		OpenSpawnSelectAfterLoadout();
	}

	static void SkipLoadoutMenuAndOpenSpawn()
	{
		if (!SDLoadoutState.IsAwaitingPostSpawnLoadout())
			return;

		SDLoadoutState.SetSuppressCancelOnHide(true);
		SDLoadoutState.SetAwaitingPostSpawnLoadout(false);
		SDLoadoutState.SetMenuOpen(false);
		SDLoadoutState.CompleteSpawnCycleLoadout();

		SDLoadoutMenu menu = SDLoadoutMenu.GetInstance();
		if (menu)
			menu.Close();

		SDLoadoutState.SetSuppressCancelOnHide(false);

		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission)
			mission.SD_StopLoadoutRetry();

		OpenSpawnSelectAfterLoadout();
	}

	protected static void OpenSpawnSelectAfterLoadout()
	{
		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission)
			mission.SD_OpenSpawnSelectAfterLoadout();
	}
}

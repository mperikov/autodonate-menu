class SDLoadoutMissionHandler
{
	private static ref map<string, ref SDLoadoutSelection> s_PendingSelection = new map<string, ref SDLoadoutSelection>;

	static void OnCatalogRequested(PlayerIdentity identity)
	{
		SendCatalogToClient(identity);
	}

	static void SendCatalogToClient(PlayerIdentity identity)
	{
		if (!GetGame().IsServer() || !identity)
			return;

		ref SDLoadoutCatalogData catalog = SDLoadoutCatalogBuilder.BuildForIdentity(identity);

		if (!catalog || !catalog.IsValid())
			Print("[SDLoadout] Catalog empty for " + identity.GetName() + " — add loadouts.json to server profile.");

		GetRPCManager().SendRPC(SDLoadoutRPC.MOD_NAME, SDLoadoutRPC.OPEN_MENU, new Param1<ref SDLoadoutCatalogData>(catalog), true, identity);
	}

	static void OnLoadoutSubmitted(PlayerIdentity identity, SDLoadoutSelection selection)
	{
		if (!GetGame().IsServer() || !identity || !selection)
			return;

		SDLoadoutSelection validated = SDLoadoutSelectionValidator.Validate(identity, selection);
		if (!validated)
		{
			Print("[SDLoadout] Rejected loadout submission from " + identity.GetName() + " — applying random.");
			OnApplyRandomRequested(identity);
			return;
		}

		ApplyToPlayer(identity, validated);
	}

	static void OnApplyRandomRequested(PlayerIdentity identity)
	{
		if (!GetGame().IsServer() || !identity)
			return;

		PlayerBase player = PlayerBase.Cast(identity.GetPlayer());
		if (!player)
			return;

		SDLoadoutSelection selection = SDLoadoutApplier.BuildRandomSelection();
		if (selection)
		{
			SDLoadoutApplier.ApplySelection(player, selection);
			SaveAppliedSelection(identity.GetPlainId(), selection);
		}

		SDLoadoutRpcGuard.CompleteSpawnFlow(identity.GetPlainId());
		NotifySpawnSelectReady(identity);
	}

	static void OnPlayerSpawned(PlayerIdentity identity, PlayerBase player)
	{
		if (!GetGame().IsServer() || !identity || !player)
			return;

		string steamId = identity.GetPlainId();
		SDLoadoutRpcGuard.ActivateSpawnFlow(steamId);

		SDLoadoutSelection pendingSelection = GetPendingSelection(steamId);

		if (pendingSelection)
		{
			SDLoadoutSelection validatedPending = SDLoadoutSelectionValidator.Validate(identity, pendingSelection);
			if (validatedPending)
				SDLoadoutApplier.ApplySelection(player, validatedPending);
			else
				ApplyLastOrRandomLoadout(player);

			if (validatedPending)
				SaveAppliedSelection(steamId, validatedPending);

			s_PendingSelection.Remove(steamId);
			SDLoadoutRpcGuard.CompleteSpawnFlow(steamId);
			NotifySpawnSelectReady(identity);
			return;
		}

		ApplyLastOrRandomLoadout(player);
		NotifySpawnSelectReady(identity);
	}

	static void ApplyToPlayer(PlayerIdentity identity, SDLoadoutSelection selection)
	{
		if (!identity || !selection)
			return;

		string steamId = identity.GetPlainId();
		s_PendingSelection.Set(steamId, selection);

		PlayerBase player = PlayerBase.Cast(identity.GetPlayer());
		if (player)
		{
			SDLoadoutApplier.ApplySelection(player, selection);
			SaveAppliedSelection(steamId, selection);
			s_PendingSelection.Remove(steamId);
			SDLoadoutRpcGuard.CompleteSpawnFlow(steamId);
			NotifySpawnSelectReady(identity);
		}
	}

	static void NotifySpawnSelectReady(PlayerIdentity identity)
	{
		if (!GetGame().IsServer() || !identity)
			return;

		GetRPCManager().SendRPC("RPCSpawnSelect", "StartSpawnSelectionFromSev", new Param1<vector>("0 0 0"), true, identity);
	}

	static ref SDLoadoutSelection GetPendingSelection(string steamId)
	{
		if (!s_PendingSelection || steamId == string.Empty || !s_PendingSelection.Contains(steamId))
			return null;

		return s_PendingSelection.Get(steamId);
	}

	protected static void SaveAppliedSelection(string steamId, SDLoadoutSelection selection)
	{
		SDLoadoutPlayerStorage.SaveLastSelection(steamId, selection);
	}

	protected static void ApplyLastOrRandomLoadout(PlayerBase player)
	{
		if (!player)
			return;

		PlayerIdentity identity = player.GetIdentity();
		if (!identity)
			return;

		string steamId = identity.GetPlainId();
		SDLoadoutSelection savedSelection = SDLoadoutPlayerStorage.LoadLastSelection(steamId);

		if (savedSelection)
		{
			SDLoadoutSelection validatedSaved = SDLoadoutSelectionValidator.Validate(identity, savedSelection);
			if (validatedSaved)
			{
				SDLoadoutApplier.ApplySelection(player, validatedSaved);
				return;
			}

			Print("[SDLoadout] Saved loadout for " + identity.GetName() + " is no longer valid — applying random.");
		}

		SDLoadoutSelection randomSelection = SDLoadoutApplier.BuildRandomSelection();
		if (!randomSelection)
			return;

		SDLoadoutApplier.ApplySelection(player, randomSelection);
		SaveAppliedSelection(steamId, randomSelection);
	}
}

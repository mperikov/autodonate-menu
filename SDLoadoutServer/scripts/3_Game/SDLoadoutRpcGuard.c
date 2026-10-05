class SDLoadoutRpcGuard
{
	private static const float REQUEST_CATALOG_COOLDOWN_SEC = 2.5;
	private static const float SUBMIT_LOADOUT_COOLDOWN_SEC = 1.5;
	private static const float APPLY_RANDOM_COOLDOWN_SEC = 3.0;

	private static ref map<string, bool> s_SpawnFlowActive;
	private static ref map<string, float> s_LastRequestCatalogAt;
	private static ref map<string, float> s_LastSubmitLoadoutAt;
	private static ref map<string, float> s_LastApplyRandomAt;

	static void ActivateSpawnFlow(string steamId)
	{
		if (steamId == string.Empty)
			return;

		EnsureMaps();
		s_SpawnFlowActive.Set(steamId, true);
	}

	static void CompleteSpawnFlow(string steamId)
	{
		if (steamId == string.Empty || !s_SpawnFlowActive)
			return;

		if (s_SpawnFlowActive.Contains(steamId))
			s_SpawnFlowActive.Remove(steamId);
	}

	static void ClearPlayer(string steamId)
	{
		if (steamId == string.Empty)
			return;

		EnsureMaps();

		if (s_SpawnFlowActive.Contains(steamId))
			s_SpawnFlowActive.Remove(steamId);

		if (s_LastRequestCatalogAt.Contains(steamId))
			s_LastRequestCatalogAt.Remove(steamId);

		if (s_LastSubmitLoadoutAt.Contains(steamId))
			s_LastSubmitLoadoutAt.Remove(steamId);

		if (s_LastApplyRandomAt.Contains(steamId))
			s_LastApplyRandomAt.Remove(steamId);
	}

	static bool TryRequestCatalog(PlayerIdentity identity)
	{
		return TryRpc(identity, "RequestCatalog", s_LastRequestCatalogAt, REQUEST_CATALOG_COOLDOWN_SEC);
	}

	static bool TrySubmitLoadout(PlayerIdentity identity)
	{
		return TryRpc(identity, "SubmitLoadout", s_LastSubmitLoadoutAt, SUBMIT_LOADOUT_COOLDOWN_SEC);
	}

	static bool TryApplyRandom(PlayerIdentity identity)
	{
		return TryRpc(identity, "ApplyRandom", s_LastApplyRandomAt, APPLY_RANDOM_COOLDOWN_SEC);
	}

	protected static bool TryRpc(PlayerIdentity identity, string rpcName, map<string, float> lastActionAt, float cooldownSec)
	{
		if (!GetGame().IsServer() || !identity)
			return false;

		string steamId = identity.GetPlainId();
		if (steamId == string.Empty)
			return false;

		EnsureMaps();

		if (!IsSpawnFlowActive(steamId))
		{
			LogBlocked(identity, rpcName, "outside spawn loadout flow");
			return false;
		}

		if (!IsCooldownReady(steamId, lastActionAt, cooldownSec))
			return false;

		MarkAction(steamId, lastActionAt);
		return true;
	}

	protected static bool IsSpawnFlowActive(string steamId)
	{
		return s_SpawnFlowActive && s_SpawnFlowActive.Contains(steamId) && s_SpawnFlowActive.Get(steamId);
	}

	protected static bool IsCooldownReady(string steamId, map<string, float> lastActionAt, float cooldownSec)
	{
		if (!lastActionAt || !lastActionAt.Contains(steamId))
			return true;

		float elapsed = GetGame().GetTickTime() - lastActionAt.Get(steamId);
		return elapsed >= cooldownSec;
	}

	protected static void MarkAction(string steamId, map<string, float> lastActionAt)
	{
		lastActionAt.Set(steamId, GetGame().GetTickTime());
	}

	protected static void LogBlocked(PlayerIdentity identity, string rpcName, string reason)
	{
		Print("[SDLoadout] Blocked RPC " + rpcName + " from " + identity.GetName() + " (" + reason + ").");
	}

	protected static void EnsureMaps()
	{
		if (!s_SpawnFlowActive)
			s_SpawnFlowActive = new map<string, bool>;

		if (!s_LastRequestCatalogAt)
			s_LastRequestCatalogAt = new map<string, float>;

		if (!s_LastSubmitLoadoutAt)
			s_LastSubmitLoadoutAt = new map<string, float>;

		if (!s_LastApplyRandomAt)
			s_LastApplyRandomAt = new map<string, float>;
	}
}

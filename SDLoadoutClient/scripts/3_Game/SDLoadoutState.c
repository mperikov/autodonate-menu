class SDLoadoutState
{
	protected static bool s_AwaitingPostSpawnLoadout;
	protected static bool s_MenuOpen;
	protected static bool s_SpawnSelectPending;
	protected static bool s_SuppressCancelOnHide;
	protected static bool s_SpawnCycleLoadoutComplete;
	protected static bool s_WaitingForPostLoadoutSpawnNotify;

	static void SetAwaitingPostSpawnLoadout(bool value)
	{
		s_AwaitingPostSpawnLoadout = value;
	}

	static bool IsAwaitingPostSpawnLoadout()
	{
		return s_AwaitingPostSpawnLoadout;
	}

	static bool IsSuppressCancelOnHide()
	{
		return s_SuppressCancelOnHide;
	}

	static void SetMenuOpen(bool value)
	{
		s_MenuOpen = value;
	}

	static bool IsMenuOpen()
	{
		return s_MenuOpen;
	}

	static void SetSpawnSelectPending(bool value)
	{
		s_SpawnSelectPending = value;
	}

	static bool IsSpawnSelectPending()
	{
		return s_SpawnSelectPending;
	}

	static void SetSuppressCancelOnHide(bool value)
	{
		s_SuppressCancelOnHide = value;
	}

	static void BeginSpawnCycle()
	{
		s_SpawnCycleLoadoutComplete = false;
		s_AwaitingPostSpawnLoadout = true;
	}

	static void CompleteSpawnCycleLoadout()
	{
		s_SpawnCycleLoadoutComplete = true;
	}

	static bool IsSpawnCycleLoadoutComplete()
	{
		return s_SpawnCycleLoadoutComplete;
	}

	static void SetWaitingForPostLoadoutSpawnNotify(bool value)
	{
		s_WaitingForPostLoadoutSpawnNotify = value;
	}

	static bool IsWaitingForPostLoadoutSpawnNotify()
	{
		return s_WaitingForPostLoadoutSpawnNotify;
	}

	static bool ShouldDeferSpawnSelect()
	{
		return !s_SpawnCycleLoadoutComplete || s_AwaitingPostSpawnLoadout || s_MenuOpen;
	}

	static void ResetAll()
	{
		s_AwaitingPostSpawnLoadout = false;
		s_MenuOpen = false;
		s_SpawnSelectPending = false;
		s_SuppressCancelOnHide = false;
		s_SpawnCycleLoadoutComplete = false;
		s_WaitingForPostLoadoutSpawnNotify = false;
	}
}

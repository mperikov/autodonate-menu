class SDLoadoutBasicMapCompat
{
	protected static int s_HideRefCount;
	protected static bool s_MarkersWereOn;

	static void OnPostSpawnMenuOpen()
	{
		if (!GetGame().IsClient())
			return;

		if (s_HideRefCount == 0)
		{
			s_MarkersWereOn = BasicMap().ShowMarkersOnHUD();
			if (s_MarkersWereOn)
				BasicMap().SetMarkersOnHUD(false);
		}

		s_HideRefCount++;
	}

	static void OnPostSpawnMenuClose()
	{
		if (!GetGame().IsClient())
			return;

		if (s_HideRefCount <= 0)
			return;

		s_HideRefCount--;

		if (s_HideRefCount == 0 && s_MarkersWereOn)
			BasicMap().SetMarkersOnHUD(true);
	}
}

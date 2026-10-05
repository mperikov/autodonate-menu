class PluginSDVehicleMenuHandler : PluginBase
{
	private ref SDVehicleListStorage m_Storage;
	private ref SDVehicleMenuUI m_Menu;
	private int m_LastOpenTimestamp;
	private const int OPEN_DELAY = 3000;

	bool CanOpenMenu()
	{
		return GetGame().GetTime() - m_LastOpenTimestamp > OPEN_DELAY;
	}

	void OnMenuOpened()
	{
		m_LastOpenTimestamp = GetGame().GetTime();
	}

	SDVehicleMenuUI GetMenu()
	{
		return m_Menu;
	}

	void SetStorage(SDVehicleListStorage storage)
	{
		m_Storage = storage;
	}

	SDVehicleListStorage GetStorage()
	{
		return m_Storage;
	}

	void OnReceiveStorage(SDVehicleListStorage storage)
	{
		SetStorage(storage);

		if (m_Menu)
		{
			m_Menu.UnblockMenu();
			m_Menu.FillContent();
		}
	}

	void OnReceiveResponse(eSDVehicleResultCode code)
	{
		if (m_Menu)
			m_Menu.ShowResponse(code);
	}

	bool OpenUI()
	{
		if (m_Menu || GetGame().GetUIManager().GetMenu())
			return false;

		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.IsUnconscious())
		{
			player.MessageStatus(SDVehicleStatic.GetMessageFromResponse(eSDVehicleResultCode.UNCONSCIOUS));
			return false;
		}

		if (!CanOpenMenu())
		{
			if (player)
				player.MessageStatus(SDVehicleLocalization.T(SDVehicleLocalization.OPEN_COOLDOWN));
			return false;
		}

		OnMenuOpened();
		m_Menu = new SDVehicleMenuUI();
		GetGame().GetUIManager().ShowScriptedMenu(m_Menu, NULL);
		return true;
	}

	void OnMenuClosed()
	{
		m_Menu = null;
	}
}

PluginSDVehicleMenuHandler GetSDVehicleMenuHandler()
{
	return PluginSDVehicleMenuHandler.Cast(GetPlugin(PluginSDVehicleMenuHandler));
}
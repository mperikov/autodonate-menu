class SDVehiclePreviewItem
{
	private ButtonWidget m_Root;
	private TextWidget m_NameLabel;
	private int m_SlotIndex;
	private string m_ClassName;
	private bool m_SpawnedThisRestart;

	void SDVehiclePreviewItem(int slotIndex, string className, bool spawnedThisRestart)
	{
		m_SlotIndex = slotIndex;
		m_ClassName = className;
		m_SpawnedThisRestart = spawnedThisRestart;

		m_Root = ButtonWidget.Cast(GetGame().GetWorkspace().CreateWidgets("SDVehicleMenuClient/assets/layouts/VehicleListItem.layout"));
		CastTo(m_NameLabel, m_Root.FindAnyWidget("VehicleName"));
		m_NameLabel.SetText(className);
		ApplyVisual(false);
	}

	void ~SDVehiclePreviewItem()
	{
		if (m_Root)
			m_Root.Unlink();
	}

	int GetSlotIndex()
	{
		return m_SlotIndex;
	}

	string GetClassName()
	{
		return m_ClassName;
	}

	Widget GetRoot()
	{
		return m_Root;
	}

	Widget GetNav()
	{
		return m_Root;
	}

	void SetSelected(bool selected)
	{
		ApplyVisual(selected);
	}

	private void ApplyVisual(bool selected)
	{
		if (m_SpawnedThisRestart)
		{
			m_NameLabel.SetColor(ARGB(255, 220, 55, 55));

			if (selected)
				m_Root.SetColor(ARGB(255, 55, 35, 35));
			else
				m_Root.SetColor(ARGB(255, 35, 25, 25));
		}
		else
		{
			m_NameLabel.SetColor(ARGB(255, 220, 220, 220));

			if (selected)
				m_Root.SetColor(ARGB(255, 60, 80, 50));
			else
				m_Root.SetColor(ARGB(255, 23, 28, 24));
		}
	}
}

class SDVehicleDetailPreview
{
	private ItemPreviewWidget m_PreviewWidget;
	private EntityAI m_PreviewEntity;

	void Link(Widget layoutRoot)
	{
		CastTo(m_PreviewWidget, layoutRoot.FindAnyWidget("VehiclePreview"));
	}

	void ~SDVehicleDetailPreview()
	{
		Clear();
	}

	void ShowVehicle(string className)
	{
		Clear();

		if (!m_PreviewWidget || className == string.Empty)
			return;

		m_PreviewEntity = EntityAI.Cast(GetGame().CreateObject(className, vector.Zero, true));
		if (!m_PreviewEntity)
			return;

		CarScript car;
		if (Class.CastTo(car, m_PreviewEntity))
			car.OnDebugSpawn();

		m_PreviewWidget.SetItem(m_PreviewEntity);
		m_PreviewWidget.SetView(m_PreviewEntity.GetViewIndex());
	}

	void Clear()
	{
		if (m_PreviewWidget)
			m_PreviewWidget.SetItem(null);

		if (m_PreviewEntity && GetGame())
			GetGame().ObjectDelete(m_PreviewEntity);

		m_PreviewEntity = null;
	}

	string GetDisplayName()
	{
		if (m_PreviewEntity)
			return m_PreviewEntity.GetDisplayName();

		return string.Empty;
	}

	bool HasPreview()
	{
		return m_PreviewEntity != null;
	}
}

class SDVehicleMenuUI : UIScriptedMenu
{
	private Widget m_Overlay;
	private TextWidget m_EmptyStorageLabel;
	private ScrollWidget m_ProductsScroll;
	private Widget m_ProductListContainer;
	private ButtonWidget m_BtnExecute;
	private TextWidget m_ProductTitle;
	private TextWidget m_SpawnErrorLabel;
	private RichTextWidget m_NotificationText;

	private PlayerBase m_Player;
	private int m_SelectedSlotIndex;
	private string m_SelectedClassName;
	private ref map<Widget, ref SDVehiclePreviewItem> m_PreviewItems;
	private ref SDVehicleDetailPreview m_DetailPreview;

	void SDVehicleMenuUI()
	{
		m_Player = PlayerBase.Cast(GetGame().GetPlayer());
		GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
	}

	void ~SDVehicleMenuUI()
	{
		if (m_DetailPreview)
			delete m_DetailPreview;

		if (GetGame() && GetGame().GetMission())
			GetGame().GetMission().PlayerControlEnable(true);

		if (GetSDVehicleMenuHandler())
			GetSDVehicleMenuHandler().OnMenuClosed();
	}

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("SDVehicleMenuClient/assets/layouts/SDVehicleMenu.layout");

		CastTo(m_Overlay, layoutRoot.FindAnyWidget("Overlay"));
		CastTo(m_EmptyStorageLabel, layoutRoot.FindAnyWidget("EmptyStorageLabel"));
		CastTo(m_ProductsScroll, layoutRoot.FindAnyWidget("ProductsScroll"));
		CastTo(m_ProductListContainer, layoutRoot.FindAnyWidget("ProductListContainer"));
		CastTo(m_BtnExecute, layoutRoot.FindAnyWidget("BtnExecute"));
		CastTo(m_ProductTitle, layoutRoot.FindAnyWidget("ProductTitle"));
		CastTo(m_SpawnErrorLabel, layoutRoot.FindAnyWidget("SpawnErrorLabel"));
		CastTo(m_NotificationText, layoutRoot.FindAnyWidget("NotificationText"));

		m_PreviewItems = new map<Widget, ref SDVehiclePreviewItem>();
		m_DetailPreview = new SDVehicleDetailPreview();
		m_DetailPreview.Link(layoutRoot);

		return layoutRoot;
	}

	override void OnShow()
	{
		super.OnShow();
		RequestStorage();
	}

	override void Update(float timeslice)
	{
		if (GetGame().GetInput().LocalPress("UAUIBack", false))
			Close();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);

		if (m_PreviewItems && m_PreviewItems.Contains(w))
		{
			SelectVehicle(m_PreviewItems.Get(w));
			return true;
		}

		if (w == m_BtnExecute)
		{
			TrySpawn();
			return true;
		}

		return false;
	}

	void FillContent()
	{
		ClearPreviewItems();
		m_SelectedSlotIndex = -1;
		m_SelectedClassName = string.Empty;
		ResetDetailPanel();

		SDVehicleListStorage storage = GetSDVehicleMenuHandler().GetStorage();
		if (!storage || !storage.HasVehicles())
		{
			ShowEmptyStorageLabel();
			return;
		}

		HideEmptyStorageLabel();

		for (int i = 0; i < storage.m_Vehicles.Count(); i++)
		{
			string className = storage.m_Vehicles[i];
			CreateVehiclePreview(i, className, storage.IsSlotConsumedThisRestart(i));
		}
	}

	void SelectVehicle(SDVehiclePreviewItem preview)
	{
		if (!preview)
			return;

		for (int i = 0; i < m_PreviewItems.Count(); i++)
		{
			SDVehiclePreviewItem item = m_PreviewItems.GetElement(i);
			if (item)
				item.SetSelected(false);
		}

		preview.SetSelected(true);
		m_SelectedSlotIndex = preview.GetSlotIndex();
		m_SelectedClassName = preview.GetClassName();

		if (m_DetailPreview)
			m_DetailPreview.ShowVehicle(m_SelectedClassName);

		string title = m_SelectedClassName;
		if (m_DetailPreview && m_DetailPreview.HasPreview())
			title = m_DetailPreview.GetDisplayName();

		m_ProductTitle.SetText(title);
		UpdateSpawnHint();
	}

	void TrySpawn()
	{
		if (m_SelectedSlotIndex < 0 || m_SelectedClassName == string.Empty)
		{
			ShowNotification(SDVehicleLocalization.T(SDVehicleLocalization.NOT_SELECTED));
			return;
		}

		if (IsSelectedSlotAlreadySpawnedThisRestart())
		{
			ShowSpawnError(SDVehicleStatic.GetMessageFromResponse(eSDVehicleResultCode.ALREADY_SPAWNED_THIS_RESTART));
			return;
		}

		eSDVehicleResultCode code = GetSDVehicleSpawnValidator().CanSpawn(m_Player);
		if (code != eSDVehicleResultCode.SUCCESS)
		{
			ShowSpawnError(SDVehicleStatic.GetMessageFromResponse(code));
			return;
		}

		HideSpawnError();
		BlockMenu();
		ShowNotification(SDVehicleLocalization.T(SDVehicleLocalization.WAIT_SERVER));
		GetGame().RPCSingleParam(GetGame().GetPlayer(), eSDVehicleRPC.REQUEST_SPAWN, new Param1<int>(m_SelectedSlotIndex), true);
	}

	void ShowResponse(eSDVehicleResultCode code)
	{
		UnblockMenu();
		ShowNotification(SDVehicleStatic.GetMessageFromResponse(code));

		if (code == eSDVehicleResultCode.SUCCESS)
			Close();
	}

	void BlockMenu()
	{
		if (m_Overlay)
			m_Overlay.Show(true);
	}

	void UnblockMenu()
	{
		if (m_Overlay)
			m_Overlay.Show(false);
	}

	void ShowNotification(string text)
	{
		if (m_NotificationText)
			m_NotificationText.SetText(text);
	}

	private void RequestStorage()
	{
		BlockMenu();
		GetGame().RPCSingleParam(GetGame().GetPlayer(), eSDVehicleRPC.GRAB_VEHICLES, null, true);
	}

	private void CreateVehiclePreview(int slotIndex, string className, bool spawnedThisRestart)
	{
		SDVehiclePreviewItem preview = new SDVehiclePreviewItem(slotIndex, className, spawnedThisRestart);
		m_PreviewItems.Insert(preview.GetNav(), preview);
		m_ProductListContainer.AddChild(preview.GetRoot());
	}

	private void ClearPreviewItems()
	{
		if (!m_PreviewItems)
			return;

		for (int i = 0; i < m_PreviewItems.Count(); i++)
		{
			SDVehiclePreviewItem item = m_PreviewItems.GetElement(i);
			if (item)
				delete item;
		}

		m_PreviewItems.Clear();
	}

	private void ShowEmptyStorageLabel()
	{
		m_EmptyStorageLabel.Show(true);
		m_ProductsScroll.Show(false);
	}

	private void HideEmptyStorageLabel()
	{
		m_EmptyStorageLabel.Show(false);
		m_ProductsScroll.Show(true);
	}

	private void ResetDetailPanel()
	{
		if (m_DetailPreview)
			m_DetailPreview.Clear();

		m_ProductTitle.SetText(SDVehicleLocalization.T(SDVehicleLocalization.SELECT_VEHICLE));
		HideSpawnError();
	}

	private void UpdateSpawnHint()
	{
		if (IsSelectedSlotAlreadySpawnedThisRestart())
		{
			ShowSpawnError(SDVehicleStatic.GetMessageFromResponse(eSDVehicleResultCode.ALREADY_SPAWNED_THIS_RESTART));
			return;
		}

		eSDVehicleResultCode code = GetSDVehicleSpawnValidator().CanSpawn(m_Player);
		if (code != eSDVehicleResultCode.SUCCESS)
			ShowSpawnError(SDVehicleStatic.GetMessageFromResponse(code));
		else
			HideSpawnError();
	}

	private bool IsSelectedSlotAlreadySpawnedThisRestart()
	{
		if (m_SelectedSlotIndex < 0)
			return false;

		SDVehicleListStorage storage = GetSDVehicleMenuHandler().GetStorage();
		if (!storage)
			return false;

		return storage.IsSlotConsumedThisRestart(m_SelectedSlotIndex);
	}

	private void ShowSpawnError(string text)
	{
		m_SpawnErrorLabel.SetText(text);
		m_SpawnErrorLabel.Show(true);
	}

	private void HideSpawnError()
	{
		m_SpawnErrorLabel.Show(false);
	}
}
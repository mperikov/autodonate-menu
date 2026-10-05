class SDLoadoutMenu extends UIScriptedMenu
{
	protected ref SDLoadoutCatalogData m_Catalog;
	protected ref array<ref SDLoadoutSlotCatalogEntry> m_DisplaySlots;
	protected ref SDLoadoutSelection m_Selection;
	protected ref SDLoadoutClientPresetFile m_PresetFile;
	protected ref array<int> m_SlotIndices;

	protected TextListboxWidget m_PresetList;
	protected ButtonWidget m_BtnSavePreset;
	protected ButtonWidget m_BtnDeletePreset;
	protected ButtonWidget m_BtnRandom;
	protected ButtonWidget m_BtnContinue;
	protected TextWidget m_PreviewTitle;
	protected Widget m_PreviewPanel;
	protected PlayerPreviewWidget m_CharacterPreview;

	protected ref array<ButtonWidget> m_SlotPrevButtons;
	protected ref array<ButtonWidget> m_SlotNextButtons;
	protected ref array<TextWidget> m_SlotNameTexts;
	protected ref array<TextWidget> m_SlotInfoTexts;

	protected int m_MaxVisibleSlots = 10;
	protected vector m_PreviewOrientation;
	protected int m_PreviewRotationX;
	protected int m_PreviewRotationY;
	protected int m_PreviewScaleDelta;
	protected bool m_PreviewDragging;

	static ref SDLoadoutMenu s_Instance;

	static SDLoadoutMenu GetInstance()
	{
		return s_Instance;
	}

	void SDLoadoutMenu()
	{
		m_DisplaySlots = new array<ref SDLoadoutSlotCatalogEntry>;
		m_SlotIndices = new array<int>;
		m_SlotPrevButtons = new array<ButtonWidget>;
		m_SlotNextButtons = new array<ButtonWidget>;
		m_SlotNameTexts = new array<TextWidget>;
		m_SlotInfoTexts = new array<TextWidget>;
		m_PresetFile = SDLoadoutPresetStorage.Load();
		s_Instance = this;
	}

	void ~SDLoadoutMenu()
	{
		if (s_Instance == this)
			s_Instance = null;

		SDLoadoutPreviewHelper.DestroyPreview();
		SDLoadoutState.SetMenuOpen(false);
	}

	override Widget Init()
	{
		layoutRoot = GetGame().GetWorkspace().CreateWidgets("SDLoadoutClient/layouts/SDLoadoutMenu.layout");

		m_PresetList = TextListboxWidget.Cast(layoutRoot.FindAnyWidget("preset_list"));
		m_BtnSavePreset = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btn_save_preset"));
		m_BtnDeletePreset = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btn_delete_preset"));
		m_BtnRandom = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btn_random"));
		m_BtnContinue = ButtonWidget.Cast(layoutRoot.FindAnyWidget("btn_continue"));
		m_PreviewTitle = TextWidget.Cast(layoutRoot.FindAnyWidget("txt_character_title"));
		m_PreviewPanel = layoutRoot.FindAnyWidget("preview_panel");
		m_CharacterPreview = PlayerPreviewWidget.Cast(layoutRoot.FindAnyWidget("character_preview"));

		if (m_PreviewTitle)
			m_PreviewTitle.SetText("Превью");

		BindSlotWidgets();
		InitPreviewWidget();
		RefreshPresetList();

		if (m_Catalog)
			BuildDisplaySlots();

		ApplyInitialSelection();
		RefreshAllSlots();
		return layoutRoot;
	}

	void SetCatalog(SDLoadoutCatalogData catalog)
	{
		m_Catalog = catalog;
		if (layoutRoot)
		{
			BuildDisplaySlots();
			ApplyInitialSelection();
			RefreshAllSlots();
		}
	}

	protected void ApplyInitialSelection()
	{
		if (!ApplyLastSelectedPreset())
			BuildSelectionFromCatalog();
	}

	protected bool ApplyLastSelectedPreset()
	{
		if (!m_PresetFile || m_PresetFile.LastSelectedPresetName == string.Empty)
			return false;

		SDLoadoutClientPreset preset = FindPresetByName(m_PresetFile.LastSelectedPresetName);
		if (!preset)
			return false;

		m_Selection = preset.ToSelection();
		SyncIndicesFromSelection();
		SelectPresetInList(preset.Name);
		return true;
	}

	protected SDLoadoutClientPreset FindPresetByName(string presetName)
	{
		if (!m_PresetFile || !m_PresetFile.Presets || presetName == string.Empty)
			return null;

		foreach (SDLoadoutClientPreset preset : m_PresetFile.Presets)
		{
			if (preset && preset.Name == presetName)
				return preset;
		}

		return null;
	}

	protected int FindPresetRowByName(string presetName)
	{
		if (!m_PresetList || presetName == string.Empty)
			return -1;

		for (int row = 0; row < m_PresetList.GetNumItems(); row++)
		{
			SDLoadoutClientPreset preset;
			m_PresetList.GetItemData(row, 0, preset);
			if (preset && preset.Name == presetName)
				return row;
		}

		return -1;
	}

	protected void SelectPresetInList(string presetName)
	{
		if (!m_PresetList || presetName == string.Empty)
			return;

		int row = FindPresetRowByName(presetName);
		if (row >= 0)
			m_PresetList.SelectRow(row);
	}

	protected void SaveLastSelectedPresetName(string presetName)
	{
		if (presetName == string.Empty)
			return;

		if (!m_PresetFile)
			m_PresetFile = new SDLoadoutClientPresetFile();

		m_PresetFile.LastSelectedPresetName = presetName;
		SDLoadoutPresetStorage.Save(m_PresetFile);
	}

	protected void BuildDisplaySlots()
	{
		m_DisplaySlots.Clear();

		if (!m_Catalog || !m_Catalog.Slots)
			return;

		foreach (SDLoadoutSlotCatalogEntry slotEntry : m_Catalog.Slots)
		{
			if (!slotEntry || slotEntry.SlotName == "CHARACTER")
				continue;

			m_DisplaySlots.Insert(slotEntry);
		}
	}

	protected void BindSlotWidgets()
	{
		m_SlotPrevButtons.Clear();
		m_SlotNextButtons.Clear();
		m_SlotNameTexts.Clear();
		m_SlotInfoTexts.Clear();
		m_SlotIndices.Clear();

		for (int i = 0; i < m_MaxVisibleSlots; i++)
		{
			string suffix = i.ToString();
			ButtonWidget prevBtn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("slot_" + suffix + "_prev"));
			ButtonWidget nextBtn = ButtonWidget.Cast(layoutRoot.FindAnyWidget("slot_" + suffix + "_next"));
			TextWidget nameTxt = TextWidget.Cast(layoutRoot.FindAnyWidget("slot_" + suffix + "_name"));
			TextWidget infoTxt = TextWidget.Cast(layoutRoot.FindAnyWidget("slot_" + suffix + "_info"));

			m_SlotPrevButtons.Insert(prevBtn);
			m_SlotNextButtons.Insert(nextBtn);
			m_SlotNameTexts.Insert(nameTxt);
			m_SlotInfoTexts.Insert(infoTxt);
			m_SlotIndices.Insert(0);
		}
	}

	protected void InitPreviewWidget()
	{
		if (!m_CharacterPreview)
			return;

		m_PreviewOrientation = Vector(0, 0, 0);
		m_PreviewDragging = false;
		SDLoadoutPreviewHelper.InitPreview(m_CharacterPreview);

		if (m_PreviewPanel)
		{
			WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown(m_PreviewPanel, this, "PreviewMouseDown");
			WidgetEventHandler.GetInstance().RegisterOnMouseWheel(m_PreviewPanel, this, "PreviewMouseWheel");
		}

		WidgetEventHandler.GetInstance().RegisterOnMouseButtonDown(m_CharacterPreview, this, "PreviewMouseDown");
		WidgetEventHandler.GetInstance().RegisterOnMouseWheel(m_CharacterPreview, this, "PreviewMouseWheel");
	}

	protected void RefreshPreview()
	{
		SDLoadoutPreviewHelper.ApplySelection(m_CharacterPreview, m_Selection);
		SDLoadoutPreviewHelper.ApplyFrameOrientation(m_CharacterPreview, m_PreviewOrientation);
	}

	bool PreviewMouseDown(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;

		GetMousePos(m_PreviewRotationX, m_PreviewRotationY);
		m_PreviewDragging = true;
		return true;
	}

	bool PreviewMouseWheel(Widget w, int x, int y, int wheel)
	{
		m_PreviewScaleDelta = wheel;
		UpdatePreviewScale();
		return true;
	}

	protected void UpdatePreviewRotation(int mouse_x, int mouse_y, bool isDragging)
	{
		if (!m_CharacterPreview)
			return;

		vector orientation = m_PreviewOrientation;
		orientation[1] = orientation[1] - (m_PreviewRotationX - mouse_x);
		m_CharacterPreview.SetModelOrientation(orientation);

		if (!isDragging)
			m_PreviewOrientation = orientation;
	}

	protected void UpdatePreviewScale()
	{
		if (!m_CharacterPreview)
			return;

		float w, h;
		m_CharacterPreview.GetSize(w, h);
		w = w + (m_PreviewScaleDelta / 25);
		h = h + (m_PreviewScaleDelta / 25);

		if (w > 0.62 && w < 3)
			m_CharacterPreview.SetSize(w, h);
		else if (w < 0.62)
			m_CharacterPreview.SetSize(0.62, 0.62);
		else if (w > 3)
			m_CharacterPreview.SetSize(3, 3);
	}

	protected void BuildSelectionFromCatalog()
	{
		m_Selection = new SDLoadoutSelection();

		if (!m_DisplaySlots)
			return;

		m_SlotIndices.Clear();
		for (int index = 0; index < m_DisplaySlots.Count(); index++)
			m_SlotIndices.Insert(0);

		for (int i = 0; i < m_DisplaySlots.Count(); i++)
		{
			SDLoadoutSlotCatalogEntry slotEntry = m_DisplaySlots.Get(i);
			if (!slotEntry)
				continue;

			string item = string.Empty;

			if (m_Catalog && m_Catalog.DefaultSelection)
				item = m_Catalog.DefaultSelection.GetItemForSlot(slotEntry.SlotName);

			if (item == string.Empty && slotEntry.Items && slotEntry.Items.Count() > 0)
				item = slotEntry.Items.Get(0);

			m_Selection.SetItemForSlot(slotEntry.SlotName, item);
		}

		SyncIndicesFromSelection();
	}

	protected void RefreshPresetList()
	{
		if (!m_PresetList)
			return;

		m_PresetList.ClearItems();

		if (!m_PresetFile || !m_PresetFile.Presets)
			return;

		foreach (SDLoadoutClientPreset preset : m_PresetFile.Presets)
		{
			if (!preset || preset.Name == string.Empty)
				continue;

			m_PresetList.AddItem(preset.Name, preset, 0);
		}
	}

	protected void RefreshAllSlots()
	{
		if (!m_DisplaySlots)
			return;

		for (int uiIndex = 0; uiIndex < m_MaxVisibleSlots; uiIndex++)
		{
			bool visible = uiIndex < m_DisplaySlots.Count();
			SetSlotRowVisible(uiIndex, visible);

			if (!visible)
				continue;

			RefreshSlotRow(uiIndex, m_DisplaySlots.Get(uiIndex));
		}

		RefreshPreview();
	}

	protected void SetSlotRowVisible(int uiIndex, bool visible)
	{
		if (uiIndex < 0 || uiIndex >= m_MaxVisibleSlots)
			return;

		if (m_SlotPrevButtons.Get(uiIndex))
			m_SlotPrevButtons.Get(uiIndex).Show(visible);
		if (m_SlotNextButtons.Get(uiIndex))
			m_SlotNextButtons.Get(uiIndex).Show(visible);
		if (m_SlotNameTexts.Get(uiIndex))
			m_SlotNameTexts.Get(uiIndex).Show(visible);
		if (m_SlotInfoTexts.Get(uiIndex))
			m_SlotInfoTexts.Get(uiIndex).Show(visible);
	}

	protected void RefreshSlotRow(int uiIndex, SDLoadoutSlotCatalogEntry slotEntry)
	{
		if (!slotEntry || uiIndex < 0 || uiIndex >= m_SlotIndices.Count())
			return;

		int itemIndex = m_SlotIndices.Get(uiIndex);
		int itemCount = 0;
		if (slotEntry.Items)
			itemCount = slotEntry.Items.Count();

		if (itemCount <= 0)
			return;

		if (itemIndex < 0)
			itemIndex = 0;
		if (itemIndex >= itemCount)
			itemIndex = itemCount - 1;

		m_SlotIndices.Set(uiIndex, itemIndex);

		string itemClass = slotEntry.Items.Get(itemIndex);
		m_Selection.SetItemForSlot(slotEntry.SlotName, itemClass);

		string displayName = SDLoadoutDisplayNames.GetItemDisplayName(itemClass);

		if (m_SlotNameTexts.Get(uiIndex))
			m_SlotNameTexts.Get(uiIndex).SetText(displayName);

		string info = slotEntry.Label + " (" + (itemIndex + 1).ToString() + "/" + itemCount.ToString() + ")";
		if (m_SlotInfoTexts.Get(uiIndex))
			m_SlotInfoTexts.Get(uiIndex).SetText(info);
	}

	protected void ChangeSlotIndex(int uiIndex, int delta)
	{
		if (!m_DisplaySlots || uiIndex < 0 || uiIndex >= m_DisplaySlots.Count())
			return;

		SDLoadoutSlotCatalogEntry slotEntry = m_DisplaySlots.Get(uiIndex);
		if (!slotEntry || !slotEntry.Items || slotEntry.Items.Count() == 0)
			return;

		int itemCount = slotEntry.Items.Count();
		int itemIndex = m_SlotIndices.Get(uiIndex);
		itemIndex = itemIndex + delta;

		if (itemIndex < 0)
			itemIndex = itemCount - 1;
		if (itemIndex >= itemCount)
			itemIndex = 0;

		m_SlotIndices.Set(uiIndex, itemIndex);
		RefreshSlotRow(uiIndex, slotEntry);
		RefreshPreview();
	}

	protected void ApplyRandomSet()
	{
		if (!m_DisplaySlots)
			return;

		for (int uiIndex = 0; uiIndex < m_DisplaySlots.Count() && uiIndex < m_SlotIndices.Count(); uiIndex++)
		{
			SDLoadoutSlotCatalogEntry slotEntry = m_DisplaySlots.Get(uiIndex);
			if (!slotEntry || !slotEntry.Items || slotEntry.Items.Count() == 0)
				continue;

			int randomIndex = Math.RandomIntInclusive(0, slotEntry.Items.Count() - 1);
			m_SlotIndices.Set(uiIndex, randomIndex);
			RefreshSlotRow(uiIndex, slotEntry);
		}

		RefreshPreview();
	}

	protected void SaveCurrentPreset()
	{
		if (!m_Selection)
			return;

		if (!m_PresetFile)
			m_PresetFile = new SDLoadoutClientPresetFile();

		SDLoadoutClientPreset preset = new SDLoadoutClientPreset();
		preset.CopyFromSelection(SDLoadoutPresetStorage.GeneratePresetName(m_PresetFile), m_Selection);
		m_PresetFile.Presets.Insert(preset);
		SDLoadoutPresetStorage.Save(m_PresetFile);
		RefreshPresetList();

		if (m_PresetList)
		{
			m_PresetList.SelectRow(m_PresetList.GetNumItems() - 1);
			SaveLastSelectedPresetName(preset.Name);
		}
	}

	protected void DeleteSelectedPreset()
	{
		if (!m_PresetList || !m_PresetFile || !m_PresetFile.Presets)
			return;

		int row = m_PresetList.GetSelectedRow();
		if (row < 0 || row >= m_PresetFile.Presets.Count())
			return;

		SDLoadoutClientPreset removed = m_PresetFile.Presets.Get(row);
		string removedName = string.Empty;
		if (removed)
			removedName = removed.Name;

		m_PresetFile.Presets.Remove(row);

		if (removedName != string.Empty && m_PresetFile.LastSelectedPresetName == removedName)
			m_PresetFile.LastSelectedPresetName = string.Empty;

		SDLoadoutPresetStorage.Save(m_PresetFile);
		RefreshPresetList();
	}

	protected void LoadSelectedPreset()
	{
		if (!m_PresetList)
			return;

		int row = m_PresetList.GetSelectedRow();
		if (row < 0)
			return;

		SDLoadoutClientPreset preset;
		m_PresetList.GetItemData(row, 0, preset);
		if (!preset)
			return;

		m_Selection = preset.ToSelection();
		SyncIndicesFromSelection();
		RefreshAllSlots();
		SaveLastSelectedPresetName(preset.Name);
	}

	protected void SyncIndicesFromSelection()
	{
		if (!m_DisplaySlots)
			return;

		for (int uiIndex = 0; uiIndex < m_DisplaySlots.Count() && uiIndex < m_SlotIndices.Count(); uiIndex++)
		{
			SDLoadoutSlotCatalogEntry slotEntry = m_DisplaySlots.Get(uiIndex);
			if (!slotEntry || !slotEntry.Items)
				continue;

			string selectedItem = m_Selection.GetItemForSlot(slotEntry.SlotName);
			int foundIndex = slotEntry.Items.Find(selectedItem);
			if (foundIndex == -1)
				foundIndex = 0;

			m_SlotIndices.Set(uiIndex, foundIndex);
		}
	}

	protected void ConfirmAndContinue()
	{
		if (!m_Selection)
			return;

		if (m_PresetList)
		{
			int row = m_PresetList.GetSelectedRow();
			if (row >= 0)
			{
				SDLoadoutClientPreset preset;
				m_PresetList.GetItemData(row, 0, preset);
				if (preset)
					SaveLastSelectedPresetName(preset.Name);
			}
		}

		SDLoadoutSpawnFlow.ConfirmSelection(m_Selection);
		Close();
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		super.OnClick(w, x, y, button);

		if (w == m_BtnSavePreset)
		{
			SaveCurrentPreset();
			return true;
		}

		if (w == m_BtnDeletePreset)
		{
			DeleteSelectedPreset();
			return true;
		}

		if (w == m_BtnRandom)
		{
			ApplyRandomSet();
			return true;
		}

		if (w == m_BtnContinue)
		{
			ConfirmAndContinue();
			return true;
		}

		for (int i = 0; i < m_SlotPrevButtons.Count(); i++)
		{
			if (w == m_SlotPrevButtons.Get(i))
			{
				ChangeSlotIndex(i, -1);
				return true;
			}

			if (w == m_SlotNextButtons.Get(i))
			{
				ChangeSlotIndex(i, 1);
				return true;
			}
		}

		return false;
	}

	override bool OnItemSelected(Widget w, int x, int y, int row, int column, int oldRow, int oldColumn)
	{
		super.OnItemSelected(w, x, y, row, column, oldRow, oldColumn);

		if (w == m_PresetList)
		{
			LoadSelectedPreset();
			return true;
		}

		return false;
	}

	override void OnShow()
	{
		super.OnShow();

		SDLoadoutState.SetMenuOpen(true);

		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission && mission.GetHud())
			mission.GetHud().ShowHudUI(false);

		GetGame().GetInput().ChangeGameFocus(1);
		SetFocus(layoutRoot);

		PPEffects.SetBlurMenu(0.5);

		GetGame().GetCallQueue(CALL_CATEGORY_GUI).CallLater(DeferredRefreshPreview, 300, false);
	}

	protected void DeferredRefreshPreview()
	{
		if (!m_CharacterPreview || !m_Selection)
			return;

		SDLoadoutPreviewHelper.InitPreview(m_CharacterPreview);
		RefreshPreview();
	}

	override void OnHide()
	{
		super.OnHide();

		SDLoadoutPreviewHelper.DestroyPreview();
		SDLoadoutState.SetMenuOpen(false);

		GetGame().GetInput().ResetGameFocus();
		PPEffects.SetBlurMenu(0);

		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission && mission.GetHud())
			mission.GetHud().ShowHudUI(true);

		if (!SDLoadoutState.IsSuppressCancelOnHide() && SDLoadoutState.IsAwaitingPostSpawnLoadout())
			SDLoadoutSpawnFlow.CancelWithoutSelection();

		SDLoadoutState.SetSuppressCancelOnHide(false);
	}

	override void Update(float timeslice)
	{
		super.Update(timeslice);

		if (!m_CharacterPreview)
			return;

		PlayerBase previewPlayer = SDLoadoutPreviewHelper.GetPreviewPlayerEntity();
		if (previewPlayer)
			m_CharacterPreview.UpdateItemInHands(previewPlayer.GetEntityInHands());

		if (m_PreviewDragging)
		{
			int mouse_x;
			int mouse_y;
			GetMousePos(mouse_x, mouse_y);

			if (GetMouseState(MouseState.LEFT) & 0x80000000)
				UpdatePreviewRotation(mouse_x, mouse_y, true);
			else
			{
				UpdatePreviewRotation(mouse_x, mouse_y, false);
				m_PreviewDragging = false;
			}
		}
	}
}

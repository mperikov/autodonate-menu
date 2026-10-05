class SDLoadoutPreviewHelper
{
	protected static PlayerBase s_PreviewPlayer;

	static const vector PREVIEW_MODEL_POSITION = "0 0 0.48";
	static const float PREVIEW_DEFAULT_SCALE = 1.0;

	static void ApplySelection(PlayerPreviewWidget previewWidget, SDLoadoutSelection selection)
	{
		if (!previewWidget || !selection)
			return;

		if (!EnsurePreviewPlayer())
			return;

		ApplySelectionToPreviewPlayer(s_PreviewPlayer, selection);

		previewWidget.SetPlayer(s_PreviewPlayer);
		previewWidget.UpdateItemInHands(s_PreviewPlayer.GetEntityInHands());
		previewWidget.Refresh();
	}

	static void InitPreview(PlayerPreviewWidget previewWidget)
	{
		if (!previewWidget)
			return;

		if (!EnsurePreviewPlayer())
			return;

		previewWidget.SetPlayer(s_PreviewPlayer);
		ApplyInitialFrame(previewWidget);
		previewWidget.Refresh();
	}

	static void ApplyInitialFrame(PlayerPreviewWidget previewWidget)
	{
		if (!previewWidget)
			return;

		previewWidget.SetModelPosition(PREVIEW_MODEL_POSITION);
		previewWidget.SetSize(PREVIEW_DEFAULT_SCALE, PREVIEW_DEFAULT_SCALE);
		previewWidget.SetModelOrientation(Vector(0, 0, 0));
	}

	static void ApplyFrameOrientation(PlayerPreviewWidget previewWidget, vector orientation)
	{
		if (!previewWidget)
			return;

		previewWidget.SetModelPosition(PREVIEW_MODEL_POSITION);
		previewWidget.SetModelOrientation(orientation);
	}

	static void DestroyPreview()
	{
		if (s_PreviewPlayer)
		{
			GetGame().ObjectDelete(s_PreviewPlayer);
			s_PreviewPlayer = null;
		}
	}

	static PlayerBase GetPreviewPlayerEntity()
	{
		return s_PreviewPlayer;
	}

	protected static bool EnsurePreviewPlayer()
	{
		PlayerBase livePlayer = PlayerBase.Cast(GetGame().GetPlayer());
		if (!livePlayer)
			return false;

		string playerType = livePlayer.GetType();

		if (s_PreviewPlayer && s_PreviewPlayer.GetType() != playerType)
			DestroyPreview();

		if (s_PreviewPlayer)
			return true;

		vector pos = livePlayer.GetPosition();
		pos[1] = pos[1] - 50;

		s_PreviewPlayer = PlayerBase.Cast(GetGame().CreateObjectEx(playerType, pos, ECE_LOCAL));
		if (!s_PreviewPlayer)
		{
			Print("[SDLoadout] Failed to create preview player.");
			return false;
		}

		s_PreviewPlayer.SetAllowDamage(false);
		s_PreviewPlayer.SetInvisible(true);
		return true;
	}

	protected static void ApplySelectionToPreviewPlayer(PlayerBase player, SDLoadoutSelection selection)
	{
		if (!player || !selection)
			return;

		array<string> slotOrder = GetSlotEquipOrder();

		foreach (string slotName : slotOrder)
		{
			int slotId = SDLoadoutSlotHelper.ResolveSlotId(slotName);
			if (!InventorySlots.IsSlotIdValid(slotId))
				continue;

			string wantedClass = selection.GetItemForSlot(slotName);
			if (wantedClass == "None")
				wantedClass = string.Empty;

			EntityAI current = player.GetInventory().FindAttachment(slotId);
			string currentClass = string.Empty;
			if (current)
				currentClass = current.GetType();

			if (currentClass == wantedClass)
				continue;

			if (current)
				GetGame().ObjectDelete(current);

			if (wantedClass == string.Empty)
				continue;

			EntityAI item = EquipPreviewItem(player, slotId, wantedClass);
			if (!item)
				Print("[SDLoadout] Preview dummy failed to equip " + wantedClass + " in slot " + SDLoadoutSlotHelper.NormalizeSlotKey(slotName));
		}
	}

	protected static EntityAI EquipPreviewItem(PlayerBase player, int slotId, string itemClass)
	{
		if (slotId == InventorySlots.HANDS)
			return player.GetHumanInventory().CreateInHands(itemClass);

		EntityAI item = EntityAI.Cast(GetGame().CreateObjectEx(itemClass, "0 0 0", ECE_LOCAL));
		if (!item)
			return null;

		if (!player.LocalTakeEntityAsAttachmentEx(item, slotId))
		{
			GetGame().ObjectDelete(item);
			return null;
		}

		return item;
	}

	protected static array<string> GetSlotEquipOrder()
	{
		array<string> order = new array<string>;
		order.Insert("HEAD");
		order.Insert("MASK");
		order.Insert("EYEWEAR");
		order.Insert("GLOVES");
		order.Insert("BODY");
		order.Insert("VEST");
		order.Insert("BACK");
		order.Insert("HIPS");
		order.Insert("LEGS");
		order.Insert("FEET");
		order.Insert("HANDS");
		order.Insert("SHOULDER");
		order.Insert("MELEE");
		return order;
	}
}

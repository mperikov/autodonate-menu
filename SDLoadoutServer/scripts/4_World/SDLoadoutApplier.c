class SDLoadoutApplier
{
	private static const int DEFAULT_ITEMS_SPAWN_DELAY_MS = 150;

	static SDLoadoutSelection BuildRandomSelection()
	{
		SDLoadoutConfig config = SDLoadoutConfigManager.GetConfig();
		if (!config)
			return null;

		array<ref SDLoadoutGearEntry> loadout = config.GetRandomLoadout();
		if (!loadout)
			return null;

		SDLoadoutSelection selection = new SDLoadoutSelection();
		foreach (SDLoadoutGearEntry entry : loadout)
		{
			if (!entry)
				continue;

			SDLoadoutGearEntry copy = new SDLoadoutGearEntry();
			copy.SlotName = entry.SlotName;
			copy.Item = entry.Item;
			selection.Gear.Insert(copy);
		}

		return selection;
	}

	static void ApplyLoadout(PlayerBase player)
	{
		SDLoadoutSelection selection = BuildRandomSelection();
		if (!selection)
			return;

		ApplySelection(player, selection);
	}

	static void ApplySelection(PlayerBase player, SDLoadoutSelection selection)
	{
		if (!GetGame().IsServer())
			return;

		if (!player || !selection)
			return;

		SDLoadoutConfigManager.LoadConfigIfChanged();
		if (!SDLoadoutConfigManager.IsReady())
			SDLoadoutConfigManager.Init();

		ClearPlayerGear(player);

		if (selection.Gear)
		{
			foreach (SDLoadoutGearEntry entry : selection.Gear)
			{
				if (!entry || entry.SlotName == "CHARACTER")
					continue;

				EquipItemInSlot(player, NormalizeSlotKey(entry.SlotName), entry.Item);
			}
		}

		SDLoadoutConfig config = SDLoadoutConfigManager.GetConfig();
		if (config && config.HasDefaultItems())
			ScheduleDefaultItemsSpawn(player);
	}

	private static void ScheduleDefaultItemsSpawn(PlayerBase player)
	{
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(SpawnDefaultItemsDelayed, DEFAULT_ITEMS_SPAWN_DELAY_MS, false, player);
	}

	private static void SpawnDefaultItemsDelayed(PlayerBase player)
	{
		if (!GetGame().IsServer() || !player)
			return;

		SDLoadoutConfig config = SDLoadoutConfigManager.GetConfig();
		if (!config || !config.HasDefaultItems())
			return;

		foreach (string className : config.DefaultItems)
		{
			SpawnDefaultItem(player, className);
		}
	}

	private static void EquipItemInSlot(PlayerBase player, string slotName, string itemClass)
	{
		int slotId = ResolveSlotId(slotName, player);
		if (!InventorySlots.IsSlotIdValid(slotId))
		{
			Print("[SDLoadout] Invalid slot: " + slotName);
			return;
		}

		RemoveAttachmentInSlot(player, slotId);

		if (itemClass == string.Empty || itemClass == "None")
			return;

		EntityAI item;
		if (slotId == InventorySlots.HANDS)
			item = player.GetHumanInventory().CreateInHands(itemClass);
		else
			item = player.GetInventory().CreateAttachmentEx(itemClass, slotId);

		if (!item)
			Print("[SDLoadout] Failed to spawn " + itemClass + " into slot " + slotName);
	}

	private static void ClearPlayerGear(PlayerBase player)
	{
		array<EntityAI> items = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, items);

		for (int i = items.Count() - 1; i >= 0; i--)
		{
			EntityAI item = items[i];
			if (!item || item == player)
				continue;

			g_Game.ObjectDelete(item);
		}
	}

	private static void RemoveAttachmentInSlot(PlayerBase player, int slotId)
	{
		EntityAI existing = player.GetInventory().FindAttachment(slotId);
		if (existing)
			g_Game.ObjectDelete(existing);
	}

	private static void SpawnDefaultItem(PlayerBase player, string className)
	{
		if (className == string.Empty)
			return;

		EntityAI item = player.GetInventory().CreateInInventory(className);
		if (item)
			return;

		array<EntityAI> inventoryItems = new array<EntityAI>;
		player.GetInventory().EnumerateInventory(InventoryTraversalType.PREORDER, inventoryItems);

		foreach (EntityAI parentItem : inventoryItems)
		{
			if (!parentItem || parentItem == player)
				continue;

			GameInventory parentInventory = parentItem.GetInventory();
			if (!parentInventory)
				continue;

			item = parentInventory.CreateInInventory(className);
			if (item)
				return;
		}

		Print("[SDLoadout] Failed to spawn default item: " + className);
	}

	private static int ResolveSlotId(string slotName, PlayerBase player)
	{
		if (slotName == string.Empty)
			return -1;

		int slotId = ResolveSlotIdFromKey(NormalizeSlotKey(slotName));

		if (!InventorySlots.IsSlotIdValid(slotId))
			return -1;

		if (!player.GetInventory().HasAttachmentSlot(slotId))
			return -1;

		return slotId;
	}

	private static int ResolveSlotIdFromKey(string slotKey)
	{
		switch (slotKey)
		{
			case "HEAD":
				return InventorySlots.HEADGEAR;
			case "MASK":
				return InventorySlots.MASK;
			case "EYEWEAR":
				return InventorySlots.EYEWEAR;
			case "GLOVES":
				return InventorySlots.GLOVES;
			case "BODY":
				return InventorySlots.BODY;
			case "VEST":
				return InventorySlots.VEST;
			case "BACK":
				return InventorySlots.BACK;
			case "HIPS":
				return InventorySlots.HIPS;
			case "LEGS":
				return InventorySlots.LEGS;
			case "FEET":
				return InventorySlots.FEET;
			case "HANDS":
				return InventorySlots.HANDS;
			case "SHOULDER":
				return InventorySlots.SHOULDER;
			case "MELEE":
				return InventorySlots.MELEE;
			case "ARMBAND":
				return InventorySlots.ARMBAND;
		}

		return InventorySlots.GetSlotIdFromString(slotKey);
	}

	private static string NormalizeSlotKey(string slotName)
	{
		switch (slotName)
		{
			case "HEADGEAR":
			case "HEAD":
			case "Head":
			case "Headgear":
				return "HEAD";
			case "MASK":
			case "Mask":
				return "MASK";
			case "EYEWEAR":
			case "GLASSES":
			case "Eyewear":
				return "EYEWEAR";
			case "GLOVES":
			case "Gloves":
				return "GLOVES";
			case "BODY":
			case "Body":
			case "TOP":
				return "BODY";
			case "VEST":
			case "Vest":
				return "VEST";
			case "BACK":
			case "Back":
				return "BACK";
			case "HIPS":
			case "BELT":
			case "Hips":
				return "HIPS";
			case "LEGS":
			case "Legs":
			case "BOTTOM":
			case "Bottom":
				return "LEGS";
			case "FEET":
			case "Feet":
			case "SHOE":
				return "FEET";
			case "HANDS":
			case "Hands":
				return "HANDS";
			case "SHOULDER":
			case "SHOULDERL":
			case "Shoulder":
				return "SHOULDER";
			case "SHOULDERR":
			case "MELEE":
			case "Melee":
				return "MELEE";
			case "ARMBAND":
			case "Armband":
				return "ARMBAND";
		}

		return slotName;
	}
}

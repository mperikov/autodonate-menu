class SDLoadoutCatalogBuilder
{
	private static const string CHARACTER_SLOT = "CHARACTER";

	static ref SDLoadoutCatalogData Build(SDLoadoutConfig config)
	{
		SDLoadoutCatalogData catalog = new SDLoadoutCatalogData();
		if (!config)
			return catalog;

		if (config.Characters && config.Characters.Count() > 0)
			catalog.Characters.Copy(config.Characters);

		if (config.SlotCatalog && config.SlotCatalog.Count() > 0)
		{
			foreach (SDLoadoutSlotCatalogEntry sourceEntry : config.SlotCatalog)
			{
				if (!sourceEntry || sourceEntry.SlotName == CHARACTER_SLOT)
					continue;

				SDLoadoutSlotCatalogEntry copy = new SDLoadoutSlotCatalogEntry();
				copy.SlotName = NormalizeCatalogSlotName(sourceEntry.SlotName);
				copy.Label = sourceEntry.Label;
				copy.Items = new TStringArray;
				if (sourceEntry.Items)
					copy.Items.Copy(sourceEntry.Items);

				SanitizeSlotItems(copy.Items);
				EnsureNoneOption(copy.Items);
				catalog.Slots.Insert(copy);
			}
		}
		else
		{
			BuildSlotsFromGear(config, catalog);
		}

		if (catalog.Slots.Count() == 0)
			AddDefaultSlotSkeleton(catalog);

		RemoveCharacterSlots(catalog);
		return catalog;
	}

	static void MergeKtPlayerLoadout(SDLoadoutCatalogData catalog, KTLoadoutPlayerData ktData)
	{
		if (!catalog || !ktData || !ktData.HasGear())
			return;

		foreach (KTLoadoutGearSlotData ktSlot : ktData.Gear)
		{
			if (!ktSlot || !ktSlot.Items)
				continue;

			string slotName = NormalizeCatalogSlotName(ktSlot.SlotName);
			if (slotName == CHARACTER_SLOT)
				continue;

			SDLoadoutSlotCatalogEntry catalogSlot = catalog.GetSlotByName(slotName);
			if (!catalogSlot)
			{
				catalogSlot = new SDLoadoutSlotCatalogEntry();
				catalogSlot.SlotName = slotName;
				catalogSlot.Label = GetDefaultLabel(slotName);
				catalogSlot.Items = new TStringArray;
				EnsureNoneOption(catalogSlot.Items);
				catalog.Slots.Insert(catalogSlot);
			}

			for (int i = 0; i < ktSlot.Items.Count(); i++)
			{
				string itemClass = ktSlot.Items.Get(i);
				if (itemClass == string.Empty || itemClass == "None")
					continue;

				if (catalogSlot.Items.Find(itemClass) == -1)
					catalogSlot.Items.Insert(itemClass);
			}
		}
	}

	static SDLoadoutSelection BuildKtDefaultSelection(KTLoadoutPlayerData ktData)
	{
		SDLoadoutSelection selection = new SDLoadoutSelection();
		if (!ktData || !ktData.Gear)
			return selection;

		foreach (KTLoadoutGearSlotData ktSlot : ktData.Gear)
		{
			if (!ktSlot || !ktSlot.HasItem())
				continue;

			string slotName = NormalizeCatalogSlotName(ktSlot.SlotName);
			if (slotName == CHARACTER_SLOT)
				continue;

			SDLoadoutGearEntry entry = new SDLoadoutGearEntry();
			entry.SlotName = slotName;
			entry.Item = ktSlot.GetFirstItem();
			selection.Gear.Insert(entry);
		}

		return selection;
	}

	static ref SDLoadoutCatalogData BuildForIdentity(PlayerIdentity identity)
	{
		if (!GetGame().IsServer() || !identity)
			return new SDLoadoutCatalogData();

		SDLoadoutConfigManager.LoadConfigIfChanged();
		if (!SDLoadoutConfigManager.IsReady())
			SDLoadoutConfigManager.Init();

		KTLoadoutPlayerData ktData;
		KTLoadoutPlayerLoader.LoadBySteamId(identity.GetPlainId(), ktData);

		ref SDLoadoutCatalogData catalog = Build(SDLoadoutConfigManager.GetConfig());
		MergeKtPlayerLoadout(catalog, ktData);

		if (ktData && ktData.HasGear())
			catalog.DefaultSelection = BuildKtDefaultSelection(ktData);

		return catalog;
	}

	private static void BuildSlotsFromGear(SDLoadoutConfig config, SDLoadoutCatalogData catalog)
	{
		map<string, ref TStringArray> slotItems = new map<string, ref TStringArray>;
		RegisterSlot(slotItems, "HEAD", "Голова");
		RegisterSlot(slotItems, "MASK", "Лицо");
		RegisterSlot(slotItems, "EYEWEAR", "Глаза");
		RegisterSlot(slotItems, "GLOVES", "Перчатки");
		RegisterSlot(slotItems, "BODY", "Верхняя одежда");
		RegisterSlot(slotItems, "VEST", "Жилет");
		RegisterSlot(slotItems, "BACK", "Спина");
		RegisterSlot(slotItems, "HIPS", "Пояс");
		RegisterSlot(slotItems, "LEGS", "Ноги");
		RegisterSlot(slotItems, "FEET", "Обувь");

		if (config.Gear)
		{
			foreach (array<ref SDLoadoutGearEntry> preset : config.Gear)
			{
				if (!preset)
					continue;

				foreach (SDLoadoutGearEntry entry : preset)
				{
					if (!entry || entry.SlotName == string.Empty || entry.SlotName == CHARACTER_SLOT || entry.Item == string.Empty)
						continue;

					AddUniqueItem(slotItems, entry.SlotName, entry.Item);
				}
			}
		}

		for (int i = 0; i < slotItems.Count(); i++)
		{
			string slotName = slotItems.GetKey(i);
			TStringArray items = slotItems.GetElement(i);
			EnsureNoneOption(items);

			SDLoadoutSlotCatalogEntry slotEntry = new SDLoadoutSlotCatalogEntry();
			slotEntry.SlotName = slotName;
			slotEntry.Label = GetDefaultLabel(slotName);
			slotEntry.Items = items;
			catalog.Slots.Insert(slotEntry);
		}
	}

	private static void RegisterSlot(map<string, ref TStringArray> slotItems, string slotName, string label)
	{
		if (slotItems.Contains(slotName))
			return;

		slotItems.Insert(slotName, new TStringArray);
	}

	private static void AddUniqueItem(map<string, ref TStringArray> slotItems, string slotName, string itemClass)
	{
		string normalizedSlot = NormalizeCatalogSlotName(slotName);
		if (!slotItems.Contains(normalizedSlot))
			RegisterSlot(slotItems, normalizedSlot, GetDefaultLabel(normalizedSlot));

		TStringArray items = slotItems.Get(normalizedSlot);
		if (itemClass == "None")
			return;

		if (items.Find(itemClass) == -1)
			items.Insert(itemClass);
	}

	private static void SanitizeSlotItems(TStringArray items)
	{
		if (!items)
			return;

		ref TStringArray unique = new TStringArray;

		for (int i = 0; i < items.Count(); i++)
		{
			string itemClass = items.Get(i);
			if (itemClass == string.Empty || itemClass == "None")
				continue;

			if (unique.Find(itemClass) == -1)
				unique.Insert(itemClass);
		}

		items.Clear();
		for (int j = 0; j < unique.Count(); j++)
			items.Insert(unique.Get(j));
	}

	private static void EnsureNoneOption(TStringArray items)
	{
		if (!items)
			return;

		SanitizeSlotItems(items);
		items.InsertAt("None", 0);
	}

	private static void RemoveCharacterSlots(SDLoadoutCatalogData catalog)
	{
		if (!catalog || !catalog.Slots)
			return;

		for (int i = catalog.Slots.Count() - 1; i >= 0; i--)
		{
			SDLoadoutSlotCatalogEntry entry = catalog.Slots.Get(i);
			if (entry && entry.SlotName == CHARACTER_SLOT)
				catalog.Slots.Remove(i);
		}
	}

	private static void AddDefaultSlotSkeleton(SDLoadoutCatalogData catalog)
	{
		InsertEmptySlot(catalog, "HEAD", "Голова");
		InsertEmptySlot(catalog, "MASK", "Лицо");
		InsertEmptySlot(catalog, "EYEWEAR", "Глаза");
		InsertEmptySlot(catalog, "GLOVES", "Перчатки");
		InsertEmptySlot(catalog, "BODY", "Верхняя одежда");
		InsertEmptySlot(catalog, "VEST", "Жилет");
		InsertEmptySlot(catalog, "BACK", "Спина");
		InsertEmptySlot(catalog, "HIPS", "Пояс");
		InsertEmptySlot(catalog, "LEGS", "Ноги");
		InsertEmptySlot(catalog, "FEET", "Обувь");
	}

	private static void InsertEmptySlot(SDLoadoutCatalogData catalog, string slotName, string label)
	{
		SDLoadoutSlotCatalogEntry entry = new SDLoadoutSlotCatalogEntry();
		entry.SlotName = slotName;
		entry.Label = label;
		entry.Items = new TStringArray;
		EnsureNoneOption(entry.Items);
		catalog.Slots.Insert(entry);
	}

	static string NormalizeCatalogSlotName(string slotName)
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
		}

		return slotName;
	}

	static string GetDefaultLabel(string slotName)
	{
		switch (slotName)
		{
			case "CHARACTER":
				return "Персонаж";
			case "HEAD":
				return "Голова";
			case "MASK":
				return "Лицо";
			case "EYEWEAR":
				return "Глаза";
			case "GLOVES":
				return "Перчатки";
			case "BODY":
				return "Верхняя одежда";
			case "VEST":
				return "Жилет";
			case "BACK":
				return "Спина";
			case "HIPS":
				return "Пояс";
			case "LEGS":
				return "Ноги";
			case "FEET":
				return "Обувь";
		}

		return slotName;
	}
}

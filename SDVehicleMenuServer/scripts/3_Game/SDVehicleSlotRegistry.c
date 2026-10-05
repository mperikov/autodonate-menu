class SDVehicleSlotRegistry
{
	private static ref map<string, ref TStringArray> s_SlotItems;
	private static bool s_Initialized;

	private static ref TStringArray s_ColorSuffixes;

	static void EnsureInitialized()
	{
		if (s_Initialized)
			return;

		s_SlotItems = new map<string, ref TStringArray>;
		s_ColorSuffixes = new TStringArray;
		s_ColorSuffixes.Insert("white");
		s_ColorSuffixes.Insert("black");
		s_ColorSuffixes.Insert("red");
		s_ColorSuffixes.Insert("blue");
		s_ColorSuffixes.Insert("darkblue");
		s_ColorSuffixes.Insert("lightblue");
		s_ColorSuffixes.Insert("yellow");
		s_ColorSuffixes.Insert("green");
		s_ColorSuffixes.Insert("orange");
		s_ColorSuffixes.Insert("pink");
		s_ColorSuffixes.Insert("teal");
		s_ColorSuffixes.Insert("carbon");
		s_ColorSuffixes.Insert("rustred");
		s_ColorSuffixes.Insert("midnightblue");
		s_ColorSuffixes.Insert("sponge");
		s_ColorSuffixes.Insert("lighting");
		s_ColorSuffixes.Insert("wine");
		s_ColorSuffixes.Insert("grey");
		s_ColorSuffixes.Insert("gray");
		BuildSlotMap();
		s_Initialized = true;
	}

	static bool HasMissingAttachments(EntityAI vehicle)
	{
		if (!vehicle)
			return false;

		string cfgPath = CFG_VEHICLESPATH + " " + vehicle.GetType() + " attachments";
		if (!GetGame().ConfigIsExisting(cfgPath))
			return true;

		TStringArray slotNames = new TStringArray;
		GetGame().ConfigGetTextArray(cfgPath, slotNames);

		int missingCount = 0;
		foreach (string carSlot : slotNames)
		{
			carSlot.ToLower();
			if (!IsVehicleMechanicalPartSlot(carSlot))
				continue;

			int slotId = InventorySlots.GetSlotIdFromString(carSlot);
			if (slotId == InventorySlots.INVALID)
				continue;

			if (!vehicle.GetInventory().FindAttachment(slotId))
				missingCount++;
		}

		return missingCount > 0;
	}

	static void ClearVehicleStorage(EntityAI vehicle)
	{
		if (!vehicle)
			return;

		ClearEntityCargo(vehicle);

		GameInventory inv = vehicle.GetInventory();
		if (!inv)
			return;

		int attachmentCount = inv.AttachmentCount();
		for (int i = 0; i < attachmentCount; i++)
		{
			EntityAI attachment = inv.GetAttachmentFromIndex(i);
			if (attachment)
				ClearEntityCargo(attachment);
		}
	}

	static void ClearEntityCargo(EntityAI entity)
	{
		if (!entity)
			return;

		GameInventory inv = entity.GetInventory();
		if (!inv)
			return;

		ClearCargoBase(inv.GetCargo());

		for (int idx = 0; idx < 8; idx++)
		{
			CargoBase cargo = inv.GetCargoFromIndex(idx);
			if (cargo)
				ClearCargoBase(cargo);
		}
	}

	static void ClearCargoBase(CargoBase cargo)
	{
		if (!cargo)
			return;

		for (int i = cargo.GetItemCount() - 1; i >= 0; i--)
		{
			EntityAI item = cargo.GetItem(i);
			if (item)
				GetGame().ObjectDelete(item);
		}
	}

	static void EquipMissingAttachments(EntityAI vehicle)
	{
		if (!vehicle)
			return;

		EnsureInitialized();

		string vehicleType = vehicle.GetType();
		string vehicleColor = GetVehicleColorSuffix(vehicleType);
		string cfgPath = CFG_VEHICLESPATH + " " + vehicleType + " attachments";

		if (!GetGame().ConfigIsExisting(cfgPath))
			return;

		TStringArray slotNames = new TStringArray;
		GetGame().ConfigGetTextArray(cfgPath, slotNames);

		foreach (string carSlot : slotNames)
		{
			carSlot.ToLower();

			if (!IsVehicleMechanicalPartSlot(carSlot))
				continue;

			int slotId = InventorySlots.GetSlotIdFromString(carSlot);
			if (slotId == InventorySlots.INVALID)
				continue;

			if (vehicle.GetInventory().FindAttachment(slotId))
				continue;

			string typeName = GetPartForSlot(carSlot, vehicleColor);
			if (typeName == string.Empty)
				continue;

			vehicle.GetInventory().CreateAttachmentEx(typeName, slotId);
		}
	}

	static string GetPartForSlot(string slotName, string vehicleColor)
	{
		slotName.ToLower();
		TStringArray items = s_SlotItems.Get(slotName);
		if (!items || items.Count() == 0)
			return string.Empty;

		string vehicleColorLower = vehicleColor;
		vehicleColorLower.ToLower();

		if (vehicleColorLower != string.Empty)
		{
			string coloredPart = slotName + "_" + vehicleColorLower;
			foreach (string className : items)
			{
				string candidate = className;
				candidate.ToLower();
				if (candidate == coloredPart)
					return className;
			}

			foreach (string className2 : items)
			{
				string candidate2 = className2;
				candidate2.ToLower();
				if (PartEndsWithColor(candidate2, vehicleColorLower))
					return className2;
			}
		}

		foreach (string className3 : items)
		{
			if (PartMatchesVehicleColor(className3, slotName, vehicleColorLower))
				return className3;
		}

		return string.Empty;
	}

	static string GetVehicleColorSuffix(string vehicleType)
	{
		int lastUnderscore = vehicleType.LastIndexOf("_");
		if (lastUnderscore == -1)
			return string.Empty;

		string suffix = vehicleType.Substring(lastUnderscore + 1, vehicleType.Length() - lastUnderscore - 1);
		string suffixLower = suffix;
		suffixLower.ToLower();

		foreach (string color : s_ColorSuffixes)
		{
			if (suffixLower == color)
				return color;
		}

		return string.Empty;
	}

	static bool PartMatchesVehicleColor(string partClass, string slotName, string vehicleColorLower)
	{
		string partLower = partClass;
		partLower.ToLower();

		if (partLower.Contains("_ruined"))
			return false;

		if (vehicleColorLower != string.Empty)
		{
			if (PartEndsWithColor(partLower, vehicleColorLower))
				return true;

			if (PartHasAnyColorSuffix(partLower))
				return false;

			return partLower == slotName || partLower.Contains(slotName);
		}

		return !PartHasAnyColorSuffix(partLower) && (partLower == slotName || partLower.Contains(slotName));
	}

	static bool PartEndsWithColor(string partLower, string vehicleColorLower)
	{
		string suffix = "_" + vehicleColorLower;
		if (partLower.Length() < suffix.Length())
			return false;

		string end = partLower.Substring(partLower.Length() - suffix.Length(), suffix.Length());
		return end == suffix;
	}

	static bool PartHasAnyColorSuffix(string partLower)
	{
		foreach (string color : s_ColorSuffixes)
		{
			if (PartEndsWithColor(partLower, color))
				return true;
		}

		return false;
	}

	static void RemoveAccessoryAttachments(EntityAI vehicle)
	{
		if (!vehicle)
			return;

		GameInventory inv = vehicle.GetInventory();
		if (!inv)
			return;

		EntityAI attachment;

		string cfgPath = CFG_VEHICLESPATH + " " + vehicle.GetType() + " attachments";
		if (GetGame().ConfigIsExisting(cfgPath))
		{
			TStringArray slotNames = new TStringArray;
			GetGame().ConfigGetTextArray(cfgPath, slotNames);

			foreach (string carSlot : slotNames)
			{
				carSlot.ToLower();
				if (!IsAccessorySlot(carSlot))
					continue;

				int slotId = InventorySlots.GetSlotIdFromString(carSlot);
				if (slotId == InventorySlots.INVALID)
					continue;

				attachment = inv.FindAttachment(slotId);
				if (attachment)
					GetGame().ObjectDelete(attachment);
			}
		}

		for (int i = inv.AttachmentCount() - 1; i >= 0; i--)
		{
			attachment = inv.GetAttachmentFromIndex(i);
			if (attachment && IsAccessoryEntity(attachment))
				GetGame().ObjectDelete(attachment);
		}
	}

	static bool IsVehicleMechanicalPartSlot(string slotName)
	{
		if (IsAccessorySlot(slotName))
			return false;

		if (slotName.Contains("wheel"))
			return true;
		if (slotName.Contains("door"))
			return true;
		if (slotName.Contains("hood"))
			return true;
		if (slotName.Contains("trunk"))
			return true;
		if (slotName.Contains("battery"))
			return true;
		if (slotName.Contains("radiator"))
			return true;
		if (slotName.Contains("spark"))
			return true;
		if (slotName.Contains("glow"))
			return true;
		if (slotName.Contains("reflector"))
			return true;
		if (slotName.Contains("headlight"))
			return true;
		if (slotName.Contains("enginebelt"))
			return true;

		return false;
	}

	static bool IsAccessorySlot(string slotName)
	{
		if (slotName == "back" || slotName.Contains("backpack"))
			return true;
		if (slotName.Contains("sea_chest"))
			return true;
		if (slotName.Contains("canister"))
			return true;
		if (slotName.Contains("barrel"))
			return true;
		if (slotName.Contains("underglow"))
			return true;
		if (slotName.Contains("cassette"))
			return true;
		if (slotName == "cd")
			return true;
		if (slotName.Contains("keychain"))
			return true;
		if (slotName.Contains("plate"))
			return true;
		if (slotName.Contains("weapon"))
			return true;
		if (slotName.Contains("radio"))
			return true;

		return false;
	}

	static bool IsAccessoryEntity(EntityAI entity)
	{
		if (!entity)
			return false;

		if (entity.IsKindOf("CarWheel"))
			return false;
		if (entity.IsKindOf("CarDoor"))
			return false;
		if (entity.IsKindOf("CarRadiator"))
			return false;
		if (entity.IsKindOf("SparkPlug"))
			return false;
		if (entity.IsKindOf("CarBattery"))
			return false;

		if (entity.IsKindOf("Container_Base"))
			return true;
		if (entity.IsKindOf("Barrel_ColorBase"))
			return true;
		if (entity.IsKindOf("Backpack_Base"))
			return true;

		string typeName = entity.GetType();
		typeName.ToLower();

		if (typeName.Contains("canister"))
			return true;
		if (typeName.Contains("cassette"))
			return true;
		if (typeName == "cd")
			return true;
		if (typeName.Contains("keychain"))
			return true;
		if (typeName.Contains("underglow"))
			return true;
		if (typeName.Contains("vehicleplate") || typeName.Contains("_plate"))
			return true;
		if (typeName.Contains("sea_chest"))
			return true;

		InventoryLocation loc = new InventoryLocation;
		if (entity.GetInventory().GetCurrentInventoryLocation(loc) && loc.GetType() == InventoryLocationType.ATTACHMENT)
		{
			string slotName = InventorySlots.GetSlotName(loc.GetSlot());
			slotName.ToLower();
			if (IsAccessorySlot(slotName))
				return true;
		}

		return false;
	}

	private static void BuildSlotMap()
	{
		int count = GetGame().ConfigGetChildrenCount(CFG_VEHICLESPATH);
		for (int i = 0; i < count; i++)
		{
			string className;
			if (!GetGame().ConfigGetChildName(CFG_VEHICLESPATH, i, className))
				continue;

			if (IsBaseClass(className))
				continue;

			RegisterItemSlots(className);
		}
	}

	private static void RegisterItemSlots(string className)
	{
		string cfgBase = CFG_VEHICLESPATH + " " + className + " inventorySlot";
		int cfgType = GetGame().ConfigGetType(cfgBase);

		if (cfgType == CT_ARRAY)
		{
			TStringArray slots = new TStringArray;
			GetGame().ConfigGetTextArray(cfgBase, slots);
			foreach (string slotEntry : slots)
				AddSlotItem(slotEntry, className);
		}
		else if (cfgType == CT_STRING)
		{
			string invSlot = string.Empty;
			if (GetGame().ConfigGetText(cfgBase, invSlot) && invSlot != string.Empty)
				AddSlotItem(invSlot, className);
		}
	}

	private static void AddSlotItem(string slot, string className)
	{
		if (slot == string.Empty)
			return;

		slot.ToLower();
		TStringArray items = s_SlotItems.Get(slot);
		if (!items)
		{
			items = new TStringArray;
			s_SlotItems.Set(slot, items);
		}

		if (items.Find(className) == -1)
			items.Insert(className);
	}

	private static bool IsBaseClass(string type)
	{
		int scope = GetGame().ConfigGetInt(CFG_VEHICLESPATH + " " + type + " scope");
		if (scope == 0)
			return true;

		if (type.Length() > 5 && type.Substring(type.Length() - 5, 5) == "_Base")
			return true;

		return false;
	}
}

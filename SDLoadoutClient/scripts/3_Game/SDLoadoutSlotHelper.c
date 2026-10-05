class SDLoadoutSlotHelper
{
	static string NormalizeSlotKey(string slotName)
	{
		if (slotName == string.Empty)
			return string.Empty;

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

	static int ResolveSlotId(string slotName)
	{
		string key = NormalizeSlotKey(slotName);

		switch (key)
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

		return InventorySlots.GetSlotIdFromString(key);
	}
}

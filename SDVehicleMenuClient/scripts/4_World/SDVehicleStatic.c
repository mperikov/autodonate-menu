class SDVehicleStatic
{
	static string GetMessageFromResponse(eSDVehicleResultCode code)
	{
		switch (code)
		{
			case eSDVehicleResultCode.SUCCESS:
				return SDVehicleLocalization.T(SDVehicleLocalization.SUCCESS);
			case eSDVehicleResultCode.EMPTY_REQUEST:
				return SDVehicleLocalization.T(SDVehicleLocalization.EMPTY_REQUEST);
			case eSDVehicleResultCode.VEHICLE_404:
				return SDVehicleLocalization.T(SDVehicleLocalization.VEHICLE_404);
			case eSDVehicleResultCode.EMPTY_STORAGE:
				return SDVehicleLocalization.T(SDVehicleLocalization.NO_VEHICLES);
			case eSDVehicleResultCode.PLAYER_DEAD:
				return SDVehicleLocalization.T(SDVehicleLocalization.PLAYER_DEAD);
			case eSDVehicleResultCode.UNCONSCIOUS:
				return SDVehicleLocalization.T(SDVehicleLocalization.UNCONSCIOUS);
			case eSDVehicleResultCode.INCAR:
				return SDVehicleLocalization.T(SDVehicleLocalization.INCAR);
			case eSDVehicleResultCode.INWATER:
				return SDVehicleLocalization.T(SDVehicleLocalization.INWATER);
			case eSDVehicleResultCode.NOT_ENOUGH_SPACE:
				return SDVehicleLocalization.T(SDVehicleLocalization.NOT_ENOUGH_SPACE);
			case eSDVehicleResultCode.OUT_OF_SPAWN_ZONE:
				return SDVehicleLocalization.T(SDVehicleLocalization.OUT_OF_SPAWN_ZONE);
			case eSDVehicleResultCode.SPAWN_ZONES_NOT_CONFIGURED:
				return SDVehicleLocalization.T(SDVehicleLocalization.SPAWN_ZONES_NOT_CONFIGURED);
			case eSDVehicleResultCode.CARS_SPAWN_DISABLED:
				return SDVehicleLocalization.T(SDVehicleLocalization.CARS_SPAWN_DISABLED);
			case eSDVehicleResultCode.ALREADY_SPAWNED_THIS_RESTART:
				return SDVehicleLocalization.T(SDVehicleLocalization.ALREADY_SPAWNED);
			case eSDVehicleResultCode.INVALID_CLASS:
				return SDVehicleLocalization.T(SDVehicleLocalization.INVALID_CLASS);
		}

		return SDVehicleLocalization.T(SDVehicleLocalization.UNKNOWN_ERROR);
	}
}

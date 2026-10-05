class SDLoadoutUiHelper

{

	static bool CanOpenPostSpawnMenu()

	{

		DayZGame game = GetDayZGame();

		if (!game || !game.IsClient())

			return false;



		if (game.GetUIManager().GetMenu() != NULL)

			return false;



		MissionGameplay mission = MissionGameplay.Cast(game.GetMission());

		if (!mission)

			return false;



		return game.GetMissionState() == DayZGame.MISSION_STATE_GAME && mission.GetHud();

	}



	static void ResetFlowState()

	{

		SDLoadoutState.ResetAll();

		SDLoadoutSpawnFlow.CancelPendingUi();

	}

}


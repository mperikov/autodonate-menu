modded class MissionGameplay
{
	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (player && player.IsAlive() && GetGame().GetInput().LocalPress("SDVEHICLEOPENMENU", true))
		{
			SDVehicleMenuUI menu = GetSDVehicleMenuHandler().GetMenu();
			if (menu)
				menu.Close();
			else
				GetSDVehicleMenuHandler().OpenUI();
		}
	}
}
class SDVehicleSpawnZoneHelper
{
	static bool IsPositionInZones(vector pos, float radius, TStringArray zoneStrings)
	{
		if (!zoneStrings || zoneStrings.Count() == 0 || radius <= 0)
			return false;

		foreach (string raw : zoneStrings)
		{
			vector zone = raw.ToVector();
			zone[1] = GetGame().SurfaceY(zone[0], zone[2]);

			if (vector.Distance(pos, zone) <= radius)
				return true;
		}

		return false;
	}

	static int DistanceToNearestZone(vector pos, float radius, TStringArray zoneStrings)
	{
		if (!zoneStrings || zoneStrings.Count() == 0)
			return -1;

		int minDist = int.MAX - 1;

		foreach (string raw : zoneStrings)
		{
			vector zone = raw.ToVector();
			zone[1] = GetGame().SurfaceY(zone[0], zone[2]);
			int dist = vector.Distance(pos, zone);

			if (dist < minDist)
				minDist = dist;
		}

		if (minDist <= radius)
			return 0;

		return minDist - radius;
	}
}

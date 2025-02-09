#include "Utils.h"

namespace AdoCpp
{
	bool toBool(const rapidjson::Value& data)
	{
		if (data.IsBool())
			return data.GetBool();
		else if (data.IsString())
		{
			if (!strcmp(data.GetString(), "Enabled")) return true;
			else if (!strcmp(data.GetString(), "Disabled")) return false;
		}
		throw std::exception();
	}
	RelativeIndex toRelativeIndex(const rapidjson::Value& data)
	{
		RelativeIndex relativeIndex;
		relativeIndex.first = data[0].GetInt(), relativeIndex.second = data[1].GetString();
		return relativeIndex;
	}
	double includedAngle(double angleDeg, double nextAngleDeg)
	{
		double incAng = angleDeg + 180 - nextAngleDeg;
		while (incAng <= 0) incAng += 360;
		while (incAng > 360) incAng -= 360;
		return incAng;
	}
}
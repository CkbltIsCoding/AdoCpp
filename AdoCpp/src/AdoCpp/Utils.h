#pragma once

#include <optional>
#include <string>
#include <rapidjson/document.h>

namespace AdoCpp
{
	typedef std::pair<double, double> Point;
	typedef std::pair<std::optional<double>, std::optional<double>> OptionalPoint;
	typedef std::pair<long long, std::string> RelativeIndex;
	typedef std::string Color;

	bool to_bool(const rapidjson::Value& data);
	RelativeIndex to_RelativeIndex(const rapidjson::Value& data);
	inline double bpm2crotchet(double bpm)
	{
		return 60.0 / bpm;
	}
	inline double bpm2mspb(double bpm)
	{
		//return 60.0 / bpm * 1000.0;
		return 60000.0 / bpm;
	}
    double includedAngle(double angleDeg, double nextAngleDeg);
}
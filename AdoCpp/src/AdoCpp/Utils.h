#pragma once

#include <optional>
#include <string>
#include <rapidjson/document.h>

namespace AdoCpp
{
	typedef std::pair<double, double> Point;
	typedef std::pair<std::optional<double>, std::optional<double>> OptionalPoint;
	typedef std::pair<long long, std::string> RelativeIndex;

	/**
	 * @brief Convert json data to bool.
	 * @param data The json data.
	 * @return The bool.
	 * data == true || data == "Enabled" => true.
	 * data == false || data == "Disabled" => false.
	 * Otherwise throw an exception.
	 */
	bool toBool(const rapidjson::Value& data);
	/**
	 * @brief Convert json data to a relativeIndex.
	 * @param data The json data.
	 * @return The RelativeIndex object.
	 */
	RelativeIndex toRelativeIndex(const rapidjson::Value& data);
	/**
	 * @brief Convert bpm to spb.
	 * @param bpm The bpm.
	 * @return The spb.
	 */
	inline double bpm2crotchet(double bpm)
	{
		return 60.0 / bpm;
	}
	/**
	 * @brief Convert bpm to mspb.
	 * @param bpm The bpm.
	 * @return The mspb.
	 */
	inline double bpm2mspb(double bpm)
	{
		//return 60.0 / bpm * 1000.0;
		return 60000.0 / bpm;
	}
    /**
     * @brief Get the included angle.
     * @param angleDeg 
     * @param nextAngleDeg 
     * @return 
     */
    double includedAngle(double angleDeg, double nextAngleDeg);
}
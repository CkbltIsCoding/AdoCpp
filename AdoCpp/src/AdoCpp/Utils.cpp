#include "Utils.h"

#include <sstream>
#include <stdexcept>
#include <vector>

namespace AdoCpp
{
    bool toBool(const rapidjson::Value& data)
    {
        if (data.IsBool())
            return data.GetBool();
        if (data.IsString())
        {
            if (!strcmp(data.GetString(), "Enabled"))
                return true;
            if (!strcmp(data.GetString(), "Disabled"))
                return false;
        }
        throw std::invalid_argument(R"(data is not a boolean or string "Enabled" or string "Disabled")");
    }
    double includedAngle(const double angleDeg, const double nextAngleDeg)
    {
        double incAng = angleDeg + 180 - nextAngleDeg;
        while (incAng <= 0)
            incAng += 360;
        while (incAng > 360)
            incAng -= 360;
        return incAng;
    }
    std::vector<std::string> cstr2tags(const char* const str)
    {
        std::istringstream iss(str);
        std::string token;
        std::vector<std::string> vec;
        while (getline(iss, token, ' '))
            vec.push_back(token);
        return vec;
    }
} // namespace AdoCpp

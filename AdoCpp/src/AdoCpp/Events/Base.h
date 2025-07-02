#pragma once
#include <string>
#include <vector>

#include "rapidjson/document.h"

namespace AdoCpp::Event
{
    /**
     * @brief Event class.
     */
    class Event
    {
    public:
        Event() = default;
        virtual ~Event() = default;
        explicit Event(const rapidjson::Value& data);
        size_t floor = 0;
        virtual bool stackable() = 0;
        virtual std::string name() = 0;
        /**
         * @brief Clone the event.
         *
         * The method is used in order to make cloning events easier.
         *
         * For example, RepeatEvents use the method
         * so that it does not need to figure out the type of the event.
         * @return The cloned event.
         */
        virtual Event* clone() = 0;
    };

    /**
     * @brief StaticEvent class.
     */
    class StaticEvent : public Event
    {
    public:
        StaticEvent() = default;
        ~StaticEvent() override = default;
        explicit StaticEvent(const rapidjson::Value& data);
    };

    /**
     * @brief DynamicEvent class.
     */
    class DynamicEvent : public Event
    {
    public:
        DynamicEvent() = default;
        ~DynamicEvent() override = default;
        explicit DynamicEvent(const rapidjson::Value& data);
        DynamicEvent* clone() override = 0;
        double angleOffset = 0;
        double beat = 0;
        double seconds = 0;
        std::vector<std::string> eventTag;
        /**
         * This field is used to make deleting event pointers easier
         * @see AdoCpp::Level::clear
         * @see AdoCpp::Level::~Level
         */
        bool generated = false;
    };
} // namespace AdoCpp::Event

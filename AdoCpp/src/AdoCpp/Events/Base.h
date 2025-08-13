#pragma once
#include <memory>
#include <rapidjson/document.h>
#include <string>
#include <vector>
#include "AdoCpp/Utils.h"

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
        bool active = true;
        [[nodiscard]] constexpr virtual bool stackable() const noexcept = 0;
        [[nodiscard]] constexpr virtual const char* name() const noexcept = 0;
        /**
         * @brief Clone the event.
         *
         * The method is used in order to make cloning events easier.
         *
         * For example, RepeatEvents use the method
         * so that it does not need to figure out the type of the event.
         * @return The cloned event.
         */
        [[nodiscard]] constexpr virtual Event* clone() const = 0;
        [[nodiscard]] virtual std::unique_ptr<rapidjson::Value>
        intoJson(rapidjson::Document::AllocatorType& alloc) const = 0;
        [[nodiscard]] std::unique_ptr<rapidjson::Document> intoJson() const;
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
        [[nodiscard]] constexpr DynamicEvent* clone() const override = 0;
        double angleOffset = 0;
        double beat = 0;
        double seconds = 0;
        std::vector<std::string> eventTag;
        /**
         * This field is used to make it easier to deleting event pointers.
         * @see AdoCpp::Level::clear
         * @see AdoCpp::Level::~Level
         */
        bool generated = false;
    };
} // namespace AdoCpp::Event

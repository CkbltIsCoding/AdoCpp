#pragma once
namespace AdoCpp
{
    template<typename T>
    struct DynamicValue
    {
        /**
         * @brief The original value.
         */
        T o{};
        /**
         * @brief The current value.
         */
        T c{};
        DynamicValue() = default;
        DynamicValue(T o, T c) : o(o), c(c) {}
        explicit DynamicValue(T oc) : o(oc), c(oc) {}

        void o2c()
        {
            c = o;
        }
    };
} // AdoCpp

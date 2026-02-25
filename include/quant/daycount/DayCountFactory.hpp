#ifndef DAY_COUNT_FACTORY_HPP
#define DAY_COUNT_FACTORY_HPP

#include <memory>
#include <string>
#include "DayCountCalculator.hpp"

class DayCountFactory {
public:
    enum class Convention {
        ACT_360,
        THIRTY_360
    };

    // Solo declaramos la función, no la implementamos aquí
    static std::unique_ptr<DayCountCalculator> create(Convention type);
};

#endif
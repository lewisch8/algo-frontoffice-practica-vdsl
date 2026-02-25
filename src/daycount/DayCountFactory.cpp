#include "quant/daycount/DayCountFactory.hpp"
#include "quant/daycount/Actual_360.hpp"
#include "quant/daycount/Thirty_360.hpp"
#include <stdexcept>

using Quant::Time::Actual_360;
using Quant::Time::Thirty_360;

std::unique_ptr<DayCountCalculator> DayCountFactory::create(Convention type) {
    switch (type) {
        case Convention::ACT_360:
            return std::make_unique<Actual_360>();
        case Convention::THIRTY_360:
            return std::make_unique<Thirty_360>();
        default:
            throw std::invalid_argument("Convención no reconocida");
    }
}
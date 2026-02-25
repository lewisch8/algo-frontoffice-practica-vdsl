#ifndef ACTUAL_360_HPP
#define ACTUAL_360_HPP

#include "DayCountCalculator.hpp"
#include <algorithm>

namespace Quant::Time {
class Actual_360 : public DayCountCalculator {
public:
    // Implementación para strings (usa la de fechas internamente)
    double operator()(const std::string& start, const std::string& end) const override {
        return operator()(boost::gregorian::from_string(start), boost::gregorian::from_string(end));
    }

    // Implementación para fechas (la que pedía tu ExerciseCurve)
    double operator()(const boost::gregorian::date& start, const boost::gregorian::date& end) const override {
        return static_cast<double>((end - start).days()) / 360.0;
    }
};
}

#endif
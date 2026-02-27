#ifndef DAY_COUNT_CALCULATOR_HPP
#define DAY_COUNT_CALCULATOR_HPP

#include <boost/date_time/gregorian/gregorian.hpp>
#include <string>

class DayCountCalculator {
public:
    virtual ~DayCountCalculator() = default;

    virtual double operator()(const std::string& start, const std::string& end) const = 0;
    virtual double operator()(const boost::gregorian::date& start, const boost::gregorian::date& end) const = 0;
};

#endif
#ifndef SCHEDULE_HPP
#define SCHEDULE_HPP

#include <vector>
#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "Calendar.hpp"
#include "quant/daycount/DayCountCalculator.hpp"

namespace Quant::Time {

class Schedule {
private:
    boost::gregorian::date start_date_;
    std::vector<boost::gregorian::date> dates_;
    std::vector<double> year_fractions_;

public:
    Schedule(): start_date_(boost::gregorian::not_a_date_time), 
          dates_(), 
          year_fractions_() 
    {}

    Schedule(const std::string& start_date_str, double years_total, int frequency, DayCountCalculator& calc)
        : start_date_(boost::gregorian::not_a_date_time), 
          dates_(), 
          year_fractions_() 
    {
        generate(start_date_str, years_total, frequency);
        calculate_fractions(calc);
    }

    // Genera las fechas del cronograma
    void generate(const std::string& start_date_str, double years_total, int frequency) {
        start_date_ = boost::gregorian::from_string(start_date_str);
        dates_.clear();
        
        int months_step = 12 / frequency;
        int total_periods = static_cast<int>(years_total * frequency);

        for (int i = 1; i <= total_periods; ++i) {
            auto theoretical = start_date_ + boost::gregorian::months(i * months_step);
            auto adjusted = Calendar::Calendar::first_business_day(theoretical.year(), theoretical.month());
            dates_.push_back(adjusted);
        }
    }

    // Calcula las fracciones de año usando las fechas internas
    void calculate_fractions(DayCountCalculator& calculator) {
        year_fractions_.clear();
        year_fractions_.reserve(dates_.size());

        boost::gregorian::date previous = start_date_;
        for (const auto& current : dates_) {
            double yf = calculator(
                boost::gregorian::to_iso_extended_string(previous),
                boost::gregorian::to_iso_extended_string(current)
            );
            year_fractions_.push_back(yf);
            previous = current;
        }
    }

    // Getters para acceder a los datos
    const std::vector<boost::gregorian::date>& get_dates() const { return dates_; }
    const std::vector<double>& get_year_fractions() const { return year_fractions_; }
    const boost::gregorian::date& get_start_date() const { return start_date_; }
};

} // namespace Quant::Time

#endif
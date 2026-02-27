// include/quant/calendar/Calendar.hpp
#ifndef CALENDAR_HPP
#define CALENDAR_HPP

#include <boost/date_time/gregorian/gregorian.hpp>

namespace Quant::Calendar {
    using date = boost::gregorian::date;
    using day_iterator = boost::gregorian::day_iterator;

    class Calendar {
    public:
        static date first_business_day(unsigned short year, unsigned short month) {
            date d(year, month, 1);
            
            // Si es Sábado (6) o Domingo (0), avanzamos
            while (is_weekend(d)) {
                d += boost::gregorian::days(1);
            }
            return d;
        }

        static boost::gregorian::date first_business_day(const std::string& date_str) {
        boost::gregorian::date d = boost::gregorian::from_string(date_str);
        
        return first_business_day(d.year(), d.month());
    }

        static bool is_weekend(const date& d) {
            auto day = d.day_of_week();
            return (day == boost::date_time::Saturday || day == boost::date_time::Sunday);
        }
    };
}

#endif
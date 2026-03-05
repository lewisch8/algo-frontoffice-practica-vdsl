#ifndef CALENDAR_HPP
#define CALENDAR_HPP

#include <boost/date_time/gregorian/gregorian.hpp>

namespace Quant::Calendar {
    using date = boost::gregorian::date;

    class Calendar {
    public:
        // Verifica si es fin de semana (sábado o domingo)
        static bool is_weekend(const date& d) {
            auto day = d.day_of_week();
            return (day == boost::date_time::Saturday || day == boost::date_time::Sunday);
        }

        // Convención FOLLOWING: Mueve al siguiente día hábil
        static date adjust_following(date d) {
            while (is_weekend(d)) {
                d += boost::gregorian::days(1);
            }
            return d;
        }

        // Convención MODIFIED FOLLOWING: Siguiente día hábil, a menos que cambie de mes
        static date adjust_modified_following(date d) {
            date adjusted = adjust_following(d);
            
            // Si al avanzar se cambia de mes, se aplica retroceso
            if (adjusted.month() != d.month()) {
                adjusted = d;
                while (is_weekend(adjusted)) {
                    adjusted -= boost::gregorian::days(1);
                }
            }
            return adjusted;
        }

        // Convención MODIFIED FOLLOWING: para anho, mes, dia
        static date adjust_modified_following(unsigned short year, unsigned short month, unsigned short day) {
            date d(year, month, day);

            date adjusted = adjust_following(d);
            
            // Si al avanzar se cambia de mes, se aplica retroceso
            if (adjusted.month() != d.month()) {
                adjusted = d;
                while (is_weekend(adjusted)) {
                    adjusted -= boost::gregorian::days(1);
                }
            }
            return adjusted;
        }

        // Convención MODIFIED FOLLOWING: para anho, mes, dia en formato string
        static boost::gregorian::date adjust_modified_following(const std::string& date_str) {
            boost::gregorian::date d = boost::gregorian::from_string(date_str);
            date adjusted = adjust_following(d);
            
            // Si al avanzar se cambia de mes, se aplica retroceso
            if (adjusted.month() != d.month()) {
                adjusted = d;
                while (is_weekend(adjusted)) {
                    adjusted -= boost::gregorian::days(1);
                }
            }
            return adjusted;
        }
    };
}

#endif
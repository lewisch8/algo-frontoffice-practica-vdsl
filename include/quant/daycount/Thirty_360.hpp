#ifndef THIRTY_360_HPP
#define THIRTY_360_HPP

#include "DayCountCalculator.hpp"
#include <algorithm>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace Quant::Time {

class Thirty_360 : public DayCountCalculator {
public:
    // Mantenemos tus métodos estáticos para lógica interna
    static short compute_daycount(const std::string& from, const std::string& to);
    static short compute_daycount(const boost::gregorian::date& from, const boost::gregorian::date& to);
    static short compute_daycount(short years, short months, short days_from, short days_to);

    // 1. Implementación para Strings (contrato de la base)
    double operator()(const std::string& start, const std::string& end) const override {
        return compute_daycount(start, end) / 360.0;
    }

    // 2. Implementación para Fechas de Boost (contrato de la base)
    // Esta es la que llamará tu ExerciseCurve
    double operator()(const boost::gregorian::date& start, const boost::gregorian::date& end) const override {
        return compute_daycount(start, end) / 360.0;
    }
};

// --- IMPLEMENTACIÓN DE LA LÓGICA MATEMÁTICA ---

inline short Thirty_360::compute_daycount(short years, short months, short days_from, short days_to) {
    // Aplicando la lógica 30/360:
    // Days = (Y2 - Y1) * 360 + (M2 - M1) * 30 + (D2 - D1)
    // Tu implementación usa un ajuste manual de límites:
    return static_cast<short>((360 * years) + 30 * (months - 1) + 
           std::max(0, 30 - (int)days_from) + std::min(30, (int)days_to));
}

inline short Thirty_360::compute_daycount(const boost::gregorian::date& from, const boost::gregorian::date& to) {
    auto d1 = from.year_month_day();
    auto d2 = to.year_month_day();
    
    return compute_daycount(
        static_cast<short>(d2.year - d1.year),
        static_cast<short>(d2.month - d1.month),
        static_cast<short>(d1.day),
        static_cast<short>(d2.day)
    );
}

inline short Thirty_360::compute_daycount(const std::string& from, const std::string& to) {
    // Reutilizamos la lógica de Boost para parsing
    return compute_daycount(boost::gregorian::from_string(from), 
                           boost::gregorian::from_string(to));
}

} // namespace Quant::Time

#endif
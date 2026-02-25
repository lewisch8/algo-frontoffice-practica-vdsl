#ifndef LEG_HPP
#define LEG_HPP

#include <memory>
#include "../time/Schedule.hpp"
#include "../market/ZeroCouponCurve.hpp"

namespace Quant::Instruments {

struct CashFlow {
    boost::gregorian::date date;
    double year_fraction;
    double rate;       // Tasa aplicada (fija o proyectada)
    double amount;     // Cupón sin descontar
    double discount;   // Factor de descuento (ZC)
    double pv;         // Valor presente del cupón
};

class Leg {
protected:
    Time::Schedule schedule_; // Integra tu lógica de fechas previa
    double notional_;

public:
    Leg(Time::Schedule s, double notional)
        : schedule_(std::move(s)), notional_(notional) {}

    virtual ~Leg() = default;

    // Cada pata calcula su propio valor presente
    virtual double price(const Market::ZeroCouponCurve& curve) const = 0;

    /**
     * @brief Permite que el Swap consulte el cronograma de la pata.
     */
    const Time::Schedule& get_schedule() const { return schedule_; }

    /**
     * @brief Permite que el Swap consulte el monto principal.
     */
    double get_notional() const { return notional_; }

    virtual std::vector<CashFlow> get_cashflows(const Market::ZeroCouponCurve& curve) const = 0;
};

} // namespace Quant::Instruments
#endif
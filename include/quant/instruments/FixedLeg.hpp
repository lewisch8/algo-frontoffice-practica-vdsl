#ifndef FIXED_LEG_HPP
#define FIXED_LEG_HPP

#include "Leg.hpp"

namespace Quant::Instruments {

class FixedLeg : public Leg {
private:
    double fixed_rate_;

public:
    FixedLeg(Time::Schedule s, double notional, double rate)
        : Leg(std::move(s), notional), fixed_rate_(rate) {}

    double price(const Market::ZeroCouponCurve& curve) const override {
        double pv = 0.0;
        const auto& dates = schedule_.get_dates();
        const auto& yfs = schedule_.get_year_fractions();

        for (size_t i = 0; i < dates.size(); ++i) {
            double coupon = notional_ * fixed_rate_ * yfs[i];
            pv += coupon * curve.get_zc(dates[i]);
        }
        return pv;
    }

    std::vector<CashFlow> get_cashflows(const Market::ZeroCouponCurve& curve) const override {
        std::vector<CashFlow> results;
        const auto& dates = schedule_.get_dates();
        const auto& yfs = schedule_.get_year_fractions();

        for (size_t i = 0; i < dates.size(); ++i) {
            // Cupón fijo: N * R * Alpha
            double amount = notional_ * fixed_rate_ * yfs[i];
            double zc = curve.get_zc(dates[i]);
            
            results.push_back({
                dates[i],
                yfs[i],
                fixed_rate_,
                amount,
                zc,
                amount * zc
            });
        }
        return results;
    }
};

} // namespace Quant::Instruments
#endif
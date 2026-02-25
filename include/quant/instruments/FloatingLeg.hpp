#ifndef FLOATING_LEG_HPP
#define FLOATING_LEG_HPP

#include "Leg.hpp"
#include "../market/Index.hpp"
#include <vector>

namespace Quant::Instruments {

class FloatingLeg : public Leg {
private:
    std::shared_ptr<Market::Index> index_;
    double spread_;

public:
    FloatingLeg(Time::Schedule s, double notional, std::shared_ptr<Market::Index> idx, double spread = 0.0)
        : Leg(std::move(s), notional), index_(std::move(idx)), spread_(spread) {}

    // Unificado: price() ahora también proyecta Forwards
    double price(const Market::ZeroCouponCurve& curve) const override {
        double pv = 0.0;
        const auto& dates = schedule_.get_dates();
        const auto& yfs = schedule_.get_year_fractions();

        for (size_t i = 0; i < dates.size(); ++i) {
            boost::gregorian::date start = (i == 0) ? schedule_.get_start_date() : dates[i-1];
            boost::gregorian::date end = dates[i];

            double fwd_rate = index_->get_rate(start, end, yfs[i]);
            double coupon = notional_ * (fwd_rate + spread_) * yfs[i];
            pv += coupon * curve.get_zc(end);
        }
        return pv;
    }

    std::vector<CashFlow> get_cashflows(const Market::ZeroCouponCurve& curve) const override {
        std::vector<CashFlow> results;
        const auto& dates = schedule_.get_dates();
        const auto& yfs = schedule_.get_year_fractions();

        for (size_t i = 0; i < dates.size(); ++i) {
            boost::gregorian::date start = (i == 0) ? schedule_.get_start_date() : dates[i-1];
            boost::gregorian::date end = dates[i];

            // Proyección real usando la curva ZC
            double fwd_rate = index_->get_rate(start, end, yfs[i]); 
            double coupon_amount = notional_ * (fwd_rate + spread_) * yfs[i];
            double zc_factor = curve.get_zc(end);
            
            results.push_back({
                end, yfs[i], fwd_rate + spread_,
                coupon_amount, zc_factor, coupon_amount * zc_factor
            });
        }
        return results;
    }
};

} // namespace Quant::Instruments
#endif
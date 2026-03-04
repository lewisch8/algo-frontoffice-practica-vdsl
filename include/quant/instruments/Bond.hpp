#ifndef BOND_HPP
#define BOND_HPP

#include "Instrument.hpp"
#include "Leg.hpp"

namespace Quant::Instruments {

class Bond : public Instrument {
private:
    std::unique_ptr<Leg> coupons_;

public:
    Bond(std::shared_ptr<Market::ZeroCouponCurve> c, std::unique_ptr<Leg> leg)
        : Instrument(std::move(c)), coupons_(std::move(leg)) {}

    double price() const override {
        // Valor Presente de los cupones
        double pv_coupons = coupons_->price(*curve_);
        
        // Valor Presente del Principal (Nocional) en la última fecha
        const auto& dates = coupons_->get_schedule().get_dates();
        if (dates.empty()) return pv_coupons;
        
        boost::gregorian::date maturity = dates.back();
        double notional = coupons_->get_notional();
        
        double pv_principal = notional * curve_->get_zc(maturity);
        
        // Cupones + Principal
        return pv_coupons + pv_principal;
    }
};

} // namespace Quant::Instruments
#endif
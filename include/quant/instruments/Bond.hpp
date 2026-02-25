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
        return coupons_->price(*curve_);
    }
};

} // namespace Quant::Instruments
#endif
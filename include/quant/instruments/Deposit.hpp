#ifndef DEPOSIT_HPP
#define DEPOSIT_HPP

#include "Instrument.hpp"

namespace Quant::Instruments {

class Deposit : public Instrument {
public:
    Deposit(std::shared_ptr<Market::ZeroCouponCurve> curve, double rate, double maturity) 
        : Instrument(std::move(curve)), rate_(rate), maturity_(maturity) {}

    double getRate() const { return rate_; }
    double getMaturity() const { return maturity_; }

    double price() const override {
        double total_amount = 1.0 * (1.0 + rate_ * maturity_);
        return total_amount;
    }

private:
    double rate_;
    double maturity_; // Expresado en años
};

} // namespace Quant::Instruments
#endif
#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

#include <memory>
#include "../market/ZeroCouponCurve.hpp"

namespace Quant::Instruments {

class Instrument {
protected:
    std::shared_ptr<Market::ZeroCouponCurve> curve_;

public:
    explicit Instrument(std::shared_ptr<Market::ZeroCouponCurve> c) 
        : curve_(std::move(c)) {}

    virtual ~Instrument() = default;

    virtual double price() const = 0;
};

} // namespace Quant::Instruments
#endif
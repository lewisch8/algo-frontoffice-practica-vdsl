#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP

#include <memory>
#include "../market/ZeroCouponCurve.hpp"

namespace Quant::Instruments {

class Instrument {
protected:
    // Todo instrumento necesita una curva para descontar flujos
    std::shared_ptr<Market::ZeroCouponCurve> curve_;

public:
    explicit Instrument(std::shared_ptr<Market::ZeroCouponCurve> c) 
        : curve_(std::move(c)) {}

    virtual ~Instrument() = default;

    // El método central de tu arquitectura
    virtual double price() const = 0;
};

} // namespace Quant::Instruments
#endif
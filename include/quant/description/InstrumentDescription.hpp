#ifndef INSTRUMENT_DESCRIPTION_HPP
#define INSTRUMENT_DESCRIPTION_HPP

#include "LegDescription.hpp"
#include "../market/ZeroCouponCurve.hpp"
#include <memory>
#include <string>

namespace Quant::Description {

struct InstrumentDescription {
    enum class Type { bond, swap };
    
    Type type;
    LegDescription payer;
    LegDescription receiver;
    std::shared_ptr<Market::ZeroCouponCurve> discount_curve;

    // Constructor para Swap (2 patas)
    InstrumentDescription(Type t, LegDescription p, LegDescription r, std::shared_ptr<Market::ZeroCouponCurve> curve)
        : type(t), 
          payer(std::move(p)), 
          receiver(std::move(r)), 
          discount_curve(std::move(curve)) 
    {}

    // Constructor para Bond (pata payer queda por defecto)
    InstrumentDescription(Type t, LegDescription r, std::shared_ptr<Market::ZeroCouponCurve> curve)
        : type(t), 
          payer(), // Inicializa con el constructor por defecto de LegDescription
          receiver(std::move(r)), 
          discount_curve(std::move(curve)) 
    {}
};

} // namespace Quant::Description

#endif
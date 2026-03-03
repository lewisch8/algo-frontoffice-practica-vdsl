#ifndef INSTRUMENT_DESCRIPTION_HPP
#define INSTRUMENT_DESCRIPTION_HPP

#include "LegDescription.hpp"
#include "../market/ZeroCouponCurve.hpp"
#include <memory>
#include <string>
#include <vector>

namespace Quant::Description {

struct InstrumentDescription {
    enum class Type { bond, swap };
    
    Type type;
    std::vector<LegDescription> legs;
    std::shared_ptr<Market::ZeroCouponCurve> discount_curve;

    // Constructor para Swap (2 patas: [0] payer, [1] receiver)
    InstrumentDescription(Type t, LegDescription p, LegDescription r, std::shared_ptr<Market::ZeroCouponCurve> curve)
        : type(t), legs{std::move(p), std::move(r)}, discount_curve(std::move(curve)) {}

    // Constructor genérico / para Bond (1 pata)
    InstrumentDescription(Type t, LegDescription leg, std::shared_ptr<Market::ZeroCouponCurve> curve)
        : type(t), legs{std::move(leg)}, discount_curve(std::move(curve)) {}
};

} // namespace Quant::Description

#endif
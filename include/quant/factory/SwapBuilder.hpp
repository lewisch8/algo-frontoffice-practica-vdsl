#ifndef SWAP_BUILDER_HPP
#define SWAP_BUILDER_HPP

#include "FactoryRegistrator.hpp"
#include "../instruments/Swap.hpp"
#include "../instruments/FixedLeg.hpp"
#include "../instruments/FloatingLeg.hpp"
#include "../daycount/DayCountFactory.hpp"

namespace Quant::Factory {

class SwapBuilder {
public:
    static std::unique_ptr<Instruments::Instrument> build(const Description::InstrumentDescription& desc);
    static Description::InstrumentDescription::Type getId() { 
        return Description::InstrumentDescription::Type::swap; 
    }
};

// Variable estática para disparar el registro automático
namespace {
    FactoryRegistrator<SwapBuilder> registration;
}

} // namespace Quant::Factory
#endif
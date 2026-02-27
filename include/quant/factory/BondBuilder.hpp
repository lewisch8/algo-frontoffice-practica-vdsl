#ifndef BOND_BUILDER_HPP
#define BOND_BUILDER_HPP

#include "FactoryRegistrator.hpp"
#include "../instruments/Bond.hpp"
#include "../instruments/FixedLeg.hpp"
#include "../instruments/FloatingLeg.hpp"
#include "../daycount/DayCountFactory.hpp"

namespace Quant::Factory {

class BondBuilder {
public:
    static std::unique_ptr<Instruments::Instrument> build(const Description::InstrumentDescription& desc);
    
    static Description::InstrumentDescription::Type getId() { 
        return Description::InstrumentDescription::Type::bond; 
    }
};

// Registro automático en la Factory
namespace {
    FactoryRegistrator<BondBuilder> bond_registration;
}

} // namespace Quant::Factory
#endif
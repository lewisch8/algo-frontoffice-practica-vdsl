#include "../../include/quant/factory/BondBuilder.hpp"
#include "../../include/quant/daycount/DayCountFactory.hpp"
#include <stdexcept>

namespace Quant::Factory {

std::unique_ptr<Instruments::Instrument> BondBuilder::build(const Description::InstrumentDescription& desc) {
    auto curve = desc.discount_curve;
    
    if (!curve) {
        throw std::runtime_error("Error: La descripción del bono no contiene una curva válida.");
    }

    const auto& leg_desc = desc.receiver;

    // Generacion del cronograma
    Time::Schedule sched;
    sched.generate(leg_desc.start_date, leg_desc.years, leg_desc.frequency);
    
    auto convention = DayCountFactory::get_convention(desc.receiver.day_count);
    auto dc = DayCountFactory::create(convention);
    sched.calculate_fractions(*dc);

    // Construccion de la pata
    std::unique_ptr<Instruments::Leg> leg;
    if (leg_desc.type == Description::LegDescription::LegType::Fixed) {
        leg = std::make_unique<Instruments::FixedLeg>(
            std::move(sched), leg_desc.notional, leg_desc.rate_or_spread
        );
    } else {
        leg = std::make_unique<Instruments::FloatingLeg>(
            std::move(sched), leg_desc.notional, leg_desc.index_obj, leg_desc.rate_or_spread
        );
    }

    // Retornar el Bono
    return std::make_unique<Instruments::Bond>(curve, std::move(leg));
}

} // namespace Quant::Factory
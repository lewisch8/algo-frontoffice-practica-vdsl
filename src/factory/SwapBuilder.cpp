#include "../../include/quant/factory/SwapBuilder.hpp"
#include "../../include/quant/daycount/DayCountFactory.hpp"
#include "../../include/quant/instruments/FixedLeg.hpp"
#include "../../include/quant/instruments/FloatingLeg.hpp"

namespace Quant::Factory {

std::unique_ptr<Instruments::Instrument> SwapBuilder::build(const Description::InstrumentDescription& desc) {
    auto curve = desc.discount_curve;

    // --- PATA PAGADORA ---
    Time::Schedule payer_sched;
    payer_sched.generate(desc.payer.start_date, desc.payer.years, desc.payer.frequency);
    auto convention = DayCountFactory::get_convention(desc.payer.day_count);
    auto dc_payer = DayCountFactory::create(convention);
    payer_sched.calculate_fractions(*dc_payer);

    std::unique_ptr<Instruments::Leg> payer_leg;
    if (desc.payer.type == Description::LegDescription::LegType::Fixed) {
        payer_leg = std::make_unique<Instruments::FixedLeg>(std::move(payer_sched), desc.payer.notional, desc.payer.rate_or_spread);
    } else {
        payer_leg = std::make_unique<Instruments::FloatingLeg>(std::move(payer_sched), desc.payer.notional, desc.payer.index_obj, desc.payer.rate_or_spread);
    }

    // --- PATA RECEPTORA ---
    Time::Schedule receiver_sched;
    receiver_sched.generate(desc.receiver.start_date, desc.receiver.years, desc.receiver.frequency);
    auto dc_receiver = DayCountFactory::create(convention);
    receiver_sched.calculate_fractions(*dc_receiver);

    std::unique_ptr<Instruments::Leg> receiver_leg;
    if (desc.receiver.type == Description::LegDescription::LegType::Fixed) {
        receiver_leg = std::make_unique<Instruments::FixedLeg>(std::move(receiver_sched), desc.receiver.notional, desc.receiver.rate_or_spread);
    } else {
        receiver_leg = std::make_unique<Instruments::FloatingLeg>(std::move(receiver_sched), desc.receiver.notional, desc.receiver.index_obj, desc.receiver.rate_or_spread);
    }

    return std::make_unique<Instruments::Swap>(curve, std::move(payer_leg), std::move(receiver_leg));
}

} // namespace Quant::Factory
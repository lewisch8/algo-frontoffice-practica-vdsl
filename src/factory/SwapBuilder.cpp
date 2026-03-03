#include "../../include/quant/factory/SwapBuilder.hpp"
#include "../../include/quant/daycount/DayCountFactory.hpp"
#include "../../include/quant/instruments/FixedLeg.hpp"
#include "../../include/quant/instruments/FloatingLeg.hpp"

namespace Quant::Factory {

std::unique_ptr<Instruments::Instrument> SwapBuilder::build(const Description::InstrumentDescription& desc) {
    auto curve = desc.discount_curve;

    if (desc.legs.size() < 2) {
        throw std::runtime_error("Error: El Swap requiere exactamente 2 patas.");
    }

    const auto& payer_desc = desc.legs[0];
    const auto& receiver_desc = desc.legs[1];

    // --- PATA PAGADORA ---
    Time::Schedule payer_sched;
    payer_sched.generate(payer_desc.start_date, payer_desc.years, payer_desc.frequency);
    auto convention_payer = DayCountFactory::get_convention(payer_desc.day_count);
    auto dc_payer = DayCountFactory::create(convention_payer);
    payer_sched.calculate_fractions(*dc_payer);

    std::unique_ptr<Instruments::Leg> payer_leg;
    if (payer_desc.type == Description::LegDescription::LegType::Fixed) {
        payer_leg = std::make_unique<Instruments::FixedLeg>(std::move(payer_sched), payer_desc.notional, payer_desc.rate_or_spread);
    } else {
        payer_leg = std::make_unique<Instruments::FloatingLeg>(std::move(payer_sched), payer_desc.notional, payer_desc.index_obj, payer_desc.rate_or_spread);
    }

    // --- PATA RECEPTORA ---
    Time::Schedule receiver_sched;
    receiver_sched.generate(receiver_desc.start_date, receiver_desc.years, receiver_desc.frequency);
    auto convention_receiver = DayCountFactory::get_convention(receiver_desc.day_count);
    auto dc_receiver = DayCountFactory::create(convention_receiver);
    receiver_sched.calculate_fractions(*dc_receiver);

    std::unique_ptr<Instruments::Leg> receiver_leg;
    if (receiver_desc.type == Description::LegDescription::LegType::Fixed) {
        receiver_leg = std::make_unique<Instruments::FixedLeg>(std::move(receiver_sched), receiver_desc.notional, receiver_desc.rate_or_spread);
    } else {
        receiver_leg = std::make_unique<Instruments::FloatingLeg>(std::move(receiver_sched), receiver_desc.notional, receiver_desc.index_obj, receiver_desc.rate_or_spread);
    }

    return std::make_unique<Instruments::Swap>(curve, std::move(payer_leg), std::move(receiver_leg));
}

} // namespace Quant::Factory
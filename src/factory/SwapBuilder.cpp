#include "../../include/quant/factory/SwapBuilder.hpp"
#include "../../include/quant/market/ExerciseCurve.hpp"
#include "../../include/quant/daycount/DayCountFactory.hpp"
#include "../../include/quant/instruments/FixedLeg.hpp"
#include "../../include/quant/instruments/FloatingLeg.hpp"

namespace Quant::Factory {

// Clase temporal interna para el Euribor del ejercicio
class SimpleIndex : public Market::Index {
public:
    SimpleIndex(std::shared_ptr<Market::ZeroCouponCurve> c) 
        : Market::Index("EURIBOR_6M", c) {}

    double get_rate(const boost::gregorian::date& d1, const boost::gregorian::date& d2, double yf) const override {
        // Si la fecha de inicio es la fecha de hoy (1 de Abril), usamos el 4.8%
        if (d1 == boost::gregorian::date(2016, 4, 1)) {
            return 0.048; 
        }
        // Para cualquier otra fecha futura, usamos la fórmula matemática de la base
        return Index::get_rate(d1, d2, yf);
    }
};

std::unique_ptr<Instruments::Instrument> SwapBuilder::build(const Description::InstrumentDescription& desc) {
    // 1. Curva de Mercado compartida
    auto curve = std::make_shared<Market::ExerciseCurve>(
        boost::gregorian::from_string("2016-04-01")
    );

    // --- BLOQUE 1: CONSTRUCCIÓN DE LA PATA PAGADORA (PAYER) ---
    Time::Schedule payer_sched;
    payer_sched.generate(desc.payer.start_date, desc.payer.years, desc.payer.frequency);
    auto dc_payer = DayCountFactory::create(DayCountFactory::Convention::ACT_360);
    payer_sched.calculate_fractions(*dc_payer);

    std::unique_ptr<Instruments::Leg> payer_leg;
    if (desc.payer.type == Description::LegDescription::LegType::Fixed) {
        payer_leg = std::make_unique<Instruments::FixedLeg>(
            std::move(payer_sched), desc.payer.notional, desc.payer.rate_or_spread
        );
    } else {
        auto idx = std::make_shared<SimpleIndex>(curve);
        payer_leg = std::make_unique<Instruments::FloatingLeg>(
            std::move(payer_sched), desc.payer.notional, idx, desc.payer.rate_or_spread
        );
    }

    // --- BLOQUE 2: CONSTRUCCIÓN DE LA PATA RECEPTORA (RECEIVER) ---
    // Repetimos la lógica para solucionar el error de declaración
    Time::Schedule receiver_sched;
    receiver_sched.generate(desc.receiver.start_date, desc.receiver.years, desc.receiver.frequency);
    auto dc_receiver = DayCountFactory::create(DayCountFactory::Convention::ACT_360);
    receiver_sched.calculate_fractions(*dc_receiver);

    std::unique_ptr<Instruments::Leg> receiver_leg;
    if (desc.receiver.type == Description::LegDescription::LegType::Fixed) {
        receiver_leg = std::make_unique<Instruments::FixedLeg>(
            std::move(receiver_sched), desc.receiver.notional, desc.receiver.rate_or_spread
        );
    } else {
        // En tu ejercicio, esta es la pata Euribor 6M
        auto idx = std::make_shared<SimpleIndex>(curve);
        receiver_leg = std::make_unique<Instruments::FloatingLeg>(
            std::move(receiver_sched), desc.receiver.notional, idx, desc.receiver.rate_or_spread
        );
    }

    // 4. ENSAMBLAJE FINAL
    // Ahora ambas variables existen en este scope y se pueden mover al Swap
    return std::make_unique<Instruments::Swap>(curve, std::move(payer_leg), std::move(receiver_leg));
}

} // namespace Quant::Factory
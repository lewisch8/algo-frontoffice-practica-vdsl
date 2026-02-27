#ifndef SWAP_HPP
#define SWAP_HPP

#include "Instrument.hpp"
#include "Leg.hpp"

namespace Quant::Instruments {

class Swap : public Instrument {
private:
    std::unique_ptr<Leg> payer_leg_;
    std::unique_ptr<Leg> receiver_leg_;

public:
    Swap(std::shared_ptr<Market::ZeroCouponCurve> c, 
         std::unique_ptr<Leg> payer, 
         std::unique_ptr<Leg> receiver)
        : Instrument(std::move(c)), 
          payer_leg_(std::move(payer)), 
          receiver_leg_(std::move(receiver)) {}

    const Leg& get_payer_leg() const { return *payer_leg_; }
    const Leg& get_receiver_leg() const { return *receiver_leg_; }

    double price() const override {
        // El precio del Swap es la diferencia de sus patas descontadas
        return receiver_leg_->price(*curve_) - payer_leg_->price(*curve_);
    }

    double calculate_par_rate() const {
        double pv_floating = receiver_leg_->price(*curve_);
        
        // Necesitamos la suma de (alpha * ZC) de la pata fija
        const auto& dates = payer_leg_->get_schedule().get_dates();
        const auto& yfs = payer_leg_->get_schedule().get_year_fractions();
        
        double annuity = 0.0;
        for (size_t i = 0; i < dates.size(); ++i) {
            annuity += yfs[i] * curve_->get_zc(dates[i]);
        }

        // Tasa Par = PV Variable / (Nocional * Annuity)
        // Asumiendo que el nocional es el mismo para ambas patas
        return pv_floating / (payer_leg_->get_notional() * annuity);
    }
};

} // namespace Quant::Instruments
#endif
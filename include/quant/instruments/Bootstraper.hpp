#ifndef BOOTSTRAPER_HPP
#define BOOTSTRAPER_HPP

#include <vector>
#include <memory>
#include <stdexcept>
#include "Deposit.hpp"
#include "Swap.hpp"
#include "FixedLeg.hpp"

namespace Quant::Instruments {

    struct DiscountFactor {
        double time;  // En años
        double value; // Z(t)
    };

    class Bootstrapper {
    public:
        static std::vector<DiscountFactor> bootstrapCurve(
            const Deposit& deposit, 
            const std::vector<std::shared_ptr<Swap>>& swaps) 
        {
            std::vector<DiscountFactor> curve;

            // Bootstrapping del Depósito
            double t_dep = deposit.getMaturity(); 
            double r_dep = deposit.getRate();
            double z_dep = 1.0 / (1.0 + r_dep * t_dep);
            
            curve.push_back({t_dep, z_dep});

            // Sumatoria de Z_i * dt_i para la anualidad.
            double sum_Z_dt = z_dep * t_dep; 
            double current_time = t_dep;

            // Bootstrapping de los Swaps
            for (const auto& swap_ptr : swaps) {
                const Swap& swap = *swap_ptr;
                
                const FixedLeg& fixed_leg = swap.get_fixed_leg();

                double r_swap = fixed_leg.get_fixed_rate(); 
                const auto& yfs = fixed_leg.get_schedule().get_year_fractions();
                
                double dt_N = yfs.back();
                current_time += dt_N;

                // Despejar Z_N de la fórmula de Swap a la par
                double z_N = (1.0 - r_swap * sum_Z_dt) / (1.0 + r_swap * dt_N);
                
                curve.push_back({current_time, z_N});
                
                // Actualizar la sumatoria de la anualidad para el siguiente Swap
                sum_Z_dt += z_N * dt_N;
            }

            return curve;
        }
    };
} 
#endif
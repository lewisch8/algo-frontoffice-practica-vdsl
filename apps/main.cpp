#include <iostream>
#include <iomanip>
#include "quant/factory/Factory.hpp"
#include "quant/factory/SwapBuilder.hpp"
#include "quant/market/ExerciseCurve.hpp"

using namespace Quant;

// Función auxiliar para imprimir tablas de flujos de caja
void print_leg_report(const std::string& title, const std::vector<Instruments::CashFlow>& flows) {
    std::cout << "\n>>> " << title << " <<<" << std::endl;
    std::cout << std::left << std::setw(12) << "Fecha" 
              << std::setw(10) << "YF" 
              << std::setw(10) << "Tasa" 
              << std::setw(15) << "Cupón" 
              << std::setw(12) << "Factor ZC" 
              << "PV" << std::endl;
    std::cout << std::string(75, '-') << std::endl;

    for (const auto& cf : flows) {
        std::cout << std::left << std::setw(12) << boost::gregorian::to_iso_extended_string(cf.date)
                  << std::fixed << std::setprecision(6) << std::setw(10) << cf.year_fraction
                  << std::setprecision(4) << std::setw(10) << cf.rate
                  << std::setprecision(2) << std::setw(15) << cf.amount
                  << std::setprecision(6) << std::setw(12) << cf.discount
                  << std::setprecision(2) << cf.pv << std::endl;
    }
}

int main() {
    try {
        // 1. Configuración de Mercado (1 de Abril de 2016)
        auto curve = std::make_shared<Market::ExerciseCurve>(boost::gregorian::from_string("2016-04-01"));

        // 2. Definición del Instrumento (Plano de Construcción)
        Description::LegDescription fixed_p(Description::LegDescription::LegType::Fixed, "2016-04-01", 2, 2, 100e6, 0.05, "", "ACT_360");
        Description::LegDescription float_r(Description::LegDescription::LegType::Floating, "2016-04-01", 2, 2, 100e6, 0.0, "EURIBOR_6M", "ACT_360");
        Description::InstrumentDescription desc(Description::InstrumentDescription::Type::swap, fixed_p, float_r, "Market_2016");

        // 3. Creación y Valoración
        auto irs = Factory::Factory::instance()(desc);
        auto* swap = dynamic_cast<Instruments::Swap*>(irs.get());

        if (swap) {
            // Cálculo de métricas principales
            double pv_payer = swap->get_payer_leg().price(*curve);
            double pv_receiver = swap->get_receiver_leg().price(*curve);
            double npv = swap->price();
            double par_rate = swap->calculate_par_rate();

            // --- BLOQUE DE RESULTADOS TOTALES ---
            std::cout << std::fixed << std::setprecision(2);
            std::cout << "====================================================" << std::endl;
            std::cout << "       INFORME DE VALORACIÓN: SWAP 100M EUR         " << std::endl;
            std::cout << "====================================================" << std::endl;
            std::cout << "PV Pata Pagadora (Fija):     " << std::setw(15) << pv_payer << " EUR" << std::endl;
            std::cout << "PV Pata Receptora (Var):     " << std::setw(15) << pv_receiver << " EUR" << std::endl;
            std::cout << "----------------------------------------------------" << std::endl;
            std::cout << "VALOR NETO (NPV):            " << std::setw(15) << npv << " EUR" << std::endl;
            std::cout << "TASA SWAP (PAR RATE):        " << std::setw(15) << par_rate * 100.0 << " %" << std::endl;
            std::cout << "====================================================" << std::endl;

            // --- BLOQUE DE FLUJOS DETALLADOS ---
            print_leg_report("DETALLE PATA FIJA (PAGADORA)", swap->get_payer_leg().get_cashflows(*curve));
            print_leg_report("DETALLE PATA VARIABLE (RECEPTORA)", swap->get_receiver_leg().get_cashflows(*curve));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error Crítico: " << e.what() << std::endl;
    }
    return 0;
}
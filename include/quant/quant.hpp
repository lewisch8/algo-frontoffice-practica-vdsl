#ifndef QUANT_HPP
#define QUANT_HPP

#include <boost/date_time/gregorian/gregorian.hpp>
#include <string>

// --- Módulo Daycount ---
#include "quant/daycount/Actual_360.hpp"
#include "quant/daycount/DayCountCalculator.hpp"
#include "quant/daycount/DayCountFactory.hpp"
#include "quant/daycount/Thirty_360.hpp"

// --- Módulo Description ---
#include "quant/description/InstrumentDescription.hpp"
#include "quant/description/LegDescription.hpp"

// --- Módulo Factory ---
#include "quant/factory/BondBuilder.hpp"
#include "quant/factory/Factory.hpp"
#include "quant/factory/FactoryRegistrator.hpp"
#include "quant/factory/SwapBuilder.hpp"

// --- Módulo Instruments ---
#include "quant/instruments/Bond.hpp"
#include "quant/instruments/Bootstraper.hpp"
#include "quant/instruments/Deposit.hpp"
#include "quant/instruments/FixedLeg.hpp"
#include "quant/instruments/FloatingLeg.hpp"
#include "quant/instruments/Instrument.hpp"
#include "quant/instruments/Leg.hpp"
#include "quant/instruments/Swap.hpp"

// --- Módulo Market ---
#include "quant/market/Index.hpp"
#include "quant/market/MarketCurve.hpp"
#include "quant/market/ZeroCouponCurve.hpp"

// --- Módulo Time ---
#include "quant/time/Calendar.hpp"
#include "quant/time/Schedule.hpp"


namespace Quant {

    namespace Conventions {
        inline const std::string ACT_360 = "ACT_360";
        inline const std::string THIRTY_360 = "THIRTY_360";
    }

    // Alias genérico para simplificar el uso de fechas al usuario
    using Date = boost::gregorian::date;

    // Utilidad: Crear fechas a partir de enteros (Año, Mes, Día)
    inline Date make_date(unsigned short year, unsigned short month, unsigned short day) {
        return Date(year, month, day);
    }

    // Utilidad: Crear fechas a partir de un string (ej. "2016-04-01")
    inline Date make_date(const std::string& date_str) {
        return boost::gregorian::from_string(date_str);
    }
    
    // Utilidad: Convertir la fecha a formato string para imprimir
    inline std::string to_string(const Date& d) {
        return boost::gregorian::to_iso_extended_string(d);
    }

    // --- 1. Abstracción de Mercado (Curvas e Índices) ---

    // Crea una curva a partir de una lista de pares {fecha, tasa}
    inline std::shared_ptr<Market::MarketCurve> make_curve(
        const std::string& ref_date_str,
        const std::vector<std::pair<std::string, double>>& rates)
    {
        auto curve = std::make_shared<Market::MarketCurve>(make_date(ref_date_str));
        for (const auto& r : rates) {
            curve->add_rate(make_date(r.first), r.second);
        }
        return curve;
    }

    // Crea un índice y le añade un fixing inicial
    inline std::shared_ptr<Market::Index> make_index(
        const std::string& name,
        std::shared_ptr<Market::MarketCurve> curve,
        int tenor_months,
        const std::string& fixing_date_str,
        double fixing_rate)
    {
        auto index = std::make_shared<Market::Index>(name, curve, tenor_months);
        index->add_fixing(make_date(fixing_date_str), fixing_rate);
        return index;
    }


    // --- 2. Abstracción de Patas (Legs) ---

    inline Description::LegDescription make_fixed_leg(
        const std::string& start_date, int payment_frequency, int tenor_years, 
        double notional, double fixed_rate, const std::string& daycount = Conventions::ACT_360) 
    {
        return Description::LegDescription(
            Description::LegDescription::LegType::Fixed, 
            start_date, payment_frequency, tenor_years, notional, fixed_rate, nullptr, daycount);
    }

    inline Description::LegDescription make_floating_leg(
        const std::string& start_date, int payment_frequency, int tenor_years, 
        double notional, std::shared_ptr<Market::Index> floating_index, const std::string& daycount = Conventions::ACT_360) 
    {
        return Description::LegDescription(
            Description::LegDescription::LegType::Floating, 
            start_date, payment_frequency, tenor_years, notional, 0.0, floating_index, daycount);
    }


    // --- 3. Abstracción de Instrumentos ---
    
    // Ahora make_swap recibe las dos patas ya construidas. 
    // Tú decides cuál es la pagadora (payer_leg) y cuál la receptora (receiver_leg).
    inline std::unique_ptr<Instruments::Swap> make_swap(
        const Description::LegDescription& payer_leg,
        const Description::LegDescription& receiver_leg,
        std::shared_ptr<Market::ZeroCouponCurve> discount_curve) 
    {
        Description::InstrumentDescription desc(
            Description::InstrumentDescription::Type::swap, 
            payer_leg, receiver_leg, discount_curve);
            
        auto instrument = Factory::Factory::instance()(desc);
        auto swap_ptr = dynamic_cast<Instruments::Swap*>(instrument.release());
        
        if (!swap_ptr) {
            throw std::runtime_error("Error Crítico: La Factory generó un instrumento que no es un Swap válido.");
        }
        
        return std::unique_ptr<Instruments::Swap>(swap_ptr);
    }

    // make_bond actualizado para usar la abstracción de patas
    inline std::unique_ptr<Instruments::Bond> make_bond(
        const Description::LegDescription& bond_leg,
        std::shared_ptr<Market::ZeroCouponCurve> discount_curve) 
    {
        Description::InstrumentDescription desc(
            Description::InstrumentDescription::Type::bond, 
            bond_leg, discount_curve);
            
        auto instrument = Factory::Factory::instance()(desc);
        auto bond_ptr = dynamic_cast<Instruments::Bond*>(instrument.release());
        
        if (!bond_ptr) {
            throw std::runtime_error("Error Crítico: La Factory generó un instrumento que no es un Bono válido.");
        }
        
        return std::unique_ptr<Instruments::Bond>(bond_ptr);
    }

    // --- Módulo de Reportes ---
    namespace Reports {
        inline void print_cashflows(const std::string& title, const std::vector<Instruments::CashFlow>& flows) {
            std::cout << "\n>>> " << title << " <<<" << std::endl;
            std::cout << std::left << std::setw(12) << "Fecha" << std::setw(10) << "YF" << std::setw(10) << "Tasa" << std::setw(15) << "Cupón" << "PV" << std::endl;
            std::cout << std::string(60, '-') << std::endl;
            for (const auto& cf : flows) {
                std::cout << std::left << std::setw(12) << Quant::to_string(cf.date)
                          << std::fixed << std::setprecision(4) << std::setw(10) << cf.year_fraction
                          << std::setw(10) << cf.rate << std::setw(15) << cf.amount << cf.pv << std::endl;
            }
        }

        inline void print_swap_report(Instruments::Swap& swap, Market::MarketCurve& curve, const std::string& name) {
            std::cout << "\n====================================================" << std::endl;
            std::cout << "       INFORME: " << name << std::endl;
            std::cout << "====================================================" << std::endl;
            std::cout << "NPV:          " << std::fixed << std::setprecision(2) << swap.price() << " EUR" << std::endl;
            std::cout << "Par Rate:     " << swap.calculate_par_rate() * 100.0 << " %" << std::endl;
            print_cashflows("PATA PAGADORA", swap.get_payer_leg().get_cashflows(curve));
            print_cashflows("PATA RECEPTORA", swap.get_receiver_leg().get_cashflows(curve));
        }
    }
}

#endif // QUANT_HPP
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

    using Date = boost::gregorian::date;

    /**
     * @brief Crea un objeto de fecha a partir de componentes numéricos.
     * @param year Año (ej. 2024).
     * @param month Mes (1-12).
     * @param day Día del mes (1-31).
     * @return Objeto Date (boost::gregorian::date).
     */
    inline Date make_date(unsigned short year, unsigned short month, unsigned short day) {
        return Date(year, month, day);
    }

    /**
     * @brief Crea un objeto de fecha a partir de una cadena de texto.
     * @param date_str Fecha en formato ISO (ej. "2024-12-31").
     * @return Objeto Date parseado.
     */
    inline Date make_date(const std::string& date_str) {
        return boost::gregorian::from_string(date_str);
    }
    
    /**
     * @brief Convierte una fecha a su representación en cadena de texto.
     * @param d Objeto fecha a convertir.
     * @return String en formato extendido ISO (YYYY-MM-DD).
     */
    inline std::string to_string(const Date& d) {
        return boost::gregorian::to_iso_extended_string(d);
    }

    // --- Abstracción de Mercado (Curvas e Índices) ---

    /**
     * @brief Construye una curva de mercado a partir de una serie de nodos de tipos de interés.
     * @param ref_date_str Fecha de referencia/valor de la curva.
     * @param rates Vector de pares {fecha_vencimiento, tasa_tnto_por_uno}.
     * @return Puntero compartido a la curva de mercado generada.
     */
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

    /**
     * @brief Inicializa un índice financiero y registra su primer fixing.
     * @param {string} nombre - Identificador único del índice (ej. "EURIBOR").
     * @param {Curve} curva - Objeto de curva de tipos para la valoración.
     * @param {int} frecuencia - Pagos por año (ej. 2 para semestral).
     * @param {string} fixingDate - Fecha del fixing en formato ISO (YYYY-MM-DD).
     * @param {double} fixingRate - Valor porcentual del ratio inicial.
     * @return Puntero compartido al índice configurado.
     */
    inline std::shared_ptr<Market::Index> make_index(
        const std::string& name,
        std::shared_ptr<Market::MarketCurve> curve,
        int frequency,
        const std::string& fixing_date_str,
        double fixing_rate)
    {
        auto index = std::make_shared<Market::Index>(name, curve, frequency);
        index->add_fixing(make_date(fixing_date_str), fixing_rate);
        return index;
    }


    // --- Abstracción de Patas ---

    /**
     * @brief Define la estructura de una pata de flujos fijos.
     * @param start_date Fecha de inicio de devengo.
     * @param payment_frequency Número de pagos anuales.
     * @param tenor_years Plazo de la pata en años.
     * @param notional Capital nominal del contrato.
     * @param fixed_rate Cupón fijo anual (en tanto por uno).
     * @param daycount Convención de conteo de días (por defecto ACT/360).
     * @return Objeto descriptivo de la pata fija.
     */
    inline Description::LegDescription make_fixed_leg(
        const std::string& start_date, int payment_frequency, int tenor_years, 
        double notional, double fixed_rate, const std::string& daycount = Conventions::ACT_360) 
    {
        return Description::LegDescription(
            Description::LegDescription::LegType::Fixed, 
            start_date, payment_frequency, tenor_years, notional, fixed_rate, nullptr, daycount);
    }

    /**
     * @brief Define la estructura de una pata de flujos flotantes vinculada a un índice.
     * @param start_date Fecha de inicio de devengo.
     * @param payment_frequency Número de pagos anuales.
     * @param tenor_years Plazo de la pata en años.
     * @param notional Capital nominal del contrato.
     * @param floating_index Puntero al índice de referencia (ej. Euribor).
     * @param daycount Convención de conteo de días (por defecto ACT/360).
     * @return Objeto descriptivo de la pata flotante.
     */
    inline Description::LegDescription make_floating_leg(
        const std::string& start_date, int payment_frequency, int tenor_years, 
        double notional, std::shared_ptr<Market::Index> floating_index, const std::string& daycount = Conventions::ACT_360) 
    {
        return Description::LegDescription(
            Description::LegDescription::LegType::Floating, 
            start_date, payment_frequency, tenor_years, notional, 0.0, floating_index, daycount);
    }


    // --- Abstracción de Instrumentos ---
    
    /**
     * @brief Crea un Swap financiero mediante la Factory a partir de dos definiciones de patas.
     * @param payer_leg Descripción de la pata pagadora.
     * @param receiver_leg Descripción de la pata receptora.
     * @param discount_curve Curva de cupón cero utilizada para el descuento de flujos.
     * @return Puntero único (unique_ptr) al Swap construido.
     * @throw std::runtime_error Si el instrumento generado no es del tipo Swap.
     */
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

    /**
     * @brief Crea un Bono mediante la Factory.
     * @param bond_leg Descripción de la pata de pagos del bono.
     * @param discount_curve Curva para el descuento de los cupones y el principal.
     * @return Puntero único al Bono construido.
     * @throw std::runtime_error Si el instrumento generado no es un Bono válido.
     */
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
        /**
         * @brief Imprime en consola el desglose de flujos de caja (Cashflows).
         * @param title Encabezado del reporte.
         * @param flows Vector de estructuras CashFlow con datos de fecha, tasa y valor presente.
         */
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
        
        /**
         * @brief Genera un informe detallado de un Swap, incluyendo NPV, Par Rate y flujos de ambas patas.
         * @param swap Referencia al objeto Swap a valorar.
         * @param curve Curva de mercado para proyectar tasas flotantes.
         * @param name Nombre descriptivo para el encabezado del informe.
         */
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
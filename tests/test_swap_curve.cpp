#include <boost/test/unit_test.hpp>
#include <memory>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "quant/instruments/Bootstraper.hpp"
#include "quant/instruments/Deposit.hpp"
#include "quant/instruments/Swap.hpp"
#include "quant/instruments/FixedLeg.hpp"
#include "quant/instruments/FloatingLeg.hpp"
#include "quant/market/MarketCurve.hpp"
#include "quant/market/Index.hpp"      
#include "quant/daycount/Actual_360.hpp"

using namespace Quant::Instruments;
using namespace Quant::Market;
using namespace Quant::Time;

BOOST_AUTO_TEST_CASE(test_swap_curve_calibration) {
    BOOST_TEST_MESSAGE("=== Iniciando calibración de la curva completa (Depósito + Swaps) ===");
    boost::gregorian::date ref_date(2016, 4, 1);
    auto curve = std::make_shared<MarketCurve>(ref_date);
    
    auto index = std::make_shared<Index>("Euribor6M", curve, 2);
    Actual_360 dc_calc;

    // Configuración del Depósito (6 meses, r = 5%)
    double r_dep = 0.05; 
    double t_dep = 0.5;  
    Deposit deposit(curve, r_dep, t_dep);

    BOOST_TEST_MESSAGE("Añadido Pilar 1: Depósito 6m (Tasa: " << r_dep * 100 << "%)");

    // Configuración de Swaps (Frecuencia = 2, semianual)
    double notional = 1000000.0;

    // Swap 12m (1 año) r = 5.5%
    Schedule schedule_12m("2016-04-01", 1.0, 2, dc_calc); 
    auto swap_12m = std::make_shared<Swap>(curve, 
        std::make_unique<FixedLeg>(schedule_12m, notional, 0.055), 
        std::make_unique<FloatingLeg>(schedule_12m, notional, index, 0.0));

    BOOST_TEST_MESSAGE("Añadido Pilar 2: Swap 12m (Tasa Fija: 5.5%)");

    // Swap 18m (1.5 años) r = 6.0%
    Schedule schedule_18m("2016-04-01", 1.5, 2, dc_calc); 
    auto swap_18m = std::make_shared<Swap>(curve, 
        std::make_unique<FixedLeg>(schedule_18m, notional, 0.06), 
        std::make_unique<FloatingLeg>(schedule_18m, notional, index, 0.0));

    BOOST_TEST_MESSAGE("Añadido Pilar 3: Swap 18m (Tasa Fija: 6.0%)");

    // Swap 2y (2 años) r = 6.4%
    Schedule schedule_24m("2016-04-01", 2.0, 2, dc_calc); 
    auto swap_24m = std::make_shared<Swap>(curve, 
        std::make_unique<FixedLeg>(schedule_24m, notional, 0.064), 
        std::make_unique<FloatingLeg>(schedule_24m, notional, index, 0.0));

    BOOST_TEST_MESSAGE("Añadido Pilar 4: Swap 24m (Tasa Fija: 6.4%)");

    std::vector<std::shared_ptr<Swap>> swaps = { swap_12m, swap_18m, swap_24m };

    // Ejecución del algoritmo de Bootstrapping
    std::vector<DiscountFactor> z_curve = Bootstrapper::bootstrapCurve(deposit, swaps);

    // Verificaciones
    BOOST_REQUIRE_EQUAL(z_curve.size(), 4); // Debe haber generado 4 puntos
    
    // Test: Factor del depósito (Iteración 0)
    double expected_z_dep = 1.0 / (1.0 + r_dep * t_dep);
    BOOST_TEST_MESSAGE("[Pilar 1 - 6m] Z calculado: " << z_curve[0].value << " | Z teórico: " << expected_z_dep);
    BOOST_CHECK_CLOSE(z_curve[0].value, expected_z_dep, 1e-4);

    // Test: Factor del Swap a 12m (Iteración 1)
    double sum_Z_dt_1 = expected_z_dep * t_dep;
    double dt_12m = swap_12m->get_payer_leg().get_schedule().get_year_fractions().back();
    double expected_z_12m = (1.0 - 0.055 * sum_Z_dt_1) / (1.0 + 0.055 * dt_12m);
    BOOST_TEST_MESSAGE("[Pilar 2 - 12m] Z calculado: " << z_curve[1].value << " | Z teórico: " << expected_z_12m);
    BOOST_CHECK_CLOSE(z_curve[1].value, expected_z_12m, 1e-4);

    // Test: Factor del Swap a 18m (Iteración 2)
    double sum_Z_dt_2 = sum_Z_dt_1 + expected_z_12m * dt_12m;
    double dt_18m = swap_18m->get_payer_leg().get_schedule().get_year_fractions().back();
    double expected_z_18m = (1.0 - 0.060 * sum_Z_dt_2) / (1.0 + 0.060 * dt_18m);
    BOOST_TEST_MESSAGE("[Pilar 3 - 18m] Z calculado: " << z_curve[2].value << " | Z teórico: " << expected_z_18m);
    BOOST_CHECK_CLOSE(z_curve[2].value, expected_z_18m, 1e-4);

    // Test: Factor del Swap a 2y (Iteración 3)
    double sum_Z_dt_3 = sum_Z_dt_2 + expected_z_18m * dt_18m;
    double dt_24m = swap_24m->get_payer_leg().get_schedule().get_year_fractions().back();
    double expected_z_24m = (1.0 - 0.064 * sum_Z_dt_3) / (1.0 + 0.064 * dt_24m);
    BOOST_TEST_MESSAGE("[Pilar 4 - 24m] Z calculado: " << z_curve[3].value << " | Z teórico: " << expected_z_24m);
    BOOST_CHECK_CLOSE(z_curve[3].value, expected_z_24m, 1e-4);

    BOOST_TEST_MESSAGE("=== Todos los factores de descuento coinciden con los cálculos teóricos ===");
}
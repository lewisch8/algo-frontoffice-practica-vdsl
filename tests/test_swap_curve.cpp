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

BOOST_AUTO_TEST_CASE(test_swap_bootstrapping_logic) {
    boost::gregorian::date ref_date(2016, 4, 1);
    auto curve = std::make_shared<MarketCurve>(ref_date);
    
    auto index = std::make_shared<Index>("Euribor6M", curve, 2);

    // Depósito Inicial
    double r_dep = 0.05; 
    double t_dep = 0.5;  
    Deposit deposit(curve, r_dep, t_dep);

    // Configuración Swap a 12m (1 año, frecuencia semianual -> 2) al 5.5%
    Actual_360 dc_calc; 
    Schedule schedule_1y("2016-04-01", 1, 2, dc_calc); 

    double r_swap = 0.055;
    auto fixed_leg_1y = std::make_unique<FixedLeg>(schedule_1y, 1000000.0, r_swap);
    auto float_leg_1y = std::make_unique<FloatingLeg>(schedule_1y, 1000000.0, index, 0.0);
    
    // Construcción del Swap inyectando ambas patas
    auto swap_1y = std::make_shared<Swap>(curve, std::move(fixed_leg_1y), std::move(float_leg_1y));
    std::vector<std::shared_ptr<Swap>> swaps = { swap_1y };

    // Ejecución del algoritmo de Bootstrapping
    std::vector<DiscountFactor> z_curve = Bootstrapper::bootstrapCurve(deposit, swaps);

    // Verificaciones
    BOOST_REQUIRE_EQUAL(z_curve.size(), 2); // Debe haber generado 2 puntos: [Deposit Z, Swap Z]
    
    // Comprobacion de la primera iteración
    double expected_z1 = 1.0 / (1.0 + r_dep * t_dep);

    // Test de la lógica matemática para el Swap
    double sum_Z_dt = expected_z1 * t_dep;
    
    double dt2 = swap_1y->get_payer_leg().get_schedule().get_year_fractions().back();
    
    double expected_z2 = (1.0 - r_swap * sum_Z_dt) / (1.0 + r_swap * dt2);
    
    BOOST_CHECK_CLOSE(z_curve[1].value, expected_z2, 1e-4);
}
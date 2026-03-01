#include <boost/test/unit_test.hpp>
#include <memory>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "quant/instruments/Deposit.hpp"
#include "quant/instruments/Bootstraper.hpp"
#include "quant/instruments/Swap.hpp"
#include "quant/market/MarketCurve.hpp"

using namespace Quant::Instruments;
using namespace Quant::Market;

BOOST_AUTO_TEST_CASE(test_deposit_only_bootstrapping) {
    boost::gregorian::date ref_date(2016, 4, 1);
    auto curve = std::make_shared<MarketCurve>(ref_date);

    // Configuración del Depósito (6 meses, r = 5%)
    double r_dep = 0.05; 
    double t_dep = 0.5;  
    Deposit deposit(curve, r_dep, t_dep);

    // Comprobación de propiedades básicas del depósito
    BOOST_CHECK_EQUAL(deposit.getRate(), 0.05);
    BOOST_CHECK_EQUAL(deposit.getMaturity(), 0.5);
    BOOST_CHECK_EQUAL(deposit.price(), 0.0);

    // Ejecución del Bootstrapper
    std::vector<std::shared_ptr<Swap>> empty_swaps;
    std::vector<DiscountFactor> z_curve = Bootstrapper::bootstrapCurve(deposit, empty_swaps);

    // Verificación de la curva con un solo 1 punto
    BOOST_REQUIRE_EQUAL(z_curve.size(), 1);

    // Verificación del cálculo matemático Z = 1 / (1 + r * t)
    double expected_z = 1.0 / (1.0 + r_dep * t_dep); // 1.0 / 1.025
    
    BOOST_CHECK_CLOSE(z_curve[0].time, 0.5, 1e-4);
    BOOST_CHECK_CLOSE(z_curve[0].value, expected_z, 1e-4);
}
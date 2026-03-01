#include <boost/test/unit_test.hpp>
#include <memory>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "quant/instruments/Deposit.hpp"
#include "quant/instruments/Bootstraper.hpp"
#include "quant/market/MarketCurve.hpp"

using namespace Quant::Instruments;
using namespace Quant::Market;

BOOST_AUTO_TEST_CASE(test_deposit_only_bootstrapping) {
    BOOST_TEST_MESSAGE("=== Iniciando calibración de la curva (Solo Depósito) ===");
    // Fecha inicial de calibración requerida: 1/4/2016
    boost::gregorian::date ref_date(2016, 4, 1);
    auto curve = std::make_shared<MarketCurve>(ref_date);

    // Configuración del Depósito (6 meses, r = 5%)
    double r_dep = 0.05; 
    double t_dep = 0.5;  
    BOOST_TEST_MESSAGE("Instrumento configurado: Depósito a " << t_dep * 12 << " meses. Tasa: " << r_dep * 100 << "%");

    Deposit deposit(curve, r_dep, t_dep);

    // Ejecución del Bootstrapper sin swaps
    std::vector<std::shared_ptr<Swap>> empty_swaps;
    std::vector<DiscountFactor> z_curve = Bootstrapper::bootstrapCurve(deposit, empty_swaps);

    BOOST_REQUIRE_EQUAL(z_curve.size(), 1);

    // Verificación del factor de descuento esperado
    double expected_z = 1.0 / (1.0 + r_dep * t_dep); 

    BOOST_TEST_MESSAGE("Factor de Descuento (Z) calculado por el algoritmo: " << z_curve[0].value);
    BOOST_TEST_MESSAGE("Factor de Descuento (Z) esperado teóricamente:    " << expected_z);
    
    BOOST_CHECK_CLOSE(z_curve[0].time, 0.5, 1e-4);
    BOOST_CHECK_CLOSE(z_curve[0].value, expected_z, 1e-4);

    BOOST_TEST_MESSAGE("=== Calibración de Depósito completada correctamente ===");
}
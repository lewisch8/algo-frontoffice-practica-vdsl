#include <boost/test/unit_test.hpp>
#include "quant/market/MarketCurve.hpp"
#include "quant/instruments/FixedLeg.hpp"
#include "quant/daycount/DayCountFactory.hpp"

BOOST_AUTO_TEST_CASE(test_cashflow_amounts) {
    BOOST_TEST_MESSAGE("=== Iniciando test de flujos de caja en pata fija ===");
    using namespace Quant;
    boost::gregorian::date ref_date(2016, 4, 1);
    BOOST_TEST_MESSAGE("Fecha de referencia: " << ref_date);
    auto curva = std::make_shared<Market::MarketCurve>(ref_date);
    
    boost::gregorian::date fecha1 = boost::gregorian::from_string("2016-10-03");
    boost::gregorian::date fecha2 = boost::gregorian::from_string("2017-04-03");
    curva->add_rate(fecha1, 0.0474);
    curva->add_rate(fecha2, 0.0500);

    auto calc = DayCountFactory::create(DayCountFactory::Convention::ACT_360);
    Time::Schedule sched("2016-04-01", 1, 2, *calc);
    
    double nominal = 100e6;  // 100 millones
    double tipo_fijo = 0.05;

    Instruments::FixedLeg leg(std::move(sched), nominal, tipo_fijo);
    auto flujos = leg.get_cashflows(*curva);
    
    for (size_t i = 0; i < flujos.size(); ++i) {
        BOOST_TEST_MESSAGE("Flujo #" << (i+1) << ":");
        BOOST_TEST_MESSAGE(" - Fracción de año: " << flujos[i].year_fraction);
        BOOST_TEST_MESSAGE(" - Importe (sin descontar): " << flujos[i].amount << " EUR");
    }
    
    // Comparando por el primer flujo
    double expected = nominal * tipo_fijo * flujos[0].year_fraction;    
    BOOST_TEST_MESSAGE("VERIFICACIÓN DEL PRIMER FLUJO:");
    BOOST_TEST_MESSAGE(" - Cálculo esperado: nominal * tipo_fijo * fracción = " << expected << " EUR");
    BOOST_TEST_MESSAGE(" - Importe calculado por get_cashflows(): " << flujos[0].amount << " EUR");
    BOOST_TEST_MESSAGE(" - Diferencia: " << (flujos[0].amount - expected) << " EUR");
    
    BOOST_CHECK_CLOSE(flujos[0].amount, expected, 0.01);
}
